#ifndef _ERRORPIPE_H_
#define _ERRORPIPE_H_


#include <tchar.h>
#include <Windows.h>
#include <cassert>

#include <thread>
#include <mutex>
#include <condition_variable>
#include <chrono>

#include <Common/Container.h>
#include <Common/String.h>
#include <Error/Error.h>


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


namespace JBF{
    namespace ErrorPipe{
        static constexpr TCHAR pipePath[] = _T("\\\\.\\pipe\\jbf_pipe");
        static constexpr size_t pipeBufferSize = 1024;


        template<typename CHARTYPE = TCHAR>
        class Server{
        public:
            Server(unsigned long id)
                : pipeHandle(nullptr)
                , bConnected(false)
            {
                TCHAR curPath[std::size(pipePath) + 128] = { 0 };
                memcpy_s(curPath, sizeof(curPath), pipePath, sizeof(pipePath));

                const Common::String mainHandleStr = Common::ToString(id);
                memcpy_s(curPath + std::size(pipePath) - 1, (std::size(curPath) - std::size(pipePath) + 1) * sizeof(TCHAR), mainHandleStr.c_str(), mainHandleStr.length() * sizeof(TCHAR));
                
                pipeHandle = CreateNamedPipe(
                    curPath
                    , PIPE_ACCESS_INBOUND
                    , PIPE_TYPE_MESSAGE | PIPE_READMODE_MESSAGE | PIPE_WAIT
                    , 1
                    , (pipeBufferSize * sizeof(CHARTYPE)) + 1
                    , (pipeBufferSize * sizeof(CHARTYPE)) + 1
                    , 0
                    , nullptr
                    );
                if(pipeHandle == INVALID_HANDLE_VALUE)
                    pipeHandle = nullptr;

                if(!pipeHandle)
                    return;
            }
            ~Server(){
                if(pipeHandle){
                    if(bConnected){
                        DisconnectNamedPipe(pipeHandle);
                        bConnected = false;
                    }
                    
                    CloseHandle(pipeHandle);
                }
            }


        public:
            inline bool IsValid()const{ return (pipeHandle != nullptr); }

        public:
            bool WaitUntilConnected(){
                for(;;){
                    if(ConnectNamedPipe(pipeHandle, nullptr))
                        break;
                    if(GetLastError() == ERROR_PIPE_CONNECTED){
                        pipeHandle = nullptr;
                        return false;
                    }
                }
                bConnected = true;
                return true;
            }
            
            bool Read(Common::String<CHARTYPE>& result){
                result.clear();
                
                if(!bConnected)
                    return false;
                
                unsigned char rawBuffer[(pipeBufferSize * sizeof(CHARTYPE)) + 1] = { 0 };
                CHARTYPE* buffer = reinterpret_cast<CHARTYPE*>(rawBuffer + 1);

                DWORD bytesRead = 0;
                if(!PeekNamedPipe(pipeHandle, nullptr, 0, nullptr, &bytesRead, nullptr))
                    return false;
                if(bytesRead < sizeof(rawBuffer))
                    return false;

                for(;;){
                    bytesRead = 0;
                    if(!ReadFile(
                        pipeHandle
                        , rawBuffer
                        , sizeof(rawBuffer)
                        , &bytesRead,
                        nullptr
                        ))
                        break;

                    if(bytesRead != sizeof(rawBuffer)){
                        Error::ShowFatalMessage(Error::FatalCode::ERRORPIPE_SERVER_READ_MISMATCH, sizeof(rawBuffer), bytesRead);
                        assert(false);
                        return false;
                    }

                    if((*rawBuffer) == 0xff){
                        result += buffer;
                        break;
                    }
                    else if((*rawBuffer) == 0x00){
                        result.resize(result.size() + pipeBufferSize);
                        memcpy_s(result.data() + (result.size() - pipeBufferSize), pipeBufferSize, buffer, pipeBufferSize);
                    }
                }

                return !result.empty();
            }

            
        private:
            HANDLE pipeHandle;
            bool bConnected;
        };

        template<typename CHARTYPE = TCHAR>
        class Client{
        public:
            Client(unsigned long id, unsigned long waitTimeInMillisecond)
                : thread(ThreadWork, this)
                , bExit(false)
                , pipeHandle(nullptr)
            {
                TCHAR curPath[std::size(pipePath) + 128] = { 0 };
                memcpy_s(curPath, sizeof(curPath), pipePath, sizeof(pipePath));

                const Common::String mainHandleStr = Common::ToString(id);
                memcpy_s(curPath + std::size(pipePath) - 1, (std::size(curPath) - std::size(pipePath) + 1) * sizeof(TCHAR), mainHandleStr.c_str(), mainHandleStr.length() * sizeof(TCHAR));

                const std::chrono::steady_clock::time_point lateTime(std::chrono::steady_clock::now());
                for(;;){
                    pipeHandle = CreateFile(
                        curPath
                        , GENERIC_WRITE
                        , 0
                        , nullptr
                        , OPEN_EXISTING
                        , 0
                        , nullptr
                        );
                    if(pipeHandle == INVALID_HANDLE_VALUE)
                        pipeHandle = nullptr;

                    if(pipeHandle)
                        break;

                    const std::chrono::steady_clock::time_point currentTime(std::chrono::steady_clock::now());
                    if(std::chrono::duration_cast<std::chrono::milliseconds>(currentTime - lateTime).count() > waitTimeInMillisecond)
                        break;
                }
            }
            ~Client(){
                {
                    std::unique_lock lock(mutex);
                    bExit = true;
                }
                conVal.notify_one();
                
                thread.join();
                
                if(pipeHandle)
                    CloseHandle(pipeHandle);
            }


        public:
            inline bool IsValid()const{ return (pipeHandle != nullptr); }

        public:
            void PushMessage(const Common::String<CHARTYPE>& message){
                {
                    std::unique_lock lock(mutex);
                    messageQueue.emplace_back(message);
                }
                conVal.notify_one();
            }
            void PushMessage(Common::String<CHARTYPE>&& message){
                {
                    std::unique_lock lock(mutex);
                    messageQueue.emplace(std::move(message));
                }
                conVal.notify_one();
            }


        private:
            static void ThreadWork(Client* _this){
                for(;;){
                    std::unique_lock lock(_this->mutex);
                    _this->conVal.wait(lock, [_this]{
                        return (_this->bExit || (!_this->messageQueue.empty()));
                    });

                    while(!_this->messageQueue.empty()){
                        _this->Write(_this->messageQueue.front());
                        _this->messageQueue.pop();
                    }

                    if(_this->bExit)
                        break;
                }
            }

            
        private:
            void Write(const Common::String<CHARTYPE>& message){
                unsigned char rawBuffer[(pipeBufferSize * sizeof(CHARTYPE)) + 1] = { 0 };
                CHARTYPE* buffer = reinterpret_cast<CHARTYPE*>(rawBuffer + 1);
                
                const size_t stringLen = message.length();

                for(size_t cur = 0;;){
                    const size_t next = cur + pipeBufferSize;
                    if(next >= stringLen){
                        (*rawBuffer) = 0xff;

                        const size_t strLen = stringLen - cur;
                        memcpy_s(buffer, pipeBufferSize * sizeof(CHARTYPE), message.c_str() + cur, strLen * sizeof(CHARTYPE));

                        static constexpr CHARTYPE nullBuffer[pipeBufferSize] = { 0 };
                        memcpy_s(buffer + strLen, (pipeBufferSize - strLen) * sizeof(CHARTYPE), nullBuffer, (pipeBufferSize - strLen) * sizeof(CHARTYPE));
                        
                        DWORD bytesWritten = 0;
                        if(!WriteFile(
                            pipeHandle
                            , &rawBuffer
                            , sizeof(rawBuffer)
                            , &bytesWritten
                            , nullptr
                            ))
                        {
                            Error::ShowFatalMessage(Error::FatalCode::ERRORPIPE_CLIENT_WRITE_FAILED);
                            assert(false);
                            return;
                        }
                        if(bytesWritten != sizeof(rawBuffer)){
                            Error::ShowFatalMessage(Error::FatalCode::ERRORPIPE_CLIENT_WRITE_MISMATCH, sizeof(rawBuffer), bytesWritten);
                            assert(false);
                            return;
                        }
                        
                        break;
                    }
                    else{
                        (*rawBuffer) = 0x00;

                        memcpy_s(buffer, pipeBufferSize * sizeof(CHARTYPE), message.c_str() + cur, pipeBufferSize * sizeof(CHARTYPE));
                        
                        DWORD bytesWritten = 0;
                        if(!WriteFile(
                            pipeHandle
                            , &rawBuffer
                            , sizeof(rawBuffer)
                            , &bytesWritten
                            , nullptr
                            ))
                        {
                            Error::ShowFatalMessage(Error::FatalCode::ERRORPIPE_CLIENT_WRITE_FAILED);
                            assert(false);
                            return;
                        }
                        if(bytesWritten != sizeof(rawBuffer)){
                            Error::ShowFatalMessage(Error::FatalCode::ERRORPIPE_CLIENT_WRITE_MISMATCH, sizeof(rawBuffer), bytesWritten);
                            assert(false);
                            return;
                        }
                        
                        cur = next;
                    }
                }
            }


        private:
            std::thread thread;
            std::mutex mutex;
            std::condition_variable conVal;
            bool bExit;
            
            Common::Queue<Common::String<CHARTYPE>> messageQueue;
            
        private:
            HANDLE pipeHandle;
        };
    };
};


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


#endif


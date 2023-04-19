#ifndef _ERRORPIPE_H_
#define _ERRORPIPE_H_


#include <tchar.h>
#include <Windows.h>
#include <cassert>

#include <thread>
#include <mutex>
#include <condition_variable>

#include <Common/Container.h>
#include <Common/String.h>
#include <Error/Error.h>


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


namespace JBF{
    namespace ErrorPipe{
        static constexpr TCHAR PipePath[] = _T("\\\\.\\pipe\\jbf_pipe");
        static constexpr size_t PipeBufferSize = 1024;


        template<typename CHARTYPE = TCHAR>
        class Server{
        public:
            Server(unsigned long ID)
                : PipeHandle(nullptr)
                , bConnected(false)
            {
                TCHAR CurPath[std::size(PipePath) + 128] = { 0 };
                memcpy_s(CurPath, sizeof(CurPath), PipePath, sizeof(PipePath));

                const Common::String MainHandleStr = Common::ToString(ID);
                memcpy_s(CurPath + std::size(PipePath) - 1, (std::size(CurPath) - std::size(PipePath) + 1) * sizeof(TCHAR), MainHandleStr.c_str(), MainHandleStr.length() * sizeof(TCHAR));
                
                PipeHandle = CreateNamedPipe(
                CurPath
                    , PIPE_ACCESS_INBOUND
                    , PIPE_READMODE_MESSAGE | PIPE_WAIT
                    , 1
                    , (PipeBufferSize * sizeof(CHARTYPE)) + 1
                    , (PipeBufferSize * sizeof(CHARTYPE)) + 1
                    , 0
                    , nullptr
                    );
                if(PipeHandle == INVALID_HANDLE_VALUE)
                    PipeHandle = nullptr;

                if(!PipeHandle)
                    return;
            }
            ~Server(){
                if(PipeHandle){
                    if(bConnected){
                        DisconnectNamedPipe(PipeHandle);
                        bConnected = false;
                    }
                    
                    CloseHandle(PipeHandle);
                }
            }


        public:
            inline bool IsValid()const{ return (PipeHandle != nullptr); }

        public:
            bool WaitUntilConnected(){
                for(;;){
                    if(ConnectNamedPipe(PipeHandle, nullptr))
                        break;
                    if(GetLastError() == ERROR_PIPE_CONNECTED){
                        PipeHandle = nullptr;
                        return false;
                    }
                }
                bConnected = true;
                return true;
            }
            
            bool Read(Common::String<CHARTYPE>& Result){
                Result.clear();
                
                if(!bConnected)
                    return false;
                
                unsigned char RawBuffer[(PipeBufferSize * sizeof(CHARTYPE)) + 1] = { 0 };
                CHARTYPE* Buffer = reinterpret_cast<CHARTYPE*>(RawBuffer + 1);

                for(;;){
                    DWORD BytesRead = 0;
                    if(!ReadFile(
                        PipeHandle
                        , RawBuffer
                        , sizeof(RawBuffer)
                        , &BytesRead,
                        nullptr
                        ))
                    {
                        break;
                    }
                    if(BytesRead != sizeof(RawBuffer)){
                        Error::ShowFatalMessage(Error::FatalCode::ERRORPIPE_SERVER_READ_MISMATCH, sizeof(RawBuffer), BytesRead);
                        assert(false);
                        return false;
                    }

                    if((*RawBuffer) == 0xff){
                        Result += Buffer;
                        break;
                    }
                    else if((*RawBuffer) == 0x00){
                        Result.resize(Result.size() + PipeBufferSize);
                        memcpy_s(Result.data() + (Result.size() - PipeBufferSize), PipeBufferSize, Buffer, PipeBufferSize);
                    }
                }

                return !Result.empty();
            }

            
        private:
            HANDLE PipeHandle;
            bool bConnected;
        };

        template<typename CHARTYPE = TCHAR>
        class Client{
        public:
            Client(unsigned long ID)
                : Thread(ThreadWork, this)
                , bExit(false)
                , PipeHandle(nullptr)
            {
                TCHAR CurPath[std::size(PipePath) + 128] = { 0 };
                memcpy_s(CurPath, sizeof(CurPath), PipePath, sizeof(PipePath));

                const Common::String MainHandleStr = Common::ToString(ID);
                memcpy_s(CurPath + std::size(PipePath) - 1, (std::size(CurPath) - std::size(PipePath) + 1) * sizeof(TCHAR), MainHandleStr.c_str(), MainHandleStr.length() * sizeof(TCHAR));
                
                PipeHandle = CreateFile(
                    CurPath
                    , GENERIC_WRITE
                    , 0
                    , nullptr
                    , OPEN_EXISTING
                    , 0
                    , nullptr
                    );
                if(PipeHandle == INVALID_HANDLE_VALUE)
                    PipeHandle = nullptr;
            }
            ~Client(){
                {
                    std::unique_lock Lock(Mutex);
                    bExit = true;
                }
                Switch.notify_one();
                
                Thread.join();
                
                if(PipeHandle)
                    CloseHandle(PipeHandle);
            }


        public:
            inline bool IsValid()const{ return (PipeHandle != nullptr); }

        public:
            void PushMessage(const Common::String<CHARTYPE>& Message){
                {
                    std::unique_lock Lock(Mutex);
                    MessageQueue.emplace_back(Message);
                }
                Switch.notify_one();
            }
            void PushMessage(Common::String<CHARTYPE>&& Message){
                {
                    std::unique_lock Lock(Mutex);
                    MessageQueue.emplace_back(std::move(Message));
                }
                Switch.notify_one();
            }


        private:
            static void ThreadWork(Client* This){
                for(;;){
                    std::unique_lock Lock(This->Mutex);
                    This->Switch.wait(Lock, [This]{
                        return (This->bExit || (!This->MessageQueue.empty()));
                    });

                    while(!This->MessageQueue.empty()){
                        This->Write(This->MessageQueue.front());
                        This->MessageQueue.pop_front();
                    }

                    if(This->bExit)
                        break;
                }
            }

            
        private:
            void Write(const Common::String<CHARTYPE>& Message){
                unsigned char RawBuffer[(PipeBufferSize * sizeof(CHARTYPE)) + 1] = { 0 };
                CHARTYPE* Buffer = reinterpret_cast<CHARTYPE*>(RawBuffer + 1);
                
                const size_t StringLen = Message.length();

                for(size_t Cur = 0;;){
                    const size_t Next = Cur + PipeBufferSize;
                    if(Next >= StringLen){
                        (*RawBuffer) = 0xff;

                        const size_t StrLen = StringLen - Cur;
                        memcpy_s(Buffer, PipeBufferSize * sizeof(CHARTYPE), Message.c_str() + Cur, StrLen * sizeof(CHARTYPE));

                        static constexpr CHARTYPE NullBuffer[PipeBufferSize] = { 0 };
                        memcpy_s(Buffer + StrLen, (PipeBufferSize - StrLen) * sizeof(CHARTYPE), NullBuffer, (PipeBufferSize - StrLen) * sizeof(CHARTYPE));
                        
                        DWORD BytesWritten = 0;
                        if(!WriteFile(
                            PipeHandle
                            , &RawBuffer
                            , sizeof(RawBuffer)
                            , &BytesWritten
                            , nullptr
                            ))
                        {
                            Error::ShowFatalMessage(Error::FatalCode::ERRORPIPE_CLIENT_WRITE_FAILED);
                            assert(false);
                            return;
                        }
                        if(BytesWritten != sizeof(RawBuffer)){
                            Error::ShowFatalMessage(Error::FatalCode::ERRORPIPE_CLIENT_WRITE_MISMATCH, sizeof(RawBuffer), BytesWritten);
                            assert(false);
                            return;
                        }
                        
                        break;
                    }
                    else{
                        (*RawBuffer) = 0x00;

                        memcpy_s(Buffer, PipeBufferSize * sizeof(CHARTYPE), Message.c_str() + Cur, PipeBufferSize * sizeof(CHARTYPE));
                        
                        DWORD BytesWritten = 0;
                        if(!WriteFile(
                            PipeHandle
                            , &RawBuffer
                            , sizeof(RawBuffer)
                            , &BytesWritten
                            , nullptr
                            ))
                        {
                            Error::ShowFatalMessage(Error::FatalCode::ERRORPIPE_CLIENT_WRITE_FAILED);
                            assert(false);
                            return;
                        }
                        if(BytesWritten != sizeof(RawBuffer)){
                            Error::ShowFatalMessage(Error::FatalCode::ERRORPIPE_CLIENT_WRITE_MISMATCH, sizeof(RawBuffer), BytesWritten);
                            assert(false);
                            return;
                        }
                        
                        Cur = Next;
                    }
                }
            }


        private:
            std::thread Thread;
            std::mutex Mutex;
            std::condition_variable Switch;
            bool bExit;
            
            Common::Queue<Common::String<CHARTYPE>> MessageQueue;
            
        private:
            HANDLE PipeHandle;
        };
    };
};


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


#endif _ERRORPIPE_H_


#ifndef _ERRORPIPE_H_
#define _ERRORPIPE_H_


#include <tchar.h>
#include <Windows.h>
#include <cassert>

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

                for(;;){
                    if(ConnectNamedPipe(PipeHandle, nullptr))
                        break;
                    if(GetLastError() == ERROR_PIPE_CONNECTED){
                        PipeHandle = nullptr;
                        break;
                    }
                }
            }
            ~Server(){
                if(PipeHandle){
                    DisconnectNamedPipe(PipeHandle);
                    CloseHandle(PipeHandle);
                }
            }


        public:
            inline bool IsValid()const{ return (PipeHandle != nullptr); }

        public:
            bool Read(Common::String<CHARTYPE>& Result){
                unsigned char RawBuffer[(PipeBufferSize * sizeof(CHARTYPE)) + 1] = { 0 };
                CHARTYPE* Buffer = reinterpret_cast<CHARTYPE*>(RawBuffer + 1);

                Result.clear();

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
        };

        template<typename CHARTYPE = TCHAR>
        class Client{
        public:
            Client(unsigned long ID)
                : PipeHandle(nullptr)
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
                if(PipeHandle){
                    CloseHandle(PipeHandle);
                }
            }


        public:
            inline bool IsValid()const{ return (PipeHandle != nullptr); }

        public:
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
            HANDLE PipeHandle;
        };
    };
};


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


#endif _ERRORPIPE_H_


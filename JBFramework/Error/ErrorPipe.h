#ifndef _ERRORPIPE_H_
#define _ERRORPIPE_H_


#include <tchar.h>
#include <Windows.h>


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


namespace JBF{
    namespace ErrorPipe{
        static constexpr TCHAR PipePath[] = _T("\\\\.\\pipe\\jbf_pipe");
        static constexpr size_t PipeBufferSize = 4096;

        
        class Server{
        public:
            Server()
                : PipeHandle(nullptr)
            {
                PipeHandle = CreateNamedPipe(
                    PipePath
                    , PIPE_ACCESS_DUPLEX
                    , PIPE_TYPE_MESSAGE | PIPE_READMODE_MESSAGE | PIPE_WAIT
                    , PIPE_UNLIMITED_INSTANCES
                    , PipeBufferSize * sizeof(TCHAR)
                    , PipeBufferSize * sizeof(TCHAR)
                    , 0
                    , nullptr
                    );
                if(PipeHandle == INVALID_HANDLE_VALUE)
                    PipeHandle = nullptr;
            }
            ~Server(){
                if(PipeHandle){
                    DisconnectNamedPipe(PipeHandle);
                    CloseHandle(PipeHandle);
                }
            }


        public:
            inline bool IsValid()const{ return (PipeHandle != nullptr); }

            
        private:
            HANDLE PipeHandle;
        };
        
        class Client{
        public:
            Client()
                : PipeHandle(nullptr)
            {
                PipeHandle = CreateFile(
                    PipePath
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
            void Write(const TCHAR* Message){
                DWORD BytesWritten = 0;
                WriteFile(
                    PipeHandle
                    , Message
                    , (DWORD)(_tcslen(Message) * sizeof(TCHAR))
                    , &BytesWritten
                    , nullptr
                    );
            }

            
        private:
            HANDLE PipeHandle;
        };
    };
};


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


#endif _ERRORPIPE_H_


#ifndef _FRAMES_H_
#define _FRAMES_H_


#include <tchar.h>
#include <Windows.h>

#include <Error/Error.h>
#include <../JBFramework/Error/ErrorPipe.h>


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


namespace JBF{
    namespace Frame{
        namespace __hidden{
            class FrameBase{
                FrameBase();
                virtual ~FrameBase();


            public:
                inline bool IsLoggerValid(void** OutHandle)const{
                    (*OutHandle) = LoggerPI.hProcess;
                    
                    if(!LoggerPI.hProcess)
                        return false;

                    DWORD ExitCode;
                    if(!GetExitCodeProcess(LoggerPI.hProcess, &ExitCode))
                        return false;

                    if(ExitCode != STILL_ACTIVE)
                        return false;
                    
                    return true;
                }
                inline bool IsLoggerValid()const{
                    void* Dummy = nullptr;
                    return IsLoggerValid(&Dummy);
                }
                
                
            private:
                PROCESS_INFORMATION LoggerPI;
            };
        };
        
        class WindowFrame : public __hidden::FrameBase{
        public:
            WindowFrame(void* InstanceHandle, const TCHAR* AppName, unsigned Width, unsigned Height);


        public:
            inline bool IsValid()const{ return WindowHandle != nullptr; }


        private:
            static LRESULT CALLBACK MessageProcessor(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

            
        private:
            ErrorPipe::Client<TCHAR> ErrorPipeClient;

        private:
            HWND WindowHandle;
        };
    };
};


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


#endif _FRAMES_H_


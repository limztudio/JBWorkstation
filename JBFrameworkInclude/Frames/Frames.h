#ifndef _FRAMES_H_
#define _FRAMES_H_


#include <tchar.h>
#include <Windows.h>

#include <Error/Error.h>
#include <../JBFramework/Error/ErrorPipe.h>


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


namespace JBF{
    extern void PushLog(const Common::String<TCHAR>& Message);
    extern void PushLog(Common::String<TCHAR>&& Message);
    extern void PushLog(const TCHAR* Message, ...);
    extern void PushLog(const Common::StringView<TCHAR>& Message, ...);

    extern void PushError(Error::ErrorCode Code);
    extern void PushError(Error::ErrorCode Code, ...);
    
    
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
        private:
            static constexpr size_t ReservedFrameSize = 4;

            
        public:
            WindowFrame(void* InstanceHandle, const TCHAR* AppName, unsigned Width, unsigned Height);
            virtual ~WindowFrame()override;


        public:
            inline bool IsValid()const{ return WindowHandle != nullptr; }
            inline bool IsActive()const{ return bIsActive; }


        public:
            bool Init();
            bool Run();


        private:
            bool InitInternal();
            bool UpdateInternal(float TimeDelta);


        private:
            static LRESULT CALLBACK MessageProcessor(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

            
        private:
            ErrorPipe::Client<TCHAR> ErrorPipeClient;

        private:
            HWND WindowHandle;
            bool bIsActive;


        public:
            friend void JBF::PushLog(const Common::String<TCHAR>& Message);
            friend void JBF::PushLog(Common::String<TCHAR>&& Message);
            friend void JBF::PushLog(const TCHAR* Message, ...);
            friend void JBF::PushLog(const Common::StringView<TCHAR>& Message, ...);

            friend void JBF::PushError(Error::ErrorCode Code);
            friend void JBF::PushError(Error::ErrorCode Code, ...);
        };
    };


    extern Frame::WindowFrame* MainFrame;


    inline void PushLog(const Common::String<TCHAR>& Message){
        MainFrame->ErrorPipeClient.PushMessage(Message);
    }
    inline void PushLog(Common::String<TCHAR>&& Message){
        MainFrame->ErrorPipeClient.PushMessage(std::move(Message));
    }
    inline void PushLog(const TCHAR* Message, ...){
        va_list VA;
        va_start(VA, Message);
        auto Str = Common::FormatWithVaList<TCHAR>(Message, VA);
        va_end(VA);
        MainFrame->ErrorPipeClient.PushMessage(std::move(Str));
    }
    inline void PushLog(const Common::StringView<TCHAR>& Message, ...){
        va_list VA;
        va_start(VA, Message);
        auto Str = Common::FormatWithVaList<TCHAR>(Message, VA);
        va_end(VA);
        MainFrame->ErrorPipeClient.PushMessage(std::move(Str));
    }

    inline void PushError(Error::ErrorCode Code){
        MainFrame->ErrorPipeClient.PushMessage(Error::GetErrorMessage(Code));
    }
    inline void PushError(Error::ErrorCode Code, ...){
        va_list VA;
        va_start(VA, Code);
        auto Str = Error::GetErrorMessageWithVaList(Code, VA);
        va_end(VA);
        MainFrame->ErrorPipeClient.PushMessage(std::move(Str));
    }
};


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


#endif _FRAMES_H_


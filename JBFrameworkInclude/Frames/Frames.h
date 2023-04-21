#ifndef _FRAMES_H_
#define _FRAMES_H_


#include <tchar.h>
#include <Windows.h>

#include <Error/Error.h>
#include <../JBFramework/Error/ErrorPipe.h>
#include <../JBFramework/GraphicsAPI/GraphicsAPI.h>


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


namespace JBF{
    template<typename... ARGS>
    extern void PushLog(const Common::String<TCHAR>& Message, ARGS&&... Args);
    template<typename... ARGS>
    extern void PushLog(Common::String<TCHAR>&& Message, ARGS&&... Args);
    template<typename... ARGS>
    extern void PushLog(const Common::StringView<TCHAR>& Message, ARGS&&... Args);
    template<typename... ARGS>
    extern void PushLog(const TCHAR* Message, ARGS&&... Args);

    template<typename... ARGS>
    extern void PushError(Error::ErrorCode Code, ARGS&&... Args);
    template<typename... ARGS>
    extern void PushWarning(Error::WarningCode Code, ARGS&&... Args);

    
    namespace Frame{
        namespace __hidden{
            class FrameBase{
            public:
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

            inline void GetWindowSize(unsigned* Width, unsigned* Height)const{
                RECT RC;
                GetWindowRect(WindowHandle, &RC);
                (*Width) = static_cast<unsigned>(RC.right - RC.left);
                (*Height) = static_cast<unsigned>(RC.bottom - RC.top);
            }


        public:
            bool Init();
            bool Run();


        private:
            bool InitInternal();
            bool DestroyInternal();
            bool UpdateInternal(float TimeDelta);


        private:
            static LRESULT CALLBACK MessageProcessor(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

            
        private:
            ErrorPipe::Client<TCHAR> ErrorPipeClient;
            GraphicsAPI GraphicsModule;

        private:
            HWND WindowHandle;
            bool bIsActive;


        public:
            template<typename... ARGS>
            friend void JBF::PushLog(const Common::String<TCHAR>& Message, ARGS&&... Args);
            template<typename... ARGS>
            friend void JBF::PushLog(Common::String<TCHAR>&& Message, ARGS&&... Args);
            template<typename... ARGS>
            friend void JBF::PushLog(const Common::StringView<TCHAR>& Message, ARGS&&... Args);
            template<typename... ARGS>
            friend void JBF::PushLog(const TCHAR* Message, ARGS&&... Args);

            template<typename... ARGS>
            friend void JBF::PushError(Error::ErrorCode Code, ARGS&&... Args);
            template<typename... ARGS>
            friend void JBF::PushWarning(Error::WarningCode Code, ARGS&&... Args);
        };
    };


    extern Frame::WindowFrame* MainFrame;


    template<typename... ARGS>
    inline void PushLog(const Common::String<TCHAR>& Message, ARGS&&... Args){
        if(sizeof...(ARGS) > 0)
            MainFrame->ErrorPipeClient.PushMessage(Common::Format<TCHAR>(Message, std::forward<ARGS>(Args)...));
        else
            MainFrame->ErrorPipeClient.PushMessage(Message);
    }
    template<typename... ARGS>
    inline void PushLog(Common::String<TCHAR>&& Message, ARGS&&... Args){
        if(sizeof...(ARGS) > 0)
            MainFrame->ErrorPipeClient.PushMessage(Common::Format<TCHAR>(Message, std::forward<ARGS>(Args)...));
        else
            MainFrame->ErrorPipeClient.PushMessage(std::move(Message));
    }
    template<typename... ARGS>
    inline void PushLog(const Common::StringView<TCHAR>& Message, ARGS&&... Args){
        if(sizeof...(ARGS) > 0)
            MainFrame->ErrorPipeClient.PushMessage(Common::Format<TCHAR>(Message, std::forward<ARGS>(Args)...));
        else
            MainFrame->ErrorPipeClient.PushMessage(Message.data());
    }
    template<typename... ARGS>
    inline void PushLog(const TCHAR* Message, ARGS&&... Args){
        if(sizeof...(ARGS) > 0)
            MainFrame->ErrorPipeClient.PushMessage(Common::Format<TCHAR>(Message, std::forward<ARGS>(Args)...));
        else
            MainFrame->ErrorPipeClient.PushMessage(Message);
    }

    template<typename... ARGS>
    inline void PushError(Error::ErrorCode Code, ARGS&&... Args){
        MainFrame->ErrorPipeClient.PushMessage(_T("!Error! ") + Error::GetErrorMessage(Code, std::forward<ARGS>(Args)...));
    }
    template<typename... ARGS>
    inline void PushWarning(Error::WarningCode Code, ARGS&&... Args){
        MainFrame->ErrorPipeClient.PushMessage(_T("!Warning! ") + Error::GetWarningMessage(Code, std::forward<ARGS>(Args)...));
    }
};


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


#endif


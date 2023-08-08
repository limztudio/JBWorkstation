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
    extern void PushLog(const Common::String<TCHAR>& message, ARGS&&... args);
    template<typename... ARGS>
    extern void PushLog(Common::String<TCHAR>&& message, ARGS&&... args);
    template<typename... ARGS>
    extern void PushLog(const Common::StringView<TCHAR>& message, ARGS&&... args);
    template<typename... ARGS>
    extern void PushLog(const TCHAR* message, ARGS&&... args);

    template<typename... ARGS>
    extern void PushError(Error::ErrorCode code, ARGS&&... args);
    template<typename... ARGS>
    extern void PushWarning(Error::WarningCode code, ARGS&&... args);

    
    namespace Frame{
        namespace __hidden{
            class FrameBase{
            public:
                FrameBase();
                virtual ~FrameBase();


            public:
                inline bool IsLoggerValid(void** outHandle)const{
                    (*outHandle) = loggerPI.hProcess;
                    
                    if(!loggerPI.hProcess)
                        return false;

                    DWORD exitCode;
                    if(!GetExitCodeProcess(loggerPI.hProcess, &exitCode))
                        return false;

                    if(exitCode != STILL_ACTIVE)
                        return false;
                    
                    return true;
                }
                inline bool IsLoggerValid()const{
                    void* dummy = nullptr;
                    return IsLoggerValid(&dummy);
                }
                
                
            private:
                PROCESS_INFORMATION loggerPI;
            };
        };
        
        class WindowFrame : public __hidden::FrameBase{
        private:
            static constexpr size_t reservedFrameSize = 4;

            
        public:
            WindowFrame(void* instanceHandle, const TCHAR* appName, unsigned width, unsigned height);
            virtual ~WindowFrame()override;


        public:
            inline bool IsValid()const{ return windowHandle != nullptr; }
            inline bool IsActive()const{ return bIsActive; }

            inline void GetWindowSize(unsigned* width, unsigned* height)const{
                RECT rc;
                GetWindowRect(windowHandle, &rc);
                (*width) = static_cast<unsigned>(rc.right - rc.left);
                (*height) = static_cast<unsigned>(rc.bottom - rc.top);
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
            ErrorPipe::Client<TCHAR> errorPipeClient;
            GraphicsAPI graphicsModule;

        private:
            HWND windowHandle;
            bool bIsActive;


        public:
            template<typename... ARGS>
            friend void JBF::PushLog(const Common::String<TCHAR>& message, ARGS&&... args);
            template<typename... ARGS>
            friend void JBF::PushLog(Common::String<TCHAR>&& message, ARGS&&... args);
            template<typename... ARGS>
            friend void JBF::PushLog(const Common::StringView<TCHAR>& message, ARGS&&... args);
            template<typename... ARGS>
            friend void JBF::PushLog(const TCHAR* message, ARGS&&... args);

            template<typename... ARGS>
            friend void JBF::PushError(Error::ErrorCode code, ARGS&&... args);
            template<typename... ARGS>
            friend void JBF::PushWarning(Error::WarningCode code, ARGS&&... args);
        };
    };


    extern Frame::WindowFrame* mainFrame;


    template<typename... ARGS>
    inline void PushLog(const Common::String<TCHAR>& message, ARGS&&... args){
        if(sizeof...(ARGS) > 0)
            mainFrame->errorPipeClient.PushMessage(Common::Format<TCHAR>(message, std::forward<ARGS>(args)...));
        else
            mainFrame->errorPipeClient.PushMessage(message);
    }
    template<typename... ARGS>
    inline void PushLog(Common::String<TCHAR>&& message, ARGS&&... args){
        if(sizeof...(ARGS) > 0)
            mainFrame->errorPipeClient.PushMessage(Common::Format<TCHAR>(message, std::forward<ARGS>(args)...));
        else
            mainFrame->errorPipeClient.PushMessage(std::move(message));
    }
    template<typename... ARGS>
    inline void PushLog(const Common::StringView<TCHAR>& message, ARGS&&... args){
        if(sizeof...(ARGS) > 0)
            mainFrame->errorPipeClient.PushMessage(Common::Format<TCHAR>(message, std::forward<ARGS>(args)...));
        else
            mainFrame->errorPipeClient.PushMessage(message.data());
    }
    template<typename... ARGS>
    inline void PushLog(const TCHAR* message, ARGS&&... args){
        if(sizeof...(ARGS) > 0)
            mainFrame->errorPipeClient.PushMessage(Common::Format<TCHAR>(message, std::forward<ARGS>(args)...));
        else
            mainFrame->errorPipeClient.PushMessage(message);
    }

    template<typename... ARGS>
    inline void PushError(Error::ErrorCode code, ARGS&&... args){
        mainFrame->errorPipeClient.PushMessage(_T("!Error! ") + Error::GetErrorMessage(code, std::forward<ARGS>(args)...));
    }
    template<typename... ARGS>
    inline void PushWarning(Error::WarningCode code, ARGS&&... args){
        mainFrame->errorPipeClient.PushMessage(_T("!Warning! ") + Error::GetWarningMessage(code, std::forward<ARGS>(args)...));
    }
};


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


#endif


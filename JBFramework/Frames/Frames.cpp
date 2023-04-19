#include <Frames/Frames.h>

#include <Common/Path.h>


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


namespace JBF{
    namespace Frame{
        namespace __hidden{
            FrameBase::FrameBase()
                : LoggerPI({})
            {
                SetCurrentDirectory(Common::GetModuleDirectory().String().c_str());

                TCHAR CmdLine[] = _T("JBLogger.exe");
                STARTUPINFO SI = {};
                SI.cb = sizeof(STARTUPINFO);
                if(!CreateProcess(nullptr, CmdLine, nullptr, nullptr, TRUE, 0, nullptr, nullptr, &SI, &LoggerPI)){
                    Error::ShowFatalMessage(Error::FatalCode::LOGGER_CANNOT_EXECUTE);
                    assert(false);
                    return;
                }
            }
            FrameBase::~FrameBase(){
                if(LoggerPI.hProcess){
                    CloseHandle(LoggerPI.hProcess);
                    CloseHandle(LoggerPI.hThread);
                }
            }
        };
    };
};


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


namespace JBF{
    namespace Frame{
        WindowFrame::WindowFrame(void* InstanceHandle, const TCHAR* AppName, unsigned Width, unsigned Height)
            : FrameBase()
            , ErrorPipeClient(GetCurrentProcessId())
        {
            void* LoggerHandle = nullptr;
            if(!IsLoggerValid(&LoggerHandle)){
                if(LoggerHandle){
                    Error::ShowFatalMessage(Error::FatalCode::LOGGER_NOT_VALID_AFTER_EXECUTED, LoggerHandle);
                    assert(false);
                }
                return;
            }
            
            if(!ErrorPipeClient.IsValid()){
                Error::ShowFatalMessage(Error::FatalCode::ERRORPIPE_CLIENT_CREATE_FAILED);
                assert(false);
                return;
            }

            WNDCLASSEX WC = {};
            {
                WC.cbSize = sizeof(WNDCLASSEX);
                WC.style = CS_HREDRAW | CS_VREDRAW;
                WC.lpfnWndProc = MessageProcessor;
                WC.hInstance = reinterpret_cast<decltype(WC.hInstance)>(InstanceHandle);
                WC.hCursor = LoadCursor(nullptr, IDC_ARROW);
                WC.hbrBackground = reinterpret_cast<HBRUSH>(COLOR_WINDOW);
                WC.lpszClassName = AppName;
            }
            if(!RegisterClassEx(&WC)){
                ErrorPipeClient.PushMessage(Error::GetErrorMessage(Error::ErrorCode::FRAME_WINDOW_REGISTERCLASS_FAILED));
                assert(false);
                return;
            }
        }


        LRESULT CALLBACK WindowFrame::MessageProcessor(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam){
            PAINTSTRUCT PS;
    
            switch(message){
            case WM_DESTROY:
                PostQuitMessage(0);
                return 0;

            case WM_PAINT:
                if(HDC hDC = BeginPaint(hWnd, &PS)){
                    
                    EndPaint(hWnd, &PS);
                }
                return 0;

            default:
                break;
            }
    
            return DefWindowProc(hWnd, message, wParam, lParam);
        }
    };
};


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


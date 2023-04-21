#include <Frames/Frames.h>

#include <chrono>

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
    Frame::WindowFrame* MainFrame = nullptr;

    
    namespace Frame{
        WindowFrame::WindowFrame(void* InstanceHandle, const TCHAR* AppName, unsigned Width, unsigned Height)
            : FrameBase()
            , ErrorPipeClient(GetCurrentProcessId(), 10000)
            , WindowHandle(nullptr)
            , bIsActive(false)
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

            static constexpr DWORD StyleEx = 0;
            static constexpr DWORD Style = WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX | WS_MAXIMIZEBOX;

            RECT RC = {0, 0, static_cast<long>(Width), static_cast<long>(Height)};

            WindowHandle = CreateWindowEx(
                StyleEx,
                WC.lpszClassName,
                AppName,
                Style,
                0,
                0,
                RC.right - RC.left,
                RC.bottom - RC.top,
                nullptr,
                nullptr,
                WC.hInstance,
                nullptr
            );
            if(!WindowHandle){
                ErrorPipeClient.PushMessage(Error::GetErrorMessage(Error::ErrorCode::FRAME_WINDOW_CREATE_FAILED));
                assert(false);
                return;
            }

            {
                if(!AdjustWindowRectEx(&RC, Style, FALSE, StyleEx)){
                    ErrorPipeClient.PushMessage(Error::GetErrorMessage(Error::ErrorCode::FRAME_WINDOW_ADJUST_FAILED));
                    WindowHandle = nullptr;
                    assert(false);
                    return;
                }

                const long ActualWidth = RC.right - RC.left;
                const long ActualHeight = RC.bottom - RC.top;
                const long X = (GetSystemMetrics(SM_CXSCREEN) - ActualWidth) >> 1;
                const long Y = (GetSystemMetrics(SM_CYSCREEN) - ActualHeight) >> 1;
    
                if(!MoveWindow(WindowHandle, X, Y, ActualWidth, ActualHeight, false)){
                    ErrorPipeClient.PushMessage(Error::GetErrorMessage(Error::ErrorCode::FRAME_WINDOW_MOVE_FAILED, WindowHandle));
                    WindowHandle = nullptr;
                    assert(false);
                    return;
                }
            }

            MainFrame = this;
        }
        WindowFrame::~WindowFrame(){
            if(!DestroyInternal()){
                ErrorPipeClient.PushMessage(Error::GetErrorMessage(Error::ErrorCode::FRAME_DESTROY_FAILED));
                assert(false);
            }
            MainFrame = nullptr;
        }


        bool WindowFrame::Init(){
            if(!InitInternal()){
                ErrorPipeClient.PushMessage(Error::GetErrorMessage(Error::ErrorCode::FRAME_INIT_FAILED));
                assert(false);
                return false;
            }
            
            ShowWindow(WindowHandle, SW_SHOW);
            return true;
        }
        bool WindowFrame::Run(){
            MSG Message = {};

            std::chrono::steady_clock::time_point LateTime(std::chrono::steady_clock::now());
            
            for(;;){
                for(;;){
                    if(bIsActive){
                        if(!PeekMessage(&Message, nullptr, 0, 0, PM_REMOVE))
                            break;
                    }
                    else
                        GetMessage(&Message, nullptr, 0, 0);

                    if(Message.message == WM_QUIT)
                        return true;

                    TranslateMessage(&Message);
                    DispatchMessage(&Message);
                }

                if(!IsLoggerValid())
                    return true;

                std::chrono::steady_clock::time_point CurrentTime(std::chrono::steady_clock::now());
                std::chrono::duration<float, std::chrono::seconds::period> TimeDifference(CurrentTime - LateTime);
                LateTime = CurrentTime;
                
                if(!UpdateInternal(TimeDifference.count())){
                    ErrorPipeClient.PushMessage(Error::GetErrorMessage(Error::ErrorCode::FRAME_UPDATE_FAILED));
                    assert(false);
                    break;
                }
            }
            return false;
        }


        bool WindowFrame::InitInternal(){
            if(!GraphicsModule.Init(WindowHandle, false))
                return false;
            
            return true;
        }
        bool WindowFrame::DestroyInternal(){
            return true;
        }
        bool WindowFrame::UpdateInternal(float TimeDelta){
            return true;
        }


        LRESULT CALLBACK WindowFrame::MessageProcessor(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam){
            if(WindowFrame* This = MainFrame){
                PAINTSTRUCT PS;
                
                switch(message){
                case WM_DESTROY:
                    PostQuitMessage(0);
                    return 0;

                case WM_ACTIVATE:
                    switch(LOWORD(wParam)){
                    case WA_INACTIVE:
                        This->bIsActive = false;
                        break;
                    default:
                        This->bIsActive = true;
                        break;
                    }
                    return 0;

                case WM_PAINT:
                    if(HDC hDC = BeginPaint(hWnd, &PS)){
                    
                        EndPaint(hWnd, &PS);
                    }
                    return 0;

                default:
                    break;
                }
            }
            return DefWindowProc(hWnd, message, wParam, lParam);
        }
    };
};


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


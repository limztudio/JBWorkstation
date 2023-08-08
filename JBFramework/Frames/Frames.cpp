#include <Frames/Frames.h>

#include <chrono>

#include <Common/Path.h>


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


namespace JBF{
    namespace Frame{
        namespace __hidden{
            FrameBase::FrameBase()
                : loggerPI({})
            {
                SetCurrentDirectory(Common::GetModuleDirectory().String().c_str());

                TCHAR cmdLine[] = _T("JBLogger.exe");
                STARTUPINFO si = {};
                si.cb = sizeof(STARTUPINFO);
                if(!CreateProcess(nullptr, cmdLine, nullptr, nullptr, TRUE, 0, nullptr, nullptr, &si, &loggerPI)){
                    Error::ShowFatalMessage(Error::FatalCode::LOGGER_CANNOT_EXECUTE);
                    assert(false);
                    return;
                }
            }
            FrameBase::~FrameBase(){
                if(loggerPI.hProcess){
                    CloseHandle(loggerPI.hProcess);
                    CloseHandle(loggerPI.hThread);
                }
            }
        };
    };
};


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


namespace JBF{
    Frame::WindowFrame* mainFrame = nullptr;

    
    namespace Frame{
        WindowFrame::WindowFrame(void* instanceHandle, const TCHAR* appName, unsigned width, unsigned height)
            : FrameBase()
            , errorPipeClient(GetCurrentProcessId(), 10000)
            , windowHandle(nullptr)
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
            
            if(!errorPipeClient.IsValid()){
                Error::ShowFatalMessage(Error::FatalCode::ERRORPIPE_CLIENT_CREATE_FAILED);
                assert(false);
                return;
            }

            WNDCLASSEX WC = {};
            {
                WC.cbSize = sizeof(WNDCLASSEX);
                WC.style = CS_HREDRAW | CS_VREDRAW;
                WC.lpfnWndProc = MessageProcessor;
                WC.hInstance = reinterpret_cast<decltype(WC.hInstance)>(instanceHandle);
                WC.hCursor = LoadCursor(nullptr, IDC_ARROW);
                WC.hbrBackground = reinterpret_cast<HBRUSH>(COLOR_WINDOW);
                WC.lpszClassName = appName;
            }
            if(!RegisterClassEx(&WC)){
                errorPipeClient.PushMessage(Error::GetErrorMessage(Error::ErrorCode::FRAME_WINDOW_REGISTERCLASS_FAILED));
                assert(false);
                return;
            }

            static constexpr DWORD StyleEx = 0;
            static constexpr DWORD Style = WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX | WS_MAXIMIZEBOX;

            RECT RC = {0, 0, static_cast<long>(width), static_cast<long>(height)};

            windowHandle = CreateWindowEx(
                StyleEx,
                WC.lpszClassName,
                appName,
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
            if(!windowHandle){
                errorPipeClient.PushMessage(Error::GetErrorMessage(Error::ErrorCode::FRAME_WINDOW_CREATE_FAILED));
                assert(false);
                return;
            }

            {
                if(!AdjustWindowRectEx(&RC, Style, FALSE, StyleEx)){
                    errorPipeClient.PushMessage(Error::GetErrorMessage(Error::ErrorCode::FRAME_WINDOW_ADJUST_FAILED));
                    windowHandle = nullptr;
                    assert(false);
                    return;
                }

                const long ActualWidth = RC.right - RC.left;
                const long ActualHeight = RC.bottom - RC.top;
                const long X = (GetSystemMetrics(SM_CXSCREEN) - ActualWidth) >> 1;
                const long Y = (GetSystemMetrics(SM_CYSCREEN) - ActualHeight) >> 1;
    
                if(!MoveWindow(windowHandle, X, Y, ActualWidth, ActualHeight, false)){
                    errorPipeClient.PushMessage(Error::GetErrorMessage(Error::ErrorCode::FRAME_WINDOW_MOVE_FAILED, windowHandle));
                    windowHandle = nullptr;
                    assert(false);
                    return;
                }
            }

            mainFrame = this;
        }
        WindowFrame::~WindowFrame(){
            if(!DestroyInternal()){
                errorPipeClient.PushMessage(Error::GetErrorMessage(Error::ErrorCode::FRAME_DESTROY_FAILED));
                assert(false);
            }
            mainFrame = nullptr;
        }


        bool WindowFrame::Init(){
            if(!InitInternal()){
                errorPipeClient.PushMessage(Error::GetErrorMessage(Error::ErrorCode::FRAME_INIT_FAILED));
                assert(false);
                return false;
            }
            
            ShowWindow(windowHandle, SW_SHOW);
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
                    errorPipeClient.PushMessage(Error::GetErrorMessage(Error::ErrorCode::FRAME_UPDATE_FAILED));
                    assert(false);
                    break;
                }
            }
            return false;
        }


        bool WindowFrame::InitInternal(){
            if(!graphicsModule.Init(windowHandle, false))
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
            if(WindowFrame* This = mainFrame){
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


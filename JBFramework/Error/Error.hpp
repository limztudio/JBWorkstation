#include <tchar.h>
#include <Windows.h>

#include <Common/String.h>


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


namespace JBF{
    namespace Error{
        enum class FatalCode : unsigned long;
        enum class ErrorCode : unsigned long;

        
        template<typename... ARGS>
        Common::String<TCHAR> GetFatalMessage(FatalCode Code, ARGS&&... Args){
            switch(Code){
            case FatalCode::SUCCEEDED:
                return _T("Succeeded.");

            case FatalCode::ERRORPIPE_SERVER_CREATE_FAILED:
                return _T("ErrorPipe server creation failed.");
            case FatalCode::ERRORPIPE_CLIENT_CREATE_FAILED:
                return _T("ErrorPipe client creation failed.");

            case FatalCode::ERRORPIPE_SERVER_CONNECT_FAILED:
                return _T("ErrorPipe server connecting failed.");

            case FatalCode::ERRORPIPE_CLIENT_WRITE_FAILED:
                return _T("ErrorPipe client writing message failed.");
                
            case FatalCode::ERRORPIPE_SERVER_READ_MISMATCH:
                return Common::Format(_T("ErrorPipe server reading message failed. Expected size: %ull, Actual size: %ull."), std::forward<ARGS>(Args)...);
            case FatalCode::ERRORPIPE_CLIENT_WRITE_MISMATCH:
                return Common::Format(_T("ErrorPipe client writing message failed. Expected size: %ull, Actual size: %ull."), std::forward<ARGS>(Args)...);

            case FatalCode::LOGGER_CANNOT_EXECUTE:
                return _T("Logger cannot be executed.");
            case FatalCode::LOGGER_NOT_VALID_AFTER_EXECUTED:
                return Common::Format(_T("Logger is not valid after executed. Handle: %x."), std::forward<ARGS>(Args)...);
                
            case FatalCode::LOGGER_NO_PARENT:
                return _T("Logger has no parent.");
            case FatalCode::LOGGER_CANNOT_LOOKUP_PARENT:
                return Common::Format(_T("Logger couldn't look up parent process %x."), std::forward<ARGS>(Args)...);

            case FatalCode::LOGGER_NO_MODULE_DIRECTORY:
                return _T("Logger couldn't find current module directory.");
            case FatalCode::LOGGER_CANNOT_OPEN_LOG_FILE:
                return Common::Format(_T("Logger couldn't open log file \"%s\"."), std::forward<ARGS>(Args)...);

            case FatalCode::LOGGER_WRITE_MISMATCH:
                return Common::Format(_T("Logger couldn't write to log file. Expected size: %ull, Actual size: %ull."), std::forward<ARGS>(Args)...);

            default:
                return _T("Unknown fatal error.");
            }
        }
        template<typename... ARGS>
        Common::String<TCHAR> GetErrorMessage(ErrorCode Code, ARGS&&... Args){
            switch(Code){
            case ErrorCode::SUCCEEDED:
                return _T("Succeeded.");

            case ErrorCode::FRAME_WINDOW_REGISTERCLASS_FAILED:
                return _T("Frame window registration failed.");
            case ErrorCode::FRAME_WINDOW_CREATE_FAILED:
                return _T("Frame window creation failed.");
            case ErrorCode::FRAME_WINDOW_ADJUST_FAILED:
                return _T("Frame window adjustment failed.");
            case ErrorCode::FRAME_WINDOW_MOVE_FAILED:
                return Common::Format(_T("Frame window moving failed. HWND: %x"), std::forward<ARGS>(Args)...);

            case ErrorCode::FRAME_INIT_FAILED:
                return _T("Frame initialization failed.");
            case ErrorCode::FRAME_DESTROY_FAILED:
                return _T("Frame destruction failed.");
            case ErrorCode::FRAME_UPDATE_FAILED:
                return _T("Frame update failed.");

            default:
                return _T("Unknown error.");
            }
        }

        template<typename... ARGS>
        void ShowFatalMessage(FatalCode Code, ARGS&&... Args){
            MessageBox(nullptr, GetFatalMessage(Code, std::forward<ARGS>(Args)...).c_str(), _T("Fatal Error"), MB_OK | MB_ICONERROR);
        }
    };
};


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


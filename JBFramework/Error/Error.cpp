#include <Error/Error.h>

#include <Windows.h>


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


namespace JBF{
    namespace Error{
        Common::String<TCHAR> GetFatalMessageWithVaList(FatalCode Code, va_list VA){
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
                return Common::FormatWithVaList(_T("ErrorPipe server reading message failed. Expected size: %ull, Actual size: %ull."), VA);
            case FatalCode::ERRORPIPE_CLIENT_WRITE_MISMATCH:
                return Common::FormatWithVaList(_T("ErrorPipe client writing message failed. Expected size: %ull, Actual size: %ull."), VA);

            case FatalCode::LOGGER_CANNOT_EXECUTE:
                return _T("Logger cannot be executed.");
            case FatalCode::LOGGER_NOT_VALID_AFTER_EXECUTED:
                return Common::FormatWithVaList(_T("Logger is not valid after executed. Handle: %x."), VA);
                
            case FatalCode::LOGGER_NO_PARENT:
                return _T("Logger has no parent.");
            case FatalCode::LOGGER_CANNOT_LOOKUP_PARENT:
                return Common::FormatWithVaList(_T("Logger couldn't look up parent process %x."), VA);

            case FatalCode::LOGGER_NO_MODULE_DIRECTORY:
                return _T("Logger couldn't find current module directory.");
            case FatalCode::LOGGER_CANNOT_OPEN_LOG_FILE:
                return Common::FormatWithVaList(_T("Logger couldn't open log file \"%s\"."), VA);

            case FatalCode::LOGGER_WRITE_MISMATCH:
                return Common::FormatWithVaList(_T("Logger couldn't write to log file. Expected size: %ull, Actual size: %ull."), VA);

            default:
                return _T("Unknown fatal error.");
            }
        }
        Common::String<TCHAR> GetErrorMessageWithVaList(ErrorCode Code, va_list VA){
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
                return Common::FormatWithVaList(_T("Frame window moving failed. HWND: %x"), VA);

            default:
                return _T("Unknown error.");
            }
        }
    
        Common::String<TCHAR> GetFatalMessage(FatalCode Code, ...){
            va_list VA;
            va_start(VA, Code);
            auto Result = GetFatalMessageWithVaList(Code, VA);
            va_end(VA);
            return std::move(Result);
        }
        Common::String<TCHAR> GetErrorMessage(ErrorCode Code, ...){
            va_list VA;
            va_start(VA, Code);
            auto Result = GetErrorMessageWithVaList(Code, VA);
            va_end(VA);
            return std::move(Result);
        }

        void ShowFatalMessage(FatalCode Code, ...){
            va_list VA;
            va_start(VA, Code);
            auto Result = GetFatalMessageWithVaList(Code, VA);
            va_end(VA);

            MessageBox(nullptr, Result.c_str(), _T("Fatal Error"), MB_OK | MB_ICONERROR);
        }
    };
};


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


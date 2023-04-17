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

            case FatalCode::ERRORPIPE_CLIENT_WRITE_FAILED:
                return _T("ErrorPipe client writing message failed.");
                
            case FatalCode::ERRORPIPE_SERVER_READ_MISMATCH:
                return Common::FormatWithVaList(_T("ErrorPipe server reading message failed. Expected size: %ull, Actual size: %ull."), VA);
            case FatalCode::ERRORPIPE_CLIENT_WRITE_MISMATCH:
                return Common::FormatWithVaList(_T("ErrorPipe client writing message failed. Expected size: %ull, Actual size: %ull."), VA);

            case FatalCode::LOGGER_NO_PARENT:
                return _T("Logger has no parent.");
            case FatalCode::LOGGER_CANNOT_LOOKUP_PARENT:
                return Common::FormatWithVaList(_T("Logger couldn't look up parent process %x."), VA);

            case FatalCode::LOGGER_NO_MODULE_DIRECTORY:
                return _T("Logger couldn't find current module directory.");
            case FatalCode::LOGGER_CANNOT_OPEN_LOG_FILE:
                return Common::FormatWithVaList(_T("Logger couldn't open log file \"%s\"."), VA);

            default:
                return _T("Unknown fatal error.");
            }
        }
        Common::String<TCHAR> GetErrorMessageWithVaList(ErrorCode Code, va_list VA){
            switch(Code){
            case ErrorCode::SUCCEEDED:
                return _T("Succeeded.");

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


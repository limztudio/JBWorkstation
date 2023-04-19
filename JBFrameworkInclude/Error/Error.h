#ifndef _ERROR_H_
#define _ERROR_H_


#include <tchar.h>

#include <Common/String.h>


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


namespace JBF{
    namespace Error{
        enum class FatalCode : unsigned long{
            SUCCEEDED,

            ERRORPIPE_SERVER_CREATE_FAILED,
            ERRORPIPE_CLIENT_CREATE_FAILED,

            ERRORPIPE_SERVER_CONNECT_FAILED,

            ERRORPIPE_CLIENT_WRITE_FAILED,
            
            ERRORPIPE_SERVER_READ_MISMATCH,
            ERRORPIPE_CLIENT_WRITE_MISMATCH,

            LOGGER_CANNOT_EXECUTE,
            LOGGER_NOT_VALID_AFTER_EXECUTED,
            
            LOGGER_NO_PARENT,
            LOGGER_CANNOT_LOOKUP_PARENT,
            
            LOGGER_NO_MODULE_DIRECTORY,
            LOGGER_CANNOT_OPEN_LOG_FILE,

            LOGGER_WRITE_MISMATCH,
        };
        enum class ErrorCode : unsigned long{
            SUCCEEDED,

            FRAME_WINDOW_REGISTERCLASS_FAILED,
            FRAME_WINDOW_CREATE_FAILED,
            FRAME_WINDOW_ADJUST_FAILED,
            FRAME_WINDOW_MOVE_FAILED,

            FRAME_INIT_FAILED,
            FRAME_DESTROY_FAILED,
            FRAME_UPDATE_FAILED,
        };

        extern Common::String<TCHAR> GetFatalMessageWithVaList(FatalCode Code, va_list VA);
        extern Common::String<TCHAR> GetErrorMessageWithVaList(ErrorCode Code, va_list VA);

        extern Common::String<TCHAR> GetFatalMessage(FatalCode Code, ...);
        extern Common::String<TCHAR> GetErrorMessage(ErrorCode Code, ...);

        void ShowFatalMessage(FatalCode Code, ...);
    };
};


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


#endif _ERROR_H_


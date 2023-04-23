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

            GAPI_FACTORY_CREATE_FAILED,
            GAPI_WARP_ADAPTER_ENUM_FAILED,
            GAPI_DEVICE_CREATE_FAILED,
            
            GAPI_SHADER_MODEL_UNSUPPORTED,
            GAPI_MESH_SHADER_UNSUPPORTED,
            
            GAPI_COMMAND_QUEUE_CREATE_FAILED,
            GAPI_SWAP_CHAIN_CREATE_FAILED,
            GAPI_SWAP_CHAIN_CONVERT_FAILED,
            
            GAPI_RTV_HEAP_CREATE_FAILED,
            GAPI_DSV_HEAP_CREATE_FAILED,
            GAPI_CBVSRVUAV_HEAP_CREATE_FAILED,

            GAPI_EARN_RT_BUFFER_FAILED,
            GAPI_SCENE_COMMAND_ALLOCATOR_CREATE_FAILED,
            GAPI_CREATE_DS_BUFFER_FAILED,
        };
        enum class WarningCode : unsigned long{
            SUCCEEDED,

            GAPI_EARN_DEBUG_INTERFACE_FAILED,
        };
    };
};


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


#include <../JBFramework/Error/Error.hpp>


#endif


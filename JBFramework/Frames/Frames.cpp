#include <Frames/Frames.h>


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


namespace JBF{
    namespace Frame{
        WindowFrame::WindowFrame()
            : ErrorPipeClient(GetCurrentProcessId())
        {
            if(!ErrorPipeClient.IsValid()){
                Error::ShowFatalMessage(Error::FatalCode::ERRORPIPE_CLIENT_CREATE_FAILED);
                assert(false);
                return;
            }
        }
    };
};


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


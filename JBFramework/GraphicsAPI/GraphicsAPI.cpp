#include "GraphicsAPI.h"


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


namespace JBF{
    GraphicsAPI::GraphicsAPI()
        : FrameIndex(0)
        , RVTDescSize(0)
        , DSVDescSize(0)
        , CSBufferView(nullptr)
        , bUseVsync(false)
    {}

    
    bool GraphicsAPI::Init(void* WindowHandle, bool bUseWarp){
        if(!InitPipeline(WindowHandle, bUseWarp))
            return false;
        
        return true;
    }
};


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


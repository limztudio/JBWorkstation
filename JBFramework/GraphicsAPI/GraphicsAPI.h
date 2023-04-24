#ifndef _GRAPHICSAPI_H_
#define _GRAPHICSAPI_H_


#include <tchar.h>

#include <wrl/client.h>

#include <d3d12.h>
#include <dxgi1_6.h>

#include "Common/Math.h"


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


namespace JBF{
    namespace Graphics{
        template <typename T>
        using COM = Microsoft::WRL::ComPtr<T>;


        struct alignas(512) ConstantBuffer{
            Common::Vector34 World;
            Common::Vector34 WorldView;
            Common::Vector44 WorldViewProj;
            unsigned long Meshlets;
        };
    };
};


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


namespace JBF{
    class GraphicsAPI{
    public:
        static constexpr UINT FrameCount = 2;


    public:
        GraphicsAPI();

        
    public:
        bool Init(void* WindowHandle, bool bUseWarp);

        
    private:
        bool InitPipeline(void* WindowHandle, bool bUseWarp);

        
    private:
        Graphics::COM<IDXGISwapChain3> SwapChain;
        Graphics::COM<ID3D12Device> Device;
        Graphics::COM<ID3D12CommandQueue> CommandQueue;
        Graphics::COM<ID3D12CommandAllocator> SceneCommandAllocators[FrameCount];
        Graphics::COM<ID3D12DescriptorHeap> RTVHeap;
        Graphics::COM<ID3D12DescriptorHeap> DSVHeap;

    private:
        Graphics::COM<ID3D12Resource> RTBuffers[FrameCount];
        Graphics::COM<ID3D12Resource> DSBuffer;

    private:
        Graphics::COM<ID3D12Resource> CSBuffer;

    private:
        UINT FrameIndex;
        UINT RVTDescSize;
        UINT DSVDescSize;

    private:
        bool bUseVsync;
    };
};


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


#endif


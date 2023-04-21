#ifndef _GRAPHICSAPI_H_
#define _GRAPHICSAPI_H_


#include <tchar.h>

#include <wrl/client.h>

#include <d3d12.h>
#include <dxgi1_6.h>


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


namespace JBF{
    class GraphicsAPI{
    public:
        template <typename T>
        using COM = Microsoft::WRL::ComPtr<T>;


    public:
        static constexpr UINT FrameCount = 2;


    public:
        GraphicsAPI();

        
    public:
        bool Init(void* WindowHandle, bool bUseWarp);

        
    private:
        bool InitPipeline(void* WindowHandle, bool bUseWarp);

        
    private:
        COM<IDXGISwapChain3> SwapChain;
        COM<ID3D12Device> Device;
        COM<ID3D12CommandQueue> CommandQueue;

        COM<ID3D12DescriptorHeap> RTVHeap;
        COM<ID3D12DescriptorHeap> DSVHeap;
        COM<ID3D12DescriptorHeap> CBVSRVUAVHeap;

    private:
        COM<ID3D12CommandAllocator> SceneCommandAllocators[FrameCount];

    private:
        UINT FrameIndex;
        UINT RVTDescSize;
        UINT CBVSRVUAVDescSize;

    private:
        bool bUseVsync;
    };
};


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


#endif

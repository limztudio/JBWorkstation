#include "GraphicsAPI.h"
#include "GraphicsAPIMacro.hpp"

#include "Frames/Frames.h"


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


namespace JBF{
    namespace __hidden_GraphicsAPI{
        void EnumHardwareAdapter(IDXGIFactory1* FactoryPtr, IDXGIAdapter1** AdapterPtr, bool bPreferHighPerformanceAdapter = true){
            (*AdapterPtr) = nullptr;

            GraphicsAPI::COM<IDXGIAdapter1> Adapter;
            auto LoopScope = [&Adapter](UINT i){
                DXGI_ADAPTER_DESC1 DESC;
                Adapter->GetDesc1(&DESC);

                if(DESC.Flags & DXGI_ADAPTER_FLAG_SOFTWARE)
                    return true;

                if(SUCCEEDED(D3D12CreateDevice(Adapter.Get(), D3D_FEATURE_LEVEL_11_0, _uuidof(ID3D12Device), nullptr)))
                    return false;

                return true;
            };

            GraphicsAPI::COM<IDXGIFactory6> Factory;
            if(SUCCEEDED(FactoryPtr->QueryInterface(IID_PPV_ARGS(&Factory)))){
                if(bPreferHighPerformanceAdapter){
                    for(UINT i = 0; SUCCEEDED(Factory->EnumAdapterByGpuPreference(i, DXGI_GPU_PREFERENCE_HIGH_PERFORMANCE, IID_PPV_ARGS(&Adapter))); ++i){
                        if(!LoopScope(i))
                            break;
                    }
                }
                else{
                    for(UINT i = 0; SUCCEEDED(Factory->EnumAdapterByGpuPreference(i, DXGI_GPU_PREFERENCE_UNSPECIFIED, IID_PPV_ARGS(&Adapter))); ++i){
                        if(!LoopScope(i))
                            break;
                    }
                }
            }

            if(!Adapter.Get()){
                for(UINT i = 0; SUCCEEDED(FactoryPtr->EnumAdapters1(i, &Adapter)); ++i){
                    if(!LoopScope(i))
                        break;
                }
            }

            (*AdapterPtr) = Adapter.Detach();
        }
    };
};


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


namespace JBF{
    extern Frame::WindowFrame* MainFrame;

    
    bool GraphicsAPI::InitPipeline(void* WindowHandle, bool bUseWarp){
        UINT DXGIFactoryFlags = 0;

#ifdef _DEBUG
        {
            COM<ID3D12Debug> DebugController;
            if(SUCCEEDED(D3D12GetDebugInterface(IID_PPV_ARGS(&DebugController)))){
                DebugController->EnableDebugLayer();
                
                DXGIFactoryFlags |= DXGI_CREATE_FACTORY_DEBUG;
            }
            else
                PushWarning(Error::WarningCode::GAPI_EARN_DEBUG_INTERFACE_FAILED);
        }
#endif

        COM<IDXGIFactory4> Factory;
        if(FAILED(CreateDXGIFactory2(DXGIFactoryFlags, IID_PPV_ARGS(&Factory)))){
            PushError(Error::ErrorCode::GAPI_FACTORY_CREATE_FAILED);
            assert(false);
            return false;
        }

        if(bUseWarp){
            COM<IDXGIAdapter> Adapter;
            if(FAILED(Factory->EnumWarpAdapter(IID_PPV_ARGS(&Adapter)))){
                PushError(Error::ErrorCode::GAPI_WARP_ADAPTER_ENUM_FAILED);
                assert(false);
                return false;
            }

            if(FAILED(D3D12CreateDevice(Adapter.Get(), D3D_FEATURE_LEVEL_11_0, IID_PPV_ARGS(&Device)))){
                PushError(Error::ErrorCode::GAPI_DEVICE_CREATE_FAILED);
                assert(false);
                return false;
            }
        }
        else{
            COM<IDXGIAdapter1> Adapter;
            __hidden_GraphicsAPI::EnumHardwareAdapter(Factory.Get(), &Adapter);

            if(FAILED(D3D12CreateDevice(Adapter.Get(), D3D_FEATURE_LEVEL_11_0, IID_PPV_ARGS(&Device)))){
                PushError(Error::ErrorCode::GAPI_DEVICE_CREATE_FAILED);
                assert(false);
                return false;
            }
        }

        {
            static constexpr D3D_SHADER_MODEL RequireShaderModel = D3D_SHADER_MODEL_6_5;
            
            D3D12_FEATURE_DATA_SHADER_MODEL ShaderModel = { RequireShaderModel };
            if(FAILED(Device->CheckFeatureSupport(D3D12_FEATURE_SHADER_MODEL, &ShaderModel, sizeof(ShaderModel))) || (ShaderModel.HighestShaderModel < RequireShaderModel)){
                PushError(Error::ErrorCode::GAPI_SHADER_MODEL_UNSUPPORTED, RequireShaderModel, ShaderModel.HighestShaderModel);
                assert(false);
                return false;
            }
        }

        {
            D3D12_FEATURE_DATA_D3D12_OPTIONS7 Features = {};
            if(FAILED(Device->CheckFeatureSupport(D3D12_FEATURE_D3D12_OPTIONS7, &Features, sizeof(Features))) || (Features.MeshShaderTier == D3D12_MESH_SHADER_TIER_NOT_SUPPORTED)){
                PushError(Error::ErrorCode::GAPI_MESH_SHADER_UNSUPPORTED);
                assert(false);
                return false;
            }
        }

        {
            D3D12_COMMAND_QUEUE_DESC DESC = {};
            DESC.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;
            DESC.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;

            if(FAILED(Device->CreateCommandQueue(&DESC, IID_PPV_ARGS(&CommandQueue)))){
                PushError(Error::ErrorCode::GAPI_COMMAND_QUEUE_CREATE_FAILED);
                assert(false);
                return false;
            }
            OBJECT_SET_NAME(CommandQueue);
        }

        {
            unsigned Width, Height;
            MainFrame->GetWindowSize(&Width, &Height);

            HWND WinHandle = reinterpret_cast<HWND>(WindowHandle);
            
            DXGI_SWAP_CHAIN_DESC1 DESC = {};
            DESC.BufferCount = FrameCount;
            DESC.Width = static_cast<decltype(DESC.Width)>(Width);
            DESC.Height = static_cast<decltype(DESC.Height)>(Height);
            DESC.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
            DESC.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
            DESC.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
            DESC.SampleDesc.Count = 1;
            DESC.Flags = bUseVsync ? 0 : DXGI_SWAP_CHAIN_FLAG_ALLOW_TEARING;

            COM<IDXGISwapChain1> LocalSwapChain;
            if(FAILED(Factory->CreateSwapChainForHwnd(CommandQueue.Get(), WinHandle, &DESC, nullptr, nullptr, &LocalSwapChain))){
                PushError(Error::ErrorCode::GAPI_SWAP_CHAIN_CREATE_FAILED);
                assert(false);
                return false;
            }

            if(!bUseVsync)
                Factory->MakeWindowAssociation(WinHandle, DXGI_MWA_NO_ALT_ENTER);

            if(FAILED(LocalSwapChain.As(&SwapChain))){
                PushError(Error::ErrorCode::GAPI_SWAP_CHAIN_CONVERT_FAILED);
                assert(false);
                return false;
            }

            FrameIndex = SwapChain->GetCurrentBackBufferIndex();
        }

        {
            D3D12_DESCRIPTOR_HEAP_DESC DESC = {};
            DESC.NumDescriptors = FrameCount + 1;
            DESC.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
            DESC.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
            
            if(FAILED(Device->CreateDescriptorHeap(&DESC, IID_PPV_ARGS(&RTVHeap)))){
                PushError(Error::ErrorCode::GAPI_RTV_HEAP_CREATE_FAILED);
                assert(false);
                return false;
            }
        }

        {
            D3D12_DESCRIPTOR_HEAP_DESC DESC = {};
            DESC.NumDescriptors = 1;
            DESC.Type = D3D12_DESCRIPTOR_HEAP_TYPE_DSV;
            DESC.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
            
            if(FAILED(Device->CreateDescriptorHeap(&DESC, IID_PPV_ARGS(&DSVHeap)))){
                PushError(Error::ErrorCode::GAPI_DSV_HEAP_CREATE_FAILED);
                assert(false);
                return false;
            }
        }

        {
            D3D12_DESCRIPTOR_HEAP_DESC DESC = {};
            DESC.NumDescriptors = FrameCount * 3;
            DESC.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
            DESC.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
            
            if(FAILED(Device->CreateDescriptorHeap(&DESC, IID_PPV_ARGS(&CBVSRVUAVHeap)))){
                PushError(Error::ErrorCode::GAPI_CBVSRVUAV_HEAP_CREATE_FAILED);
                assert(false);
                return false;
            }
            OBJECT_SET_NAME(CBVSRVUAVHeap);
        }

        RVTDescSize = Device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
        CBVSRVUAVDescSize = Device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);

        for(UINT i = 0; i < FrameCount; ++i){
            if(FAILED(Device->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(&SceneCommandAllocators[i])))){
                PushError(Error::ErrorCode::GAPI_SCENE_COMMAND_ALLOCATOR_CREATE_FAILED, i);
                assert(false);
                return false;
            }
        }
        
        return true;
    }
};


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


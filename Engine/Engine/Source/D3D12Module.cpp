#include "Globals.h"
#include "Windows.h"
#include "D3D12Module.h"

bool D3D12Module::init()
{
#if defined(_DEBUG)
	ComPtr<ID3D12Debug> debugInterface;

	HRESULT hr = D3D12GetDebugInterface(IID_PPV_ARGS(&debugInterface));

	if (FAILED(hr))
		return false;

	debugInterface->EnableDebugLayer();
	//LOG("D3D12 Debug Layer enabled");
#endif
#if defined(_DEBUG)
	CreateDXGIFactory2(DXGI_CREATE_FACTORY_DEBUG,IID_PPV_ARGS(&factory));
#else
	CreateDXGIFactory2(0,IID_PPV_ARGS(&factory));
#endif
	DXGI_SWAP_CHAIN_DESC1 swapChainDesc = {};

	RECT clientRect;
	GetClientRect(hwnd, &clientRect);

	UINT windowWidth = clientRect.right - clientRect.left;
	UINT windowHeight = clientRect.bottom - clientRect.top;
	
	swapChainDesc.Width = windowWidth; // Set the width of the swap chain to the width of the window
	swapChainDesc.Height = windowHeight; // Set the height of the swap chain to the height of the window

	swapChainDesc.Stereo = FALSE; //Para VR
	swapChainDesc.SampleDesc= {1,0}; // No multisampling {Count, Quality}
	swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT; // The swap chain will be used to render to a window
	swapChainDesc.BufferCount = bufferCount; // Double buffering

	swapChainDesc.Scaling = DXGI_SCALING_STRETCH; // How to scale when the window size doesnt match the buffer size

	swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD; // The swap chain will discard the contents of the back buffer after presenting

	swapChainDesc.AlphaMode = DXGI_ALPHA_MODE_UNSPECIFIED; // The alpha mode is unspecified
	swapChainDesc.Flags = 0; // No flags

	HRESULT hrSwapChain = factory->CreateSwapChainForHwnd(commandQueue.Get(), hwnd, &swapChainDesc, nullptr, nullptr, &swapChain);

	if (FAILED(hrSwapChain))
	{
		LOG("Failed to create swap chain. HRESULT: 0x%08X",
			static_cast<unsigned int>(hrSwapChain));
		return false;
	}

	for (UINT i = 0; i < bufferCount; ++i) 
	{
		HRESULT hrBuffer = swapChain->GetBuffer(i, IID_PPV_ARGS(&backBuffers[i]));
		if (FAILED(hrBuffer))
		{
			LOG("Failed to get back buffer %d. HRESULT: 0x%08X", i,
				static_cast<unsigned int>(hrBuffer));
			return false;
		}
	}

	D3D12_DESCRIPTOR_HEAP_DESC rtvHeapDesc = {};

	rtvHeapDesc.NumDescriptors = bufferCount; // One descriptor for each back buffer
	rtvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV; // The heap will contain render target views
	rtvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE; // The heap will not be shader visible

	HRESULT hrHeap = device->CreateDescriptorHeap(&rtvHeapDesc, IID_PPV_ARGS(&rtvHeap));
	if (FAILED(hrHeap))
	{
		LOG("Failed to create RTV descriptor heap. HRESULT: 0x%08X",
			static_cast<unsigned int>(hrHeap));
		return false;
	}

	rtvDescriptorSize = device->GetDescriptorHandleIncrementSize(
		D3D12_DESCRIPTOR_HEAP_TYPE_RTV); // Get the size (in bytes) of a descriptor in the RTV heap


	factory->EnumAdapterByGpuPreference(0,DXGI_GPU_PREFERENCE_HIGH_PERFORMANCE,IID_PPV_ARGS(&adapter));
	//LOG("Creating D3D12 device");
	HRESULT hrDevice = D3D12CreateDevice(adapter.Get(), D3D_FEATURE_LEVEL_12_0, IID_PPV_ARGS(&device));

	if (FAILED(hrDevice))
	{
		LOG("Failed to create D3D12 device. HRESULT: 0x%08X",
			static_cast<unsigned int>(hrDevice));
		return false;
	}

	D3D12_COMMAND_QUEUE_DESC queueDesc = {};

	queueDesc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;
	queueDesc.Priority = D3D12_COMMAND_QUEUE_PRIORITY_NORMAL;
	queueDesc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;

	HRESULT hrQueue = device->CreateCommandQueue(&queueDesc, IID_PPV_ARGS(&commandQueue));

	if (FAILED(hrQueue))
	{
		LOG("Failed to create D3D12 command queue. HRESULT: 0x%08X",
			static_cast<unsigned int>(hrQueue));
		return false;
	}


	ComPtr<ID3D12InfoQueue> infoQueue;
	//device.As(&infoQueue);
	HRESULT hrInfoQueue = device.As(&infoQueue);

	if (FAILED(hrInfoQueue))
	{
		LOG("Failed to get ID3D12InfoQueue. HRESULT: 0x%08X",
			static_cast<unsigned int>(hrInfoQueue));
		return false;
	}

	infoQueue->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_CORRUPTION, TRUE);
	infoQueue->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_ERROR, TRUE);
	infoQueue->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_WARNING, TRUE);

	
	return true;
}	

D3D12Module::D3D12Module(HWND hwnd): hwnd(hwnd)
{

}
//void preRender() 
//{
//
//}
//void postRender() 
//{
//
//}

bool D3D12Module::cleanUp()
{
	// Clean up Direct3D 12 resources here
	return true;
}
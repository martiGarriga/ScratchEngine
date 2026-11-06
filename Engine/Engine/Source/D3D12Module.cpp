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
	//CREATING DXGI FACTORY
	CreateDXGIFactory2(DXGI_CREATE_FACTORY_DEBUG,IID_PPV_ARGS(&factory));
#else
	CreateDXGIFactory2(0,IID_PPV_ARGS(&factory));
#endif
	//OBTAINING ADAPTER
	factory->EnumAdapterByGpuPreference(0,DXGI_GPU_PREFERENCE_HIGH_PERFORMANCE,IID_PPV_ARGS(&adapter));
	//CREATING D3D12 DEVICE
	HRESULT hrDevice = D3D12CreateDevice(adapter.Get(), D3D_FEATURE_LEVEL_12_0, IID_PPV_ARGS(&device));
	if (FAILED(hrDevice))
	{
		LOG("Failed to create D3D12 device. HRESULT: 0x%08X",
			static_cast<unsigned int>(hrDevice));
		return false;
	}

	//CREATING COMMAND QUEUE
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

	//CONFIGURING SWAP CHAIN
	DXGI_SWAP_CHAIN_DESC1 swapChainDesc = {};
	RECT clientRect;

	GetClientRect(hwnd, &clientRect);
	
	UINT windowWidth = clientRect.right - clientRect.left;
	UINT windowHeight = clientRect.bottom - clientRect.top;
	swapChainDesc.Width = windowWidth; // Set the width of the swap chain to the width of the window
	swapChainDesc.Height = windowHeight; // Set the height of the swap chain to the height of the window
	swapChainDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM; // The format of the swap chain will be 32-bit RGBA
	swapChainDesc.Stereo = FALSE; //Para VR
	swapChainDesc.SampleDesc = { 1,0 }; // No multisampling {Count, Quality}
	swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT; // The swap chain will be used to render to a window
	swapChainDesc.BufferCount = bufferCount; // Double buffering
	swapChainDesc.Scaling = DXGI_SCALING_STRETCH; // How to scale when the window size doesnt match the buffer size
	swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD; // The swap chain will discard the contents of the back buffer after presenting
	swapChainDesc.AlphaMode = DXGI_ALPHA_MODE_UNSPECIFIED; // The alpha mode is unspecified
	swapChainDesc.Flags = 0; // No flags

	//CREATING SWAP CHAIN
	ComPtr<IDXGISwapChain1> tempSwapChain;
	HRESULT hrSwapChain = factory->CreateSwapChainForHwnd(commandQueue.Get(), hwnd, &swapChainDesc, nullptr, nullptr, &tempSwapChain);
	if (FAILED(hrSwapChain))
	{
		LOG("Failed to create swap chain. HRESULT: 0x%08X",
			static_cast<unsigned int>(hrSwapChain));
		return false;
	}
	HRESULT hrSwapChain3 = tempSwapChain.As(&swapChain);
	if (FAILED(hrSwapChain3))
	{
		LOG("Failed to create swap chain. HRESULT: 0x%08X",
			static_cast<unsigned int>(hrSwapChain3));
		return false;
	}

	//OBTENING BACK BUFFERS
	for (UINT i = 0; i < bufferCount; ++i)
	{
		HRESULT hrBuffer = swapChain->GetBuffer(i, IID_PPV_ARGS(&backBuffers[i])); // Get the back buffer from the swap chain and store it in the backBuffers array
		if (FAILED(hrBuffer))
		{
			LOG("Failed to get back buffer %d. HRESULT: 0x%08X", i,
				static_cast<unsigned int>(hrBuffer));
			return false;
		}
	}

	//CONFIGURING DESCRIPTOR HEAP FOR RENDER TARGET VIEWS (RTVs)
	D3D12_DESCRIPTOR_HEAP_DESC rtvHeapDesc = {}; //Descriptor Heap
	rtvHeapDesc.NumDescriptors = bufferCount; // One descriptor for each back buffer
	rtvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV; // The heap will contain render target views
	rtvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE; // The heap will not be shader visible

	//CREATING DESCRIPTOR HEAP FOR RTVs
	HRESULT hrHeap = device->CreateDescriptorHeap(&rtvHeapDesc, IID_PPV_ARGS(&rtvHeap));
	if (FAILED(hrHeap))
	{
		LOG("Failed to create RTV descriptor heap. HRESULT: 0x%08X",
			static_cast<unsigned int>(hrHeap));
		return false;
	}

	//CREATING RTVs FOR EACH BACK BUFFER
	rtvDescriptorSize = device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV); // Get the size (in bytes) of a descriptor in the RTV heap
	for (UINT i = 0; i < bufferCount; ++i)
	{
		CD3DX12_CPU_DESCRIPTOR_HANDLE rtvCPUHandle(rtvHeap->GetCPUDescriptorHandleForHeapStart(), i, rtvDescriptorSize);

		device->CreateRenderTargetView(backBuffers[i].Get(), nullptr, rtvCPUHandle);
	}

	//CREATING COMMAND ALLOCATORS
	for (UINT i = 0; i < bufferCount; ++i)
	{
		HRESULT hrAllocator = device->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(&commandAllocators[i]));

		if (FAILED(hrAllocator))
		{
			LOG("Failed to create command allocator");
			return false;
		}
	}	

	//CREATING COMMAND LIST
	HRESULT hrCommandList = device->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT, commandAllocators[0].Get(), nullptr, IID_PPV_ARGS(&commandList));
	if (FAILED(hrCommandList))
	{
		LOG("Failed to create D3D12 command list. HRESULT: 0x%08X",
			static_cast<unsigned int>(hrCommandList));
		return false;
	}

	//CLOSING COMMAND LIST
	HRESULT hrClose = commandList->Close();

	if (FAILED(hrClose))
	{
		LOG("Failed to close command list");
		return false;
	}

	//CREATING FENCE
	HRESULT hrFence = device->CreateFence(0, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&fence));
	if (FAILED(hrFence))
	{
		LOG("Failed to create D3D12 fence. HRESULT: 0x%08X",
			static_cast<unsigned int>(hrFence));
		return false;
	}
	//CREATING FENCE EVENT
	fenceEvent = CreateEvent(nullptr, FALSE, FALSE, nullptr);	

	//CREATING INFOQUEUE FOR DEBUGGING
	ComPtr<ID3D12InfoQueue> infoQueue;
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

D3D12Module::D3D12Module(HWND hwnd): hwnd(hwnd) {}

void D3D12Module::preRender()
{
	/*if (fenceValue != 0)
	{
		HRESULT hr = fence->SetEventOnCompletion(fenceValue, fenceEvent);
		DWORD waitResult = WaitForSingleObject(fenceEvent, INFINITE);
	}

	commandAllocator->Reset();*/

	currentIndex = swapChain->GetCurrentBackBufferIndex();
	WaitForFence(_fenceValues[currentIndex]);
	commandAllocators[currentIndex]->Reset();
}

void D3D12Module::render()
{
	currentIndex = swapChain->GetCurrentBackBufferIndex();
	commandList->Reset(commandAllocators[currentIndex].Get(), nullptr);

	auto barrierToTarget = CD3DX12_RESOURCE_BARRIER::Transition(backBuffers[currentIndex].Get(), D3D12_RESOURCE_STATE_PRESENT, D3D12_RESOURCE_STATE_RENDER_TARGET);
	commandList->ResourceBarrier(1, &barrierToTarget);

	CD3DX12_CPU_DESCRIPTOR_HANDLE rtvHandle(rtvHeap->GetCPUDescriptorHandleForHeapStart(), currentIndex, rtvDescriptorSize);
	const float clearColor[] = { 1.0f, 0.0f, 0.0f, 1.0f }; // RGBA
	commandList->ClearRenderTargetView(rtvHandle, clearColor, 0, nullptr);

	
	auto barrierToPresent = CD3DX12_RESOURCE_BARRIER::Transition(backBuffers[currentIndex].Get(), D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_PRESENT);
	commandList->ResourceBarrier(1, &barrierToPresent);

	HRESULT hrClose = commandList->Close();
	if (FAILED(hrClose))
	{
		LOG("Failed to close command list");
		return;
	}

	ID3D12CommandList* commandLists[] = { commandList.Get() };
	commandQueue->ExecuteCommandLists(1, commandLists);

	HRESULT hrPresent = swapChain->Present(1, 0);
	if (FAILED(hrPresent))
	{
		LOG("Failed to present swap chain");
		LOG("HRESULT: 0x%08X", hrPresent);
		return;
	}

}

void D3D12Module::postRender()
{
	/*++fenceValue;
	HRESULT hr = commandQueue->Signal(fence.Get(), fenceValue);*/

	commandQueue->Signal(fence.Get(), ++fenceValue);
	currentIndex = swapChain->GetCurrentBackBufferIndex();
	_fenceValues[currentIndex] = fenceValue;
}

void D3D12Module::WaitForFence(UINT64 fenceValues)
{
	if (fence->GetCompletedValue() < fenceValues)
	{
		HRESULT hr = fence->SetEventOnCompletion(fenceValues, fenceEvent);
		DWORD waitResult = WaitForSingleObject(fenceEvent, INFINITE);
	}
}

bool D3D12Module::cleanUp()
{
	// Clean up Direct3D 12 resources here
	if (fenceEvent)
	{
		if (fenceValue != 0)
		{
			HRESULT hr = fence->SetEventOnCompletion(fenceValue, fenceEvent);
			DWORD waitResult = WaitForSingleObject(fenceEvent, INFINITE);
		}

		CloseHandle(fenceEvent);
		fenceEvent = nullptr;
	}
	return true;
}

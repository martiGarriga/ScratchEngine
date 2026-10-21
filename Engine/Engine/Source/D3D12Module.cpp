#include "Globals.h"
#include "D3D12Module.h"

bool D3D12Module::init()
{
#if defined(_DEBUG){
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
	factory->EnumAdapterByGpuPreference(0,DXGI_GPU_PREFERENCE_HIGH_PERFORMANCE,IID_PPV_ARGS(&adapter));
	//LOG("Creating D3D12 device");
	D3D12CreateDevice(adapter.Get(),D3D_FEATURE_LEVEL_12_0,IID_PPV_ARGS(&device));

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

bool D3D12Module::cleanUp()
{
	// Clean up Direct3D 12 resources here
	return true;
}
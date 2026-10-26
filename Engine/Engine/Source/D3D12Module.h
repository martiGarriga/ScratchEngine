#pragma once
#include "Module.h"

class D3D12Module : public Module 
{
public:
	bool init() override;
	D3D12Module(HWND hwnd);
	void preRender() override;
	void postRender() override;
	bool cleanUp() override;

private:
	ComPtr<IDXGIFactory6> factory;
	ComPtr<IDXGIAdapter4> adapter;
	ComPtr<ID3D12Device5> device;
	HWND hwnd = nullptr;
	ComPtr<ID3D12CommandQueue> commandQueue;
	ComPtr<ID3D12CommandAllocator> commandAllocator;
	ComPtr<ID3D12CommandList> commandList;
	ComPtr<IDXGISwapChain1> swapChain;
	static constexpr UINT bufferCount = 2;
	ComPtr<ID3D12Resource> backBuffers[bufferCount];
	ComPtr<ID3D12DescriptorHeap> rtvHeap;
	UINT rtvDescriptorSize = 0; // Size of the RTV descriptor
	ComPtr<ID3D12Fence> fence;
	UINT fenceValue = 0;
	HANDLE fenceEvent = nullptr;
};

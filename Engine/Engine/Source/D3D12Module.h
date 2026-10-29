#pragma once
#include "Module.h"

class D3D12Module : public Module 
{
public:
	bool init() override;
	D3D12Module(HWND hwnd);
	void preRender() override;
	void render() override;
	void postRender() override;
	void WaitForFence(UINT64 fenceValues);
	bool cleanUp() override;
	void renderExercise();

private:
	ComPtr<IDXGIFactory6> factory;
	ComPtr<IDXGIAdapter4> adapter;
	ComPtr<ID3D12Device5> device;
	HWND hwnd = nullptr;
	static constexpr UINT bufferCount = 2;
	ComPtr<ID3D12CommandQueue> commandQueue;
	ComPtr<ID3D12CommandAllocator> commandAllocators[bufferCount];
	ComPtr<ID3D12GraphicsCommandList> commandList;
	ComPtr<IDXGISwapChain3> swapChain;
	ComPtr<ID3D12Resource> backBuffers[bufferCount];
	ComPtr<ID3D12DescriptorHeap> rtvHeap;
	UINT rtvDescriptorSize = 0; // Size of the RTV descriptor
	ComPtr<ID3D12Fence> fence;
	UINT64 fenceValue = 0;
	UINT64 _fenceValues[bufferCount] = { 0 }; // Array to hold fence values for each buffer
	HANDLE fenceEvent = nullptr;
	UINT currentIndex = 0; // Current back buffer index
};

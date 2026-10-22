#pragma once
#include "Module.h"

class D3D12Module : public Module 
{
public:
	bool init() override;
	bool cleanUp() override;
	void preRender(); 
	void postRender();

private:
	ComPtr<IDXGIFactory6> factory;
	ComPtr<IDXGIAdapter4> adapter;
	ComPtr<ID3D12Device5> device;
};

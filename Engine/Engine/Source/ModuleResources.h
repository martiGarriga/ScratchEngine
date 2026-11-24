#pragma once
#include "Globals.h"

class ModuleResources
{
public:
    void init(ID3D12Device* device,ID3D12GraphicsCommandList* commandList);
    void CreateUploadBuffer(UINT64 bufferSize,const void* cpuData,UINT64 dataSize,ComPtr<ID3D12Resource>& buffer);
    void CreateDefaultBuffer(UINT64 bufferSize,const void* cpuData,UINT64 dataSize,ComPtr<ID3D12Resource>& buffer);

private:
    ID3D12Device* device = nullptr;
    ID3D12GraphicsCommandList* commandList = nullptr;
    ComPtr<ID3D12Resource> stagingBuffer;
};
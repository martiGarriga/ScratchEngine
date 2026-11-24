#include "Globals.h"
#include "ModuleResources.h"

void ModuleResources::init(ID3D12Device* device,ID3D12GraphicsCommandList* commandList)
{
    this->device = device;
    this->commandList = commandList;
}

void ModuleResources::CreateUploadBuffer(UINT64 bufferSize,const void* cpuData,UINT64 dataSize,ComPtr<ID3D12Resource>& buffer)
{
    D3D12_RESOURCE_DESC desc = CD3DX12_RESOURCE_DESC::Buffer(bufferSize);
    CD3DX12_HEAP_PROPERTIES heapProps(D3D12_HEAP_TYPE_UPLOAD);

    device->CreateCommittedResource(
        &heapProps,
        D3D12_HEAP_FLAG_NONE,
        &desc,
        D3D12_RESOURCE_STATE_GENERIC_READ,
        nullptr,
        IID_PPV_ARGS(&buffer)
    );

    BYTE* pData = nullptr;
    CD3DX12_RANGE readRange(0, 0);

    buffer->Map(0, &readRange, reinterpret_cast<void**>(&pData));
    memcpy(pData, cpuData, dataSize);
    buffer->Unmap(0, nullptr);
}

void ModuleResources::CreateDefaultBuffer(UINT64 bufferSize,const void* cpuData,UINT64 dataSize,ComPtr<ID3D12Resource>& buffer)
{
    auto defaultHeap = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT);
    auto bufferDesc = CD3DX12_RESOURCE_DESC::Buffer(bufferSize);
    device->CreateCommittedResource(
        &defaultHeap,D3D12_HEAP_FLAG_NONE,&bufferDesc,D3D12_RESOURCE_STATE_COPY_DEST,nullptr,IID_PPV_ARGS(&buffer));

    auto uploadHeap = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD);
    device->CreateCommittedResource(
        &uploadHeap,D3D12_HEAP_FLAG_NONE,&bufferDesc,D3D12_RESOURCE_STATE_GENERIC_READ,nullptr,IID_PPV_ARGS(&stagingBuffer));

    BYTE* pData = nullptr;
    CD3DX12_RANGE readRange(0, 0);

    stagingBuffer->Map(0, &readRange, reinterpret_cast<void**>(&pData));
    memcpy(pData, cpuData, dataSize);
    stagingBuffer->Unmap(0, nullptr);

    commandList->CopyResource(buffer.Get(), stagingBuffer.Get());

}
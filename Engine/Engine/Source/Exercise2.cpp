#include "Globals.h"
#include "Exercise2.h"

Exercise2::Exercise2(HWND hwnd) : D3D12Module(hwnd)
{
}

bool Exercise2::init()
{
    if (!D3D12Module::init())
        return false;

    Vertex vertices[3] =
    {
        { -1.0f, -1.0f, 0.0f },
        {  0.0f,  1.0f, 0.0f },
        {  1.0f, -1.0f, 0.0f }
    };

    beginResourceCommands();

    getResources().CreateDefaultBuffer(
        sizeof(vertices),
        vertices,
        sizeof(vertices),
        vertexBuffer
    );

    executeResourceCommands();

    vertexBufferView.BufferLocation = vertexBuffer->GetGPUVirtualAddress();

    vertexBufferView.SizeInBytes = sizeof(vertices);

    vertexBufferView.StrideInBytes = sizeof(Vertex);

    CD3DX12_ROOT_SIGNATURE_DESC rootSignatureDesc;
    rootSignatureDesc.Init(
        0,
        nullptr,
        0,
        nullptr,
        D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT
    );

    ComPtr<ID3DBlob> signature;
    ComPtr<ID3DBlob> error;

    HRESULT hr = D3D12SerializeRootSignature(
        &rootSignatureDesc,
        D3D_ROOT_SIGNATURE_VERSION_1,
        &signature,
        &error
    );

    if (FAILED(hr))
    {
        if (error)
        {
            OutputDebugStringA(
                (char*)error->GetBufferPointer()
            );
        }

        return false;
    }

    hr = getDevice()->CreateRootSignature(
        0,
        signature->GetBufferPointer(),
        signature->GetBufferSize(),
        IID_PPV_ARGS(&rootSignature)
    );

    if (FAILED(hr))
    {
        OutputDebugStringA("Failed to create Root Signature\n");
        return false;
    }

    return true;
}

void Exercise2::renderExercise()
{

}
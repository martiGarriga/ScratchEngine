#include "Globals.h"
#include "Exercise2.h"
#include "ReadData.h"

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

    hr = getDevice()->CreateRootSignature(0,signature->GetBufferPointer(),signature->GetBufferSize(),IID_PPV_ARGS(&rootSignature));
    if (FAILED(hr))
    {
        OutputDebugStringA("Failed to create Root Signature\n");
        return false;
    }

    D3D12_GRAPHICS_PIPELINE_STATE_DESC psoDesc = {};
    psoDesc.pRootSignature = rootSignature.Get();

    auto dataVS = DX::ReadData(L"Exercise2VS.cso");
    auto dataPS = DX::ReadData(L"Exercise2PS.cso");

    psoDesc.VS = { dataVS.data(), dataVS.size() };
    psoDesc.PS = { dataPS.data(), dataPS.size() };

    D3D12_INPUT_ELEMENT_DESC layout[] =
    {
        {
            "MY_POSITION",0,DXGI_FORMAT_R32G32B32_FLOAT,0,0,D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA,0
        }
    };
    psoDesc.InputLayout =
    {
        layout,sizeof(layout) / sizeof(D3D12_INPUT_ELEMENT_DESC)
    };

    psoDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
    psoDesc.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM;
    psoDesc.NumRenderTargets = 1;
    psoDesc.SampleDesc = { 1, 0 };
    psoDesc.SampleMask = 0xffffffff;
    psoDesc.RasterizerState = CD3DX12_RASTERIZER_DESC(D3D12_DEFAULT);
    psoDesc.BlendState = CD3DX12_BLEND_DESC(D3D12_DEFAULT);

    bool GPSCreated = SUCCEEDED(getDevice()->CreateGraphicsPipelineState(&psoDesc,IID_PPV_ARGS(&pipelineState)));
    if (!GPSCreated)
    {
        OutputDebugStringA("Failed to create Graphics Pipeline State\n");
        return false;
    }

    return true;
}

void Exercise2::renderExercise()
{
    ID3D12GraphicsCommandList* commandList = getCommandList();

    commandList->SetPipelineState(pipelineState.Get());
    commandList->SetGraphicsRootSignature(rootSignature.Get());
    commandList->IASetVertexBuffers(0, 1, &vertexBufferView);

    UINT windowWidth = 0;
    UINT windowHeight = 0;

    getWindowSize(windowWidth, windowHeight);

    D3D12_VIEWPORT viewport{0.0f,0.0f,float(windowWidth),float(windowHeight),0.0f,1.0f};
    commandList->RSSetViewports(1, &viewport);

    D3D12_RECT scissor{0,0,(LONG)windowWidth,(LONG)windowHeight};
    commandList->RSSetScissorRects(1, &scissor);

    commandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
    commandList->DrawInstanced(3, 1, 0, 0);
}
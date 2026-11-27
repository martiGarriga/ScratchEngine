#pragma once

#include "D3D12Module.h"

class Exercise2 : public D3D12Module
{
public:
    Exercise2(HWND hwnd);

    bool init() override;
    void renderExercise() override;

private:
    struct Vertex
    {
        float x, y, z;
    };

    ComPtr<ID3D12Resource> vertexBuffer;
    D3D12_VERTEX_BUFFER_VIEW vertexBufferView;
    ComPtr<ID3D12RootSignature> rootSignature;
    ComPtr<ID3D12PipelineState> pipelineState;
};
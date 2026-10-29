#pragma once
#include "Module.h"

class D3D12Module;

class Exercise1 : public Module
{
public:
    Exercise1(D3D12Module* d3d12Module);

    virtual bool init() override;
    virtual void render() override;

private:
    D3D12Module* d3d12Module = nullptr;
};

#pragma once

#include "D3D12Module.h"

class Exercise2 : public D3D12Module
{
public:
    Exercise2(HWND hwnd);

    bool init() override;
    void renderExercise() override;
};
#include "Globals.h"
#include "Exercise2.h"

Exercise2::Exercise2(HWND hwnd) : D3D12Module(hwnd)
{
}

bool Exercise2::init()
{
    if (!D3D12Module::init())
        return false;

    struct Vertex
    {
        float x, y, z;
    };

    Vertex vertices[3] =
    {
        {-1.0f, -1.0f, 0.0f}, // 0 - Bottom left
        { 0.0f,  1.0f, 0.0f}, // 1 - Top middle
        { 1.0f, -1.0f, 0.0f}  // 2 - Bottom right
    };

    return true;
}

void Exercise2::renderExercise()
{

}
#pragma once

#include "../math/NiVec2.h" // NiVec2

struct NiUI_Transform2d
{
    NiVec2 position;
    NiVec2 size;
};

struct NiUI_Transform2dEx : public NiUI_Transform2d
{
    NiVec2 parentPos;
    NiVec2 parentSize;
};

struct NiUI_InputState
{
    bool isHover;
    bool isTrigger;
    bool isRelease;
};
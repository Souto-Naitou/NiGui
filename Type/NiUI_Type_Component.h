#pragma once

// コンポーネントデータ
// コンポーネントのデータを保持する構造体です。
// この構造体はUIクラスの内部で使用されます。


#include <string> // string
#include "../math/NiVec2.h" // NiVec2
#include "../math/NiVec4.h" // NiVec4
#include "NiUI_Enum.h" // enums


// フレックスコンテナのデータ
struct FlexContainerData
{
    NiVec2 leftTop;
    NiVec2 size;
    NiVec2 padding;
    NiVec2 margin;
    NiUI_Flex_Direction direction;
    NiUI_Flex_JustifyContent justifyContent;
    NiUI_Flex_AlignItems alignItems;
    NiUI_Flex_AlignContent alignContent;
};


struct BaseDrawData
{
    std::string id;
    std::string textureName;
    NiVec4 color;
    NiVec2 leftTop;
    NiVec2 size;
    uint32_t zOrder;
};

// ボタンデータ
struct ButtonData : public BaseDrawData
{
};

struct BaseRegionData : public BaseDrawData
{
    BaseRegionData* parent = nullptr;
};

struct DivData : public BaseRegionData 
{
};

struct DragItemAreaData : public BaseRegionData
{
};

struct DragItemData : public BaseRegionData
{
};
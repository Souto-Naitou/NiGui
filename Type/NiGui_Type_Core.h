#pragma once

#include <string> // std::string
#include <unordered_map> // std::unordered_map

#include "../Math/NiVec2.h" // NiVec2
#include "../Math/NiVec4.h" // NiVec4
#include "NiGui_Type_Component.h"

using NiID = unsigned int;

template <typename T>
using StringMap = std::unordered_map<std::string, T>;

// Included in NiGuiCoreState
struct NiGuiComponentID
{
    std::string typeHover;
    std::string typeActive;
    std::string active;
    std::string hover;
    std::string preHover;
    std::string preActive;
};

// Included in NiGuiCoreState
struct NiGuiTime
{
    uint32_t hover;
    uint32_t active;
};

// Included in NiGuiCoreState
struct NiGuiValidation
{
    bool isInitialized;
    bool isBeginFrame;
    uint32_t nestCount;
};

// Included in NiGuiCoreState
struct NiGuiFlags
{
    bool autoPosition;
};

struct NiGuiBuffer
{
    BaseRegionData* currentRegion;
    uint32_t currentZOrder;
    NiVec2 currentPos;
    BaseDrawData* currentDrawData;
    StringMap<std::string> areaToItem;
};

// Included in NiGuiIO
struct NiGuiInputData
{
    NiVec2 cursorPos;
    NiVec2 triggeredPos;
    NiVec2 differencePos;
    bool isLeft;
    bool isLeftPre;
    bool isRight;
    bool isRightPre;
    bool isMiddle;
    bool isMiddlePre;
};

// Included in NiGuiIO
struct NiGuiAudioHandle
{
    int32_t buttonHover = -1;
    int32_t buttonConfirm = -1;
};

// Included in NiGuiIO
struct NiGuiAudioHandler
{
    void* buttonHover;
    void* buttonConfirm;
};

// Included in NiGuiStyle
struct NiGuiColor
{
    NiVec4 backGround;
};

struct NiGuiStyle
{
    NiGuiColor color;

    NiVec2 windowPadding;
    NiVec2 divPadding;
    NiVec2 itemSpacing;
};


struct NiGuiIO
{
    NiGuiInputData input;
    NiGuiAudioHandle audioHnd;
    NiGuiAudioHandler audioHandler;
};


struct NiGuiCoreState
{
    NiGuiValidation valid;
    NiGuiComponentID componentID;
    NiGuiFlags flags;
    NiGuiTime time;
    NiGuiBuffer buffer;
};

struct NiGuiSetting
{
    int deleteElementThreshold;
};
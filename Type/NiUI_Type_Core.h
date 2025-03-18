#pragma once

#include <string> // std::string
#include <unordered_map> // std::unordered_map

#include "../Math/NiVec2.h" // NiVec2
#include "../Math/NiVec4.h" // NiVec4
#include "NiUI_Type_Component.h"

using NiID = unsigned int;

template <typename T>
using StringMap = std::unordered_map<std::string, T>;

// Included in NiUICoreState
struct NiUIComponentID
{
    std::string typeHover;
    std::string typeActive;
    std::string active;
    std::string hover;
    std::string preHover;
    std::string preActive;
};

// Included in NiUICoreState
struct NiUITime
{
    uint32_t hover;
    uint32_t active;
};

// Included in NiUICoreState
struct NiUIValidation
{
    bool isInitialized;
    bool isBeginFrame;
    uint32_t nestCount;
};

// Included in NiUICoreState
struct NiUIFlags
{
    bool autoPosition;
};

struct NiUIBuffer
{
    BaseRegionData* currentRegion;
    uint32_t currentZOrder;
    NiVec2 currentPos;
    BaseDrawData* currentDrawData;
    StringMap<std::string> areaToItem;
};

// Included in NiUIIO
struct NiUIInputData
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

// Included in NiUIIO
struct NiUIAudioHandle
{
    int32_t buttonHover = -1;
    int32_t buttonConfirm = -1;
};

// Included in NiUIIO
struct NiUIAudioHandler
{
    void* buttonHover;
    void* buttonConfirm;
};

// Included in NiUIStyle
struct NiUIColor
{
    NiVec4 backGround;
};

struct NiUIStyle
{
    NiUIColor color;

    NiVec2 windowPadding;
    NiVec2 divPadding;
    NiVec2 itemSpacing;
};


struct NiUIIO
{
    NiUIInputData input;
    NiUIAudioHandle audioHnd;
    NiUIAudioHandler audioHandler;
};


struct NiUICoreState
{
    NiUIValidation valid;
    NiUIComponentID componentID;
    NiUIFlags flags;
    NiUITime time;
    NiUIBuffer buffer;
};

struct NiUISetting
{
    int deleteElementThreshold;
};
#pragma once

#include "../Type/NiUI_Type_Component.h"
#include <vector>
#include <string>

class IDrawer
{
public:
    IDrawer() = default;
    virtual ~IDrawer() = default;

    void EnqueueDrawInfo(BaseDrawData* _data) 
    { 
        drawData_.push_back(_data);
    }

    void DrawSetting();
    void PostDraw();

    virtual void Draw() = 0;
    virtual void PlayAudio(uint32_t _handle) = 0;
    virtual void PlayAudio(void* _audioHandler) = 0;

protected:
    std::vector<BaseDrawData*> drawData_;
    std::vector<BaseDrawData*> drawDataZOrdered_;

private:
    void ApplyButtonHoverAndClickEffects();

};
#pragma once

#include "../Type/NiUI_Type_Core.h"

class INiUIDebug
{
public:
    INiUIDebug() = default;
    virtual void DrawDebugUI() = 0;
    void SetIO(const NiUIIO* _io) { io_ = _io; }
    void SetState(const NiUICoreState* _state) { state_ = _state; }
    void SetSetting(NiUISetting* _setting) { setting_ = _setting; }

protected:
    const NiUIIO* io_ = nullptr;
    const NiUICoreState* state_ = nullptr;
    NiUISetting* setting_ = nullptr;
};
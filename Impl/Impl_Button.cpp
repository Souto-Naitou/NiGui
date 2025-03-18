#include "../NiGui.h"

NiGui_ButtonState NiGui::Button(
    const std::string& _id,
    const std::string& _textureName,
    const NiVec4& _color,
    const NiVec2& _position,
    const NiVec2& _size,
    NiGui_StandardPoint _anchor,
    NiGui_StandardPoint _pivot)
{
    auto& buttonImage = buttonImages_[_id];
    NiGui_InputState istate = {};
    bool isHeld = false;

    NiVec2 posInRegion = _position;
    NiVec2 size = size_;
    if (state_.buffer.currentRegion != nullptr)
    {
        posInRegion = _position + state_.buffer.currentRegion->leftTop;
        size = state_.buffer.currentRegion->size;
    }

    auto leftTop = ComputeLeftTop(posInRegion, _size, size, _anchor, _pivot);

    // 当たり判定
    JudgeClickRect(leftTop, _size, istate);

    // ボタンの挙動
    bool onButton = ButtonBehavior(_id, istate, isHeld);

    /// ボタンのデータを更新
    buttonImage.id = _id;
    buttonImage.textureName = _textureName;
    buttonImage.color = _color;
    buttonImage.leftTop = leftTop;
    buttonImage.size = _size;
    buttonImage.zOrder = state_.buffer.currentZOrder++;

    NiGui_ButtonState result = NiGui_ButtonState::None;
    if (onButton)
    {
        result = NiGui_ButtonState::Confirm;
    }
    else if (istate.isHover)
    {
        result = NiGui_ButtonState::Hover;
    }

    return result;
}

bool NiGui::ButtonBehavior(const std::string& _id, const NiGui_InputState& _inputState, bool& _out_held)
{
    SetComponentId(_inputState, _id, "Button");


    if(state_.componentID.active == _id)
    {
        _out_held = true;

        if (_inputState.isRelease && _inputState.isHover)
        {
            return true;
        }
    }

    return false;
}

void NiGui::ButtonDataEnqueue()
{
    for(auto& buttonImage : buttonImages_)
    {
        // 描画クラスにデータを追加
        drawer_->EnqueueDrawInfo(&buttonImage.second);
    }
}
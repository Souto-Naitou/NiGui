#include "../NiUI.h"

#include <stdexcept> // runtime_error


bool NiUI::BeginDiv(const std::string& _id, const std::string& _textureName, const NiVec4& _color, const NiVec2& _position, const NiVec2& _size, const NiUI_StandardPoint _anchor, const NiUI_StandardPoint _pivot)
{
    NiUI_Transform2dEx transformEx = {};
    transformEx.position = _position;
    transformEx.size = _size;

    ComputeRect(transformEx, _anchor, _pivot);

    /// =============
    /// 当たり判定と挙動

    NiUI_InputState input = {};

    /// 当たり判定
    JudgeClickRect(transformEx.position, transformEx.size, input);

    /// 挙動
    DivBehavior(_id, input.isHover, input.isTrigger);

    /// データの更新
    auto& divData = divData_[_id];
    divData.id = _id;
    divData.textureName = _textureName;
    divData.color = _color;
    divData.leftTop = transformEx.position;
    divData.size = transformEx.size;
    divData.zOrder = state_.buffer.currentZOrder++;
    divData.parent = state_.buffer.currentRegion;

    /// 現在のリージョンを更新
    state_.buffer.currentRegion = dynamic_cast<BaseRegionData*>(&divData);


    return true;
}

bool NiUI::BeginDivMovable(const std::string& _id, const std::string& _textureName, const NiVec4& _color, const NiVec2& _position, const NiVec2& _size, const NiUI_StandardPoint _anchor, const NiUI_StandardPoint _pivot)
{
    NiUI_Transform2dEx transformEx = {};
    transformEx.position = _position;
    transformEx.size = _size;

    ComputeRect(transformEx, _anchor, _pivot);

    NiVec2 originLeftTop = transformEx.position;

    /// =============
    /// 当たり判定と挙動
    
    NiUI_InputState istate = {};

    transformEx.position += divOffset_[_id];
    ClampRect(transformEx.position, _size, transformEx.parentPos, transformEx.parentSize);

    /// 当たり判定
    JudgeClickRect(transformEx.position, transformEx.size, istate);

    /// 挙動
    auto& regionDiff = divOffset_[_id];

    DivBehavior(_id, istate.isHover, istate.isTrigger);

    OffsetUpdate(_id, originLeftTop, transformEx, regionDiff);

    /// データの更新
    auto& divData = divData_[_id];
    divData.id = _id;
    divData.textureName = _textureName;
    divData.color = _color;
    divData.leftTop = transformEx.position;
    divData.size = transformEx.size;
    divData.zOrder = state_.buffer.currentZOrder++;
    divData.parent = state_.buffer.currentRegion;

    /// 現在のリージョンをこのDivに更新
    state_.buffer.currentRegion = dynamic_cast<BaseRegionData*>(&divData);

    return true;
}

void NiUI::EndDiv()
{
    if (state_.valid.nestCount == 0)
    {
        throw std::runtime_error("ネスト終了関数の呼び出し過多です。");
    }

    state_.valid.nestCount--;

    state_.buffer.currentRegion = state_.buffer.currentRegion->parent;
}

void NiUI::DivBehavior(const std::string& _id, bool _isHover, bool _isTrigger)
{
    // ネストカウントを増やす
    state_.valid.nestCount++;

    SetComponentId({ .isHover = _isHover, .isTrigger = _isTrigger }, _id, "Div");
}

void NiUI::DivDataEnqueue()
{
    for (auto& divData : divData_)
    {
        // 描画クラスにデータを追加
        drawer_->EnqueueDrawInfo(&divData.second);
    }
}

void NiUI::ComputeRect(NiUI_Transform2dEx& _transform, const NiUI_StandardPoint _anchor, const NiUI_StandardPoint _pivot)
{
    /// 親の座標とサイズを取得
    if (state_.buffer.currentRegion == nullptr)
    {
        _transform.parentSize = size_;
    }
    else
    {
        _transform.parentPos = state_.buffer.currentRegion->leftTop;
        _transform.parentSize = state_.buffer.currentRegion->size;
    }

    /// 親の座標を考慮した座標を計算
    if (state_.buffer.currentRegion != nullptr)
    {
        _transform.position += state_.buffer.currentRegion->leftTop;
    }

    /// ピボットとアンカーを考慮した座標を計算
    _transform.position = ComputeLeftTop(_transform.position, _transform.size, _transform.parentSize, _anchor, _pivot);
}

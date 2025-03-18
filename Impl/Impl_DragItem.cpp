#include "../NiGui.h"

std::string NiGui::DragItemArea(const std::string & _id, const std::string & _textureName, const NiVec4 & _color, const NiVec2 & _position, const NiVec2 & _size, const NiGui_StandardPoint _anchor, const NiGui_StandardPoint _pivot)
{
    NiGui_Transform2dEx transformEx = {};
    transformEx.position = _position;
    transformEx.size = _size;

    ComputeRect(transformEx, _anchor, _pivot);

    /// =============
    /// 当たり判定と挙動
    
    NiGui_InputState istate = {};

    /// 当たり判定
    JudgeClickRect(transformEx.position, transformEx.size, istate);

    /// 挙動
    std::string result = DragItemAreaBehavior(_id, istate.isHover, istate.isTrigger, transformEx, _position);

    auto& areaData = dragItemAreaData_[_id];
    areaData.id = _id;
    areaData.textureName = _textureName;
    areaData.color = _color;
    areaData.leftTop = transformEx.position;
    areaData.size = _size;
    areaData.zOrder = state_.buffer.currentZOrder++;

    return result;
}

std::string NiGui::DragItemAreaBehavior(const std::string& _id, bool _isHover, bool _isTrigger, NiGui_Transform2dEx& _transform, const NiVec2& _leftTop)
{
    SetComponentId({ .isHover = _isHover, .isTrigger = _isTrigger }, _id, "DragItemArea");

    auto item = state_.buffer.areaToItem.find(_id);
    if (item != state_.buffer.areaToItem.end())
    {
        return item->second;
    }
    else
    {
        return {};
    }
    
}

std::string NiGui::DragItemBehavior(const std::string& _id, const NiGui_InputState& _inputState, NiGui_Transform2dEx& _transform, const NiVec2& _originLeftTop)
{
    std::string preHoverId = state_.componentID.hover;
    std::string preTypeId = state_.componentID.typeHover;
    std::string result = {};

    SetComponentId(_inputState, _id, "DragItem");

    /// offsetの更新
    auto& offset = dragItemOffset_[_id];
    OffsetUpdate(_id, _originLeftTop, _transform, offset);


    if (_inputState.isRelease && state_.componentID.active == _id) 
    {
        for (auto& areaToItem : state_.buffer.areaToItem)
        {
            if (areaToItem.second == _id)
            {
                areaToItem.second = {};
            }
        }
        if (preTypeId == "DragItemArea" && (state_.buffer.areaToItem[preHoverId].empty() || state_.buffer.areaToItem[preHoverId] == _id))
        {
            const auto& area = dragItemAreaData_[preHoverId];
            NiVec2 diff = area.size - _transform.size;
            _transform.position = area.leftTop + diff * 0.5f;
            offset = _transform.position - _originLeftTop;
            state_.buffer.areaToItem[preHoverId] = _id;
        }
        else if (preTypeId != "DragItemArea")
        {
            _transform.position = _originLeftTop;
            offset = {};
        }
        else
        {
            _transform.position = _originLeftTop;
            offset = {};
        }
    }

    for (auto& areaToItem : state_.buffer.areaToItem)
    {
        if (areaToItem.second == _id)
        {
            result = areaToItem.first;
        }
    }

    return result;
}

std::string NiGui::DragItem(const std::string& _id, const std::string& _textureName, const NiVec4& _color, const NiVec2& _position, const NiVec2& _size, const NiGui_StandardPoint _anchor, const NiGui_StandardPoint _pivot)
{
    NiGui_Transform2dEx transformEx = {};
    transformEx.position = _position;
    transformEx.size = _size;

    ComputeRect(transformEx, _anchor, _pivot);

    NiVec2 originLeftTop = transformEx.position;

    // Clamp
    transformEx.position += dragItemOffset_[_id];
    ClampRect(transformEx.position, transformEx.size, transformEx.parentPos, transformEx.parentSize);

    /// =============
    /// 当たり判定と挙動

    NiGui_InputState istate = {};

    /// 当たり判定
    JudgeClickRect(transformEx.position, transformEx.size, istate);

    /// 挙動
    std::string result = DragItemBehavior(_id, istate, transformEx, originLeftTop);

    /// データの更新
    auto& itemData = dragItemData_[_id];
    itemData.id = _id;
    itemData.textureName = _textureName;
    itemData.color = _color;
    itemData.leftTop = transformEx.position;
    itemData.size = transformEx.size;
    itemData.zOrder = state_.buffer.currentZOrder++;

    return result;
}

void NiGui::DragItemAreaDataEnqueue()
{
    for (auto& dragItemAreaData : dragItemAreaData_)
    {
        // 描画クラスにデータを追加
        drawer_->EnqueueDrawInfo(&dragItemAreaData.second);
    }
}

void NiGui::DragItemDataEnqueue()
{
    for (auto& dragItemData : dragItemData_)
    {
        // 描画クラスにデータを追加
        drawer_->EnqueueDrawInfo(&dragItemData.second);
    }
}

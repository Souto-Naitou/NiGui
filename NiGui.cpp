#include "NiGui.h"

#include <stdexcept> // runtime_error
#include <cassert>

NiGui_Input         NiGui::input_        = NiGui_Input();
NiVec2              NiGui::leftTop_      = { 0, 0 };
NiVec2              NiGui::size_         = { 0, 0 };
INiGuiDrawer*       NiGui::drawer_       = nullptr;

NiGuiIO             NiGui::io_           = NiGuiIO();
NiGuiCoreState      NiGui::state_        = NiGuiCoreState();
INiGuiDebug*        NiGui::debug_        = nullptr;
NiGuiSetting        NiGui::setting_      = NiGuiSetting();
NiGuiStyle          NiGui::style_        = NiGuiStyle();

const NiVec4        NiGui::WHITE         = { 1.0f, 1.0f, 1.0f, 1.0f };
const NiVec4        NiGui::BLACK         = { 0.0f, 0.0f, 0.0f, 1.0f };
const NiVec4        NiGui::RED           = { 1.0f, 0.0f, 0.0f, 1.0f };
const NiVec4        NiGui::GREEN         = { 0.0f, 1.0f, 0.0f, 1.0f };
const NiVec4        NiGui::BLUE          = { 0.0f, 0.0f, 1.0f, 1.0f };
const NiVec4        NiGui::YELLOW        = { 1.0f, 1.0f, 0.0f, 1.0f };
const NiVec4        NiGui::CIAN          = { 0.0f, 1.0f, 1.0f, 1.0f };
const NiVec4        NiGui::MAGENTA       = { 1.0f, 0.0f, 1.0f, 1.0f };

std::unordered_map<std::string, ButtonData> NiGui::buttonImages_ = std::unordered_map<std::string, ButtonData>();
std::unordered_map<std::string, DivData> NiGui::divData_ = std::unordered_map<std::string, DivData>();
std::unordered_map<std::string, DragItemAreaData> NiGui::dragItemAreaData_ = std::unordered_map<std::string, DragItemAreaData>();
std::unordered_map<std::string, DragItemData> NiGui::dragItemData_ = std::unordered_map<std::string, DragItemData>();

std::unordered_map<std::string, NiVec2> NiGui::divOffset_ = std::unordered_map<std::string, NiVec2>();
std::unordered_map<std::string, NiVec2> NiGui::dragItemOffset_ = std::unordered_map<std::string, NiVec2>();



void NiGui::Initialize(const NiVec2& _size, const NiVec2& _leftTop)
{
    state_.valid.isInitialized = true;
    leftTop_ = _leftTop;
    size_ = _size;

    input_.Initialize();

    setting_.deleteElementThreshold = 35;

    style_.windowPadding = { 10, 10 };
    style_.itemSpacing = { 10, 10 };
    style_.divPadding = { 10, 10 };
    style_.color.backGround = { 0.0f, 0.0f, 0.0f, 0.7f };


    return;
}

void NiGui::BeginFrame()
{
    CheckValid_BeginFrame();

    // 入力データ更新
    input_.Update();

    // 入力データのコピー
    CopyInputData();

    // 確認用フラグを立てる
    state_.valid.isBeginFrame = true;

    return;
}

void NiGui::DrawUI()
{
    // 描画処理に必要なデータの確認
    CheckValid_DrawUI();

    // コンポーネントの確定処理
    PostProcessComponents();

    // コンポーネントの描画データを追加
    ButtonDataEnqueue();
    DivDataEnqueue();
    DragItemAreaDataEnqueue();
    DragItemDataEnqueue();

    // 描画前処理
    drawer_->DrawSetting();

    // 描画
    drawer_->Draw();

    // 前フレームのデータとして保存
    SavePreData();

    // データのクリア
    ClearData();

    // 確認用フラグを倒す
    state_.valid.isBeginFrame = false;
}

void NiGui::NiGui_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    input_.WndProcHandler(hWnd, msg, wParam, lParam);
    return;
}

void NiGui::CheckValid_BeginFrame()
{
    if(!state_.valid.isInitialized)
    {
        throw std::runtime_error("UIクラスが初期化されていません。");
    }

    if(state_.valid.isBeginFrame)
    {
        throw std::runtime_error("BeginFrameが連続で呼び出されています。");
    }
}

void NiGui::CheckValid_DrawUI()
{
    if(!state_.valid.isInitialized)
    {
        throw std::runtime_error("UIクラスが初期化されていません。");
    }

    if(!state_.valid.isBeginFrame)
    {
        throw std::runtime_error("BeginFrameが呼び出されていません。");
    }

    if(drawer_ == nullptr)
    {
        throw std::runtime_error("Drawerがセットされていません。");
    }

    if (state_.valid.nestCount != 0)
    {
        throw std::runtime_error("ネストが不正です。ネスト開始関数とネスト終了関数の数が正しく呼び出されていますか？");
    }
}

void NiGui::JudgeClickRect(const NiVec2& _leftTop, const NiVec2& _size, NiGui_InputState& _input)
{
    NiVec2 leftTop = _leftTop + leftTop_;

    bool& hover = _input.isHover;
    bool& trigger = _input.isTrigger;
    bool& release = _input.isRelease;

    if(leftTop.x <= input_.GetMousePos().x && input_.GetMousePos().x <= leftTop.x + _size.x &&
        leftTop.y <= input_.GetMousePos().y && input_.GetMousePos().y <= leftTop.y + _size.y)
    {
        hover = true;
        if(input_.TriggerLeft())
        {
            trigger = true;
        }
        else
        {
            trigger = false;
        }
    }
    else
    {
        hover = false;
        trigger = false;
    }

    if(input_.ReleaseLeft())
    {
        release = true;
    }
    else
    {
        release = false;
    }
}

NiVec2 NiGui::ComputeStandardPoint(NiGui_StandardPoint _stdpoint)
{
    NiVec2 result = {};

    switch (_stdpoint)
    {
    case NiGui_StandardPoint::LeftTop:
        result.x = 0.0f;
        result.y = 0.0f;
        break;

    case NiGui_StandardPoint::CenterTop:
        result.x = 0.5f;
        result.y = 0.0f;
        break;

    case NiGui_StandardPoint::RightTop:
        result.x = 1.0f;
        result.y = 0.0f;
        break;

    case NiGui_StandardPoint::LeftCenter:
        result.x = 0.0f;
        result.y = 0.5f;
        break;

    case NiGui_StandardPoint::Center:
        result.x = 0.5f;
        result.y = 0.5f;
        break;

    case NiGui_StandardPoint::RightCenter:
        result.x = 1.0f;
        result.y = 0.5f;
        break;

    case NiGui_StandardPoint::LeftBottom:
        result.x = 0.0f;
        result.y = 1.0f;
        break;

    case NiGui_StandardPoint::CenterBottom:
        result.x = 0.5f;
        result.y = 1.0f;
        break;

    case NiGui_StandardPoint::RightBottom:
        result.x = 1.0f;
        result.y = 1.0f;
        break;

    default:
        break;
    }

    return result;
}

NiVec2 NiGui::ComputeLeftTop(const NiVec2& _position, const NiVec2& _size, const NiVec2& _parentSize, NiGui_StandardPoint _anchor, NiGui_StandardPoint _pivot)
{
    NiVec2 result = {};

    NiVec2 normalAnchor = ComputeStandardPoint(_anchor);
    NiVec2 normalPivot = ComputeStandardPoint(_pivot);

    result = _position + normalAnchor * _parentSize;
    result -= _size * normalPivot;
    return result;
}

void NiGui::ClearData()
{
    buttonImages_.clear();
    divData_.clear();
    dragItemAreaData_.clear();
    dragItemData_.clear();

    auto& cid = state_.componentID;
    auto& buffer = state_.buffer;

    cid.hover = {};
    cid.typeHover = {};

    buffer.currentRegion = nullptr;
    buffer.currentZOrder = 0;

    if (buffer.areaToItem.size() > setting_.deleteElementThreshold)
    {
        std::vector<std::string> deleteKeys;
        for (auto& areaToItem : buffer.areaToItem)
        {
            if (areaToItem.second.empty())
            {
                deleteKeys.push_back(areaToItem.first);
            }
        }
        for (auto& key : deleteKeys)
        {
            buffer.areaToItem.erase(key);
        }
        if (buffer.areaToItem.size() > setting_.deleteElementThreshold) setting_.deleteElementThreshold *= 2;
    }
    
    return;
}

void NiGui::SavePreData()
{
    // 前フレームのホバー中のコンポーネントIDを保存
    state_.componentID.preHover = state_.componentID.hover;
    state_.componentID.preActive = state_.componentID.active;
}

void NiGui::CopyInputData()
{
    io_.input.isLeftPre = io_.input.isLeft;
    io_.input.isRightPre = io_.input.isRight;
    io_.input.isMiddlePre = io_.input.isMiddle;

    io_.input.isLeft = input_.PressLeft();
    io_.input.isRight = input_.PressRight();
    io_.input.isMiddle = input_.PressMiddle();

    io_.input.cursorPos = input_.GetMousePos();
    io_.input.triggeredPos = input_.GetTriggeredPos();
    io_.input.differencePos = input_.GetDifferencePos();
}

void NiGui::ClampRect(NiVec2& _leftTop, const NiVec2& _size, const NiVec2& _parentPos, const NiVec2& _parentSize)
{
    if (_leftTop.x < _parentPos.x) _leftTop.x = _parentPos.x;
    if (_leftTop.y < _parentPos.y) _leftTop.y = _parentPos.y;
    if (_leftTop.x + _size.x > _parentPos.x + _parentSize.x) _leftTop.x = _parentPos.x + _parentSize.x - _size.x;
    if (_leftTop.y + _size.y > _parentPos.y + _parentSize.y) _leftTop.y = _parentPos.y + _parentSize.y - _size.y;
}

void NiGui::OffsetUpdate(
    const std::string& _id, 
    const NiVec2& _originLeftTop,
    const NiGui_Transform2dEx& _transform,
    NiVec2& _offset)
{
    if (state_.componentID.active == _id)
    {
        _offset += io_.input.differencePos;
    }

    if (input_.ReleaseLeft() && state_.componentID.active == _id)
    {
        if (_transform.position.x == _transform.parentPos.x) _offset.x = _transform.parentPos.x - _originLeftTop.x;
        if (_transform.position.y == _transform.parentPos.y) _offset.y = _transform.parentPos.y - _originLeftTop.y;
        if (_transform.position.x + _transform.size.x == _transform.parentPos.x + _transform.parentSize.x) _offset.x = _transform.parentPos.x + _transform.parentSize.x - _originLeftTop.x - _transform.size.x;
        if (_transform.position.y + _transform.size.y == _transform.parentPos.y + _transform.parentSize.y) _offset.y = _transform.parentPos.y + _transform.parentSize.y - _originLeftTop.y - _transform.size.y;
    }
}

void NiGui::SetComponentId(const NiGui_InputState& _inputState, const std::string& _id, const std::string& _type)
{
    if (_id.empty()) assert(false && "unexpected");

    if (_inputState.isHover)
    {
        state_.componentID.hover = _id;
        state_.componentID.typeHover = _type;
    }
    if (_inputState.isTrigger)
    {
        state_.componentID.active = _id;
        state_.componentID.typeActive = _type;
    }
}

void NiGui::PostProcessComponents()
{
    auto& componentID = state_.componentID;
    auto& componentTime = state_.time;

    if (!io_.input.isLeft && io_.input.isLeftPre)
    {
        componentID.active = {};
        componentID.typeActive = {};
    }

    /// =========
    /// Active
    if (!componentID.active.empty())
    {
        /// アクティブIDが変更されたら
        if (componentID.active != componentID.preActive)
        {
            // アクティブ時間をリセット
            componentTime.active = 0;
        }
        ++componentTime.active;
    }
    else
    {
        componentTime.active = 0;
    }

    if (componentID.typeActive == "DragItem" && !componentID.active.empty())
    {
        int max = state_.buffer.currentZOrder - 1;
        int missingIndex = 0;
        int currentIndex = 0;
        StringMap<int> newZMap;

        for (auto itr = dragItemData_.begin(); itr != dragItemData_.end(); ++itr)
        {
            auto& current = itr->second;
            auto& missing = std::next(dragItemData_.begin(), missingIndex)->second;

            if (current.zOrder < dragItemData_[componentID.active].zOrder)
            {
                newZMap[current.id] = current.zOrder;
                ++currentIndex;
                continue;
            }

            if (currentIndex == 0)
            {
                newZMap[current.id] = current.zOrder;
            }
            else if (current.zOrder > missing.zOrder)
            {
                newZMap[current.id] = missing.zOrder;
                missingIndex = currentIndex;
            }
            else
            {
                newZMap[current.id] = newZMap[missing.id];
                newZMap[missing.id] = current.zOrder;
            }
            ++currentIndex;
        }

        newZMap[componentID.active] = max;
        for (auto& itr : newZMap)
        {
            dragItemData_[itr.first].zOrder = itr.second;
        }
    }

    /// ========
    /// Hover
    if (!componentID.hover.empty())
    {
        if (componentID.hover != componentID.preHover)
        {
            componentTime.hover = 0;
        }
        ++componentTime.hover;
    }
    else
    {
        componentTime.hover = 0;
    }

    /// ========
    /// Play SE
    PlaySE("Button", io_.audioHnd.buttonHover, io_.audioHnd.buttonConfirm, io_.audioHandler.buttonHover, io_.audioHandler.buttonConfirm);
}

void NiGui::PlaySE(const std::string& _type, uint32_t _hoverSE, uint32_t _confirmSE, void* _hoverHandler, void* _confirmHandler)
{
    auto& componentID = state_.componentID;
    auto& componentTime = state_.time;

    if (componentID.typeHover != _type) return;

    if (componentTime.active == 0 && componentID.preActive == componentID.preHover && !componentID.preActive.empty())
    {
        if (_confirmSE != -1)
        {
            drawer_->PlayAudio(_confirmSE);
        }
        else
        {
            drawer_->PlayAudio(_confirmHandler);
        }
    }
    if (componentTime.hover == 1)
    {
        if (_hoverSE != -1)
        {
            drawer_->PlayAudio(_hoverSE);
        }
        else
        {
            drawer_->PlayAudio(_hoverHandler);
        }
    }
}

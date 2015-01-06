#include "StdAfx.h"
#include "Common.h"
#include "graphics//D3DInteropHelper.h"
#include "graphics/color/ColorValue.h"
#include "graphics/color/ColorSet.h"
#include "graphics/brush/BrushSet.h"
#include "graphics/figure/RectangleFigure.h"
#include "graphics//decorator/Frame.h"
#include "graphics/decorator/FrameDecorator.h"
#include "ui/UIListBoxItem.h"
#include "ui/UIListBox.h"

// #define DEBUG_UILISTBOX

const wchar_t* UILISTBOX_COLORSETDEF =
    L"solid(rgba(0x0b0b0b, 1.0)),"
    L"solid(rgba(0x353535, 1.0))";

ui::UIListBox::UIListBox(SCROLL_DIRECTION scrollDirection, UISize preferredBitmapSize) :
    m_scrollDirection(scrollDirection),
    m_preferredBitmapSize(preferredBitmapSize),
    m_preferredTextAreaSize(UISize(0.0f, 0.0f)),
    m_marginBetweenBitmapAndText(4.0f),
    m_marginBetweenItems(4.0f),
    m_itemWidth(0.0f),
    m_itemHeight(0.0f),
    m_scrollBarMarginFromSide(8.0f),
    m_scrollBarThick(20.0f),
    m_rectFigure(std::make_unique<graphics::figure::RectangleFigure>()),
    m_colorSet(std::make_shared<graphics::color::ColorSet>(UILISTBOX_COLORSETDEF)),
    m_scrollBar(std::make_shared<ui::UIScrollBar>()),
    m_displayPositionOffset(0.0f)
{
#ifdef DEBUG_UILISTBOX
    LOG_ENTER(SEVERITY_LEVEL_DEBUG);
#endif
    AddElement(m_scrollBar);
    AddBeforeAddElementCallback([&](std::shared_ptr<UIBase> element)->bool {
        element->SetParentContainer(shared_from_this());
        return true;
    });
    SetFocusable(false);
#ifdef DEBUG_UILISTBOX
    LOG_LEAVE(SEVERITY_LEVEL_DEBUG);
#endif
}

ui::UIListBox::~UIListBox(void)
{
#ifdef DEBUG_UILISTBOX
    LOG_ENTER(SEVERITY_LEVEL_DEBUG);
#endif

    m_scrollBar.reset();

#ifdef DEBUG_UILISTBOX
    LOG_LEAVE(SEVERITY_LEVEL_DEBUG);
#endif
}

HRESULT ui::UIListBox::Initialize(
    graphics::D3DInteropHelper *pD3DInteropHelper)
{
#ifdef DEBUG_UILISTBOX
    LOG_ENTER(SEVERITY_LEVEL_DEBUG);
#endif
    assert(GetWidth() > 0.0f);
    assert(GetHeight() > 0.0f);

    ui::UIListBoxItem::LAYOUT_DIRECTION layoutDirection =
        ui::UIListBoxItem::LAYOUT_DIRECTION_VERTICAL;

    switch (m_scrollDirection) {
    case ui::UIListBox::SCROLL_DIRECTION_HORIZONTAL:
        layoutDirection = ui::UIListBoxItem::LAYOUT_DIRECTION_VERTICAL;
        m_itemHeight = GetHeight();
        if (m_preferredTextAreaSize.height == 0.0f) {
            m_preferredTextAreaSize.height =
                GetHeight() - m_preferredBitmapSize.height - m_marginBetweenBitmapAndText;
        }
        if (m_preferredTextAreaSize.width == 0.0f) {
            m_preferredTextAreaSize.width = m_preferredBitmapSize.width * 3.0f;
        }
        m_itemWidth = m_preferredTextAreaSize.width;
        break;
    case ui::UIListBox::SCROLL_DIRECTION_VERTICAL:
        layoutDirection = ui::UIListBoxItem::LAYOUT_DIRECTION_HORIZONTAL;
        m_itemWidth = GetWidth();
        if (m_preferredTextAreaSize.width == 0.0f) {
            m_preferredTextAreaSize.width =
                GetWidth() - m_preferredBitmapSize.width - m_marginBetweenBitmapAndText;
        }
        if (m_preferredTextAreaSize.height == 0.0f) {
            m_preferredTextAreaSize.height = m_preferredBitmapSize.height;
        }
        m_itemHeight = m_preferredTextAreaSize.height;
        break;
    }
    for (size_t i = 0; i < GetNumberOfElements(); ++i) {
        auto element = GetElement(i);
        auto listBoxItem = std::dynamic_pointer_cast<UIListBoxItem>(element);
        if (listBoxItem.get() != nullptr) {
            listBoxItem->SetLayoutDirection(layoutDirection);
            listBoxItem->SetPreferredBitmapSize(m_preferredBitmapSize);
            listBoxItem->SetPreferredTextAreaSize(m_preferredTextAreaSize);
            listBoxItem->SetBounds(0.0f, 0.0f, m_itemWidth, m_itemHeight);
        }
    }
    CHK_WARN_HRESULT(UIContainer::Initialize(pD3DInteropHelper));

    auto frameDecorator =
        std::make_shared<graphics::decorator::FrameDecorator>();
    frameDecorator->SetColorSet(m_colorSet);
    for (size_t i = 0; i < m_colorSet->GetNumberOfColors(); ++i) {
        frameDecorator->AddFrame(graphics::decorator::Frame(i));
    }
    m_rectFigure->AddDecorator(frameDecorator);

    m_rectFigure->SetX(0.0f);
    m_rectFigure->SetY(0.0f);
    m_rectFigure->SetWidth(GetWidth());
    m_rectFigure->SetHeight(GetHeight());
    m_rectFigure->SetColor(graphics::color::ColorValue(graphics::color::ColorValue::COLOR_TYPE_RGBA, 0x222222, 1.0f),
                           graphics::color::COLOR_PATTERN_FLAT);
    CHK_WARN_HRESULT(m_rectFigure->Initialize(pD3DInteropHelper));

    m_scrollBar->SetParentContainer(shared_from_this());
    m_scrollBar->SetMinValue(0.0f);
    m_scrollBar->SetMaxValue(GetNumberOfElements() - 1.0f);
    m_scrollBar->SetPageSize(1.0f);
    if (m_scrollBar->GetCurrentValue() < m_scrollBar->GetMinValue() ||
            m_scrollBar->GetCurrentValue() > m_scrollBar->GetMaxValue() - m_scrollBar->GetPageSize()) {
        m_scrollBar->SetCurrentValue(0.0f);
    }
    FLOAT frameThick = static_cast<FLOAT>(m_colorSet->GetNumberOfColors());
    if (m_scrollDirection == ui::UIListBox::SCROLL_DIRECTION_HORIZONTAL) {
        m_scrollBar->SetDirection(ui::UIScrollBar::SCROLLBAR_DIRECTION_HORIZONTAL);
        m_scrollBar->SetBounds(
            frameThick + m_scrollBarMarginFromSide,
            GetHeight() - m_scrollBarThick - frameThick - m_scrollBarMarginFromSide,
            GetWidth() - (frameThick + m_scrollBarMarginFromSide) * 2,
            m_scrollBarThick);
    } else {
        m_scrollBar->SetDirection(ui::UIScrollBar::SCROLLBAR_DIRECTION_VERTICAL);
        m_scrollBar->SetBounds(
            GetWidth() - m_scrollBarThick - frameThick - m_scrollBarMarginFromSide,
            frameThick + m_scrollBarMarginFromSide,
            m_scrollBarThick,
            GetHeight() - (frameThick + m_scrollBarMarginFromSide) * 2);
    }
    m_scrollBar->AddPositionChangedCallback([&](FLOAT position) {
        NotifyScrollPositionChanged(position);
    });
    CHK_FATAL_HRESULT(m_scrollBar->Initialize(pD3DInteropHelper));

#ifdef DEBUG_UILISTBOX
    LOG_LEAVE(SEVERITY_LEVEL_DEBUG);
#endif
    return S_OK;
}

HRESULT ui::UIListBox::LayoutComponents()
{
#ifdef DEBUG_UILISTBOX
    LOG_ENTER(SEVERITY_LEVEL_DEBUG);
    LOG_LEAVE(SEVERITY_LEVEL_DEBUG);
#endif
    return S_OK;
}

HRESULT ui::UIListBox::CreateDeviceDependentResources(
    graphics::D3DInteropHelper *pD3DInteropHelper,
    ID2D1RenderTarget *pRenderTarget)
{
#ifdef DEBUG_UILISTBOX
    LOG_ENTER(SEVERITY_LEVEL_DEBUG);
#endif
    CHK_FATAL_HRESULT(UIContainer::CreateDeviceDependentResources(pD3DInteropHelper, pRenderTarget));
    CHK_FATAL_HRESULT(m_rectFigure->CreateDeviceDependentResources(pD3DInteropHelper, pRenderTarget));
    CHK_FATAL_HRESULT(m_scrollBar->CreateDeviceDependentResources(pD3DInteropHelper, pRenderTarget));
#ifdef DEBUG_UILISTBOX
    LOG_LEAVE(SEVERITY_LEVEL_DEBUG);
#endif
    return S_OK;
}

HRESULT ui::UIListBox::Render(
    graphics::D3DInteropHelper *pD3DInteropHelper,
    ID2D1RenderTarget *pRenderTarget)
{
#ifdef DEBUG_UILISTBOX
    LOG_ENTER(SEVERITY_LEVEL_DEBUG);
#endif
    m_rectFigure->Render(pD3DInteropHelper, pRenderTarget, 1.0f);

    FLOAT frameThick = static_cast<FLOAT>(m_colorSet->GetNumberOfColors());
    D2D1_RECT_F clipRect = D2D1::RectF(frameThick, frameThick, GetWidth() - frameThick, GetHeight() - frameThick);
    pRenderTarget->PushAxisAlignedClip(clipRect, D2D1_ANTIALIAS_MODE_PER_PRIMITIVE);


    if (m_scrollDirection == SCROLL_DIRECTION_HORIZONTAL) {
        pD3DInteropHelper->PushMatrix(pRenderTarget, D2D1::Matrix3x2F::Translation(-m_displayPositionOffset, 0.0f));

        FLOAT x = 0.0f;
        FLOAT y = m_marginBetweenItems;

        for (size_t i = 0; i < GetNumberOfElements(); ++i) {
            auto element = GetElement(i);
            auto listBoxItem = std::dynamic_pointer_cast<UIListBoxItem>(element);
            if (listBoxItem.get() != nullptr) {
                x += m_marginBetweenItems;
                pD3DInteropHelper->PushMatrix(pRenderTarget, D2D1::Matrix3x2F::Translation(x, y));
                listBoxItem->Render(pD3DInteropHelper, pRenderTarget);
                x += listBoxItem->GetWidth();
                pD3DInteropHelper->PopMatrix(pRenderTarget);
            }
        }
        pD3DInteropHelper->PopMatrix(pRenderTarget);
    } else {
        pD3DInteropHelper->PushMatrix(pRenderTarget, D2D1::Matrix3x2F::Translation(0.0f, -m_displayPositionOffset));

        FLOAT x = m_marginBetweenItems;
        FLOAT y = 0.0f;

        for (size_t i = 0; i < GetNumberOfElements(); ++i) {
            auto element = GetElement(i);
            auto listBoxItem = std::dynamic_pointer_cast<UIListBoxItem>(element);
            if (listBoxItem.get() != nullptr) {
                y += m_marginBetweenItems;
                pD3DInteropHelper->PushMatrix(pRenderTarget, D2D1::Matrix3x2F::Translation(x, y));
                listBoxItem->Render(pD3DInteropHelper, pRenderTarget);
                y += listBoxItem->GetHeight();
                pD3DInteropHelper->PopMatrix(pRenderTarget);
            }
        }
        pD3DInteropHelper->PopMatrix(pRenderTarget);
    }

    pRenderTarget->PopAxisAlignedClip();

    pD3DInteropHelper->PushMatrix(
        pRenderTarget, D2D1::Matrix3x2F::Translation(m_scrollBar->GetX(), m_scrollBar->GetY()));
    CHK_WARN_HRESULT(m_scrollBar->Render(pD3DInteropHelper, pRenderTarget));
    pD3DInteropHelper->PopMatrix(pRenderTarget);

#ifdef DEBUG_UILISTBOX
    LOG_LEAVE(SEVERITY_LEVEL_DEBUG);
#endif
    return S_OK;
}

void ui::UIListBox::DiscardDeviceDependentResources()
{
    m_rectFigure->DiscardDeviceDependentResources();
}

void ui::UIListBox::NotifyFocusObtained(const std::shared_ptr<UIBase> &uiBase)
{
    for (size_t i = 0; i < GetNumberOfElements(); ++i) {
        auto child = GetElement(i);
        if (uiBase != child) {
            child->LostFocus();
        }
    }
    if (!GetParentContainer().expired()) {
        GetParentContainer().lock()->NotifyFocusObtained(shared_from_this());
    }
}

void ui::UIListBox::OnLeftMouseDown(
    HWND hWnd,
    WPARAM wParam,
    LPARAM lParam,
    UIPoint clientPoint,
    ULONGLONG timestampInMilliSeconds,
    OUT bool* eaten )
{
    ui::UIContainer::OnLeftMouseDown(
        hWnd,
        wParam,
        lParam,
        clientPoint,
        timestampInMilliSeconds,
        eaten);
}

void ui::UIListBox::OnDraggingStart(
    HWND hWnd,
    WPARAM wParam,
    LPARAM lParam,
    UI2Points clientDragRect,
    UIDelta delta,
    ULONGLONG timestampInMilliSeconds,
    OUT bool* eaten )
{
    ui::UIContainer::OnDraggingStart(
        hWnd,
        wParam,
        lParam,
        clientDragRect,
        delta,
        timestampInMilliSeconds,
        eaten);
}

void ui::UIListBox::OnDragging(
    HWND hWnd,
    WPARAM wParam,
    LPARAM lParam,
    UI2Points clientDragRect,
    UIDelta delta,
    ULONGLONG timestampInMilliSeconds,
    OUT bool* eaten )
{
    ui::UIContainer::OnDragging(
        hWnd,
        wParam,
        lParam,
        clientDragRect,
        delta,
        timestampInMilliSeconds,
        eaten);
}

void ui::UIListBox::OnDraggingEnd(
    HWND hWnd,
    WPARAM wParam,
    LPARAM lParam,
    UI2Points clientDragRect,
    UIDelta delta,
    ULONGLONG timestampInMilliSeconds,
    OUT bool* eaten )
{
    ui::UIContainer::OnDraggingEnd(
        hWnd,
        wParam,
        lParam,
        clientDragRect,
        delta,
        timestampInMilliSeconds,
        eaten);
}

void ui::UIListBox::OnMouseOver(
    HWND hWnd,
    WPARAM wParam,
    LPARAM lParam,
    UIPoint clientPoint,
    UIDelta delta,
    ULONGLONG timestampInMilliSeconds,
    OUT bool* eaten )
{
    UNREFERENCED_PARAMETER(hWnd);
    UNREFERENCED_PARAMETER(wParam);
    UNREFERENCED_PARAMETER(lParam);
    UNREFERENCED_PARAMETER(clientPoint);
    UNREFERENCED_PARAMETER(delta);
    UNREFERENCED_PARAMETER(timestampInMilliSeconds);
    UNREFERENCED_PARAMETER(eaten);
}

void ui::UIListBox::OnMouseOut(
    HWND hWnd,
    WPARAM wParam,
    LPARAM lParam,
    UIPoint clientPoint,
    UIDelta delta,
    ULONGLONG timestampInMilliSeconds,
    OUT bool* eaten )
{
    UNREFERENCED_PARAMETER(hWnd);
    UNREFERENCED_PARAMETER(wParam);
    UNREFERENCED_PARAMETER(lParam);
    UNREFERENCED_PARAMETER(clientPoint);
    UNREFERENCED_PARAMETER(delta);
    UNREFERENCED_PARAMETER(timestampInMilliSeconds);
    UNREFERENCED_PARAMETER(eaten);
}

void ui::UIListBox::OnRightSingleClick(
    HWND hWnd,
    WPARAM wParam,
    LPARAM lParam,
    UIPoint clientPoint,
    ULONGLONG timestampInMilliSeconds,
    OUT bool* eaten )
{
    UNREFERENCED_PARAMETER(hWnd);
    UNREFERENCED_PARAMETER(wParam);
    UNREFERENCED_PARAMETER(lParam);
    UNREFERENCED_PARAMETER(clientPoint);
    UNREFERENCED_PARAMETER(timestampInMilliSeconds);
    UNREFERENCED_PARAMETER(eaten);
}

void ui::UIListBox::OnGestureBegin(
    HWND hWnd,
    WPARAM wParam,
    LPARAM lParam,
    UIPoint clientPoint,
    PGESTUREINFO pGestureInfo,
    ULONGLONG timestampInMilliSeconds,
    OUT bool* eaten )
{
    UNREFERENCED_PARAMETER(hWnd);
    UNREFERENCED_PARAMETER(wParam);
    UNREFERENCED_PARAMETER(lParam);
    UNREFERENCED_PARAMETER(clientPoint);
    UNREFERENCED_PARAMETER(pGestureInfo);
    UNREFERENCED_PARAMETER(timestampInMilliSeconds);
    UNREFERENCED_PARAMETER(eaten);
}

void ui::UIListBox::OnGestureEnd(
    HWND hWnd,
    WPARAM wParam,
    LPARAM lParam,
    UIPoint clientPoint,
    PGESTUREINFO pGestureInfo,
    ULONGLONG timestampInMilliSeconds,
    OUT bool* eaten )
{
    UNREFERENCED_PARAMETER(hWnd);
    UNREFERENCED_PARAMETER(wParam);
    UNREFERENCED_PARAMETER(lParam);
    UNREFERENCED_PARAMETER(clientPoint);
    UNREFERENCED_PARAMETER(pGestureInfo);
    UNREFERENCED_PARAMETER(timestampInMilliSeconds);
    UNREFERENCED_PARAMETER(eaten);
}

void ui::UIListBox::OnPan(
    HWND hWnd,
    WPARAM wParam,
    LPARAM lParam,
    UIPoint clientPoint,
    PGESTUREINFO pGestureInfo,
    UIDelta delta,
    ULONGLONG timestampInMilliSeconds,
    OUT bool* eaten )
{
    UNREFERENCED_PARAMETER(hWnd);
    UNREFERENCED_PARAMETER(wParam);
    UNREFERENCED_PARAMETER(lParam);
    UNREFERENCED_PARAMETER(clientPoint);
    UNREFERENCED_PARAMETER(pGestureInfo);
    UNREFERENCED_PARAMETER(delta);
    UNREFERENCED_PARAMETER(timestampInMilliSeconds);
    UNREFERENCED_PARAMETER(eaten);
}

void ui::UIListBox::OnPressAndTap(
    HWND hWnd,
    WPARAM wParam,
    LPARAM lParam,
    UIPoint clientPoint,
    PGESTUREINFO pGestureInfo,
    ULONGLONG timestampInMilliSeconds,
    OUT bool* eaten )
{
    UNREFERENCED_PARAMETER(hWnd);
    UNREFERENCED_PARAMETER(wParam);
    UNREFERENCED_PARAMETER(lParam);
    UNREFERENCED_PARAMETER(clientPoint);
    UNREFERENCED_PARAMETER(pGestureInfo);
    UNREFERENCED_PARAMETER(timestampInMilliSeconds);
    UNREFERENCED_PARAMETER(eaten);
}

void ui::UIListBox::NotifyScrollPositionChanged(FLOAT position)
{
    if (0.0f <= position &&  position <= (GetNumberOfElements() - 1.0f)) {
        switch (m_scrollDirection) {
        case ui::UIListBox::SCROLL_DIRECTION_HORIZONTAL:
            m_displayPositionOffset = (m_itemWidth + m_marginBetweenItems) * position;
            break;
        case ui::UIListBox::SCROLL_DIRECTION_VERTICAL:
            m_displayPositionOffset = (m_itemHeight + m_marginBetweenItems) * position;
            break;
        }
    }
}

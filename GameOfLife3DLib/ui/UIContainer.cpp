#include "StdAfx.h"
#include "Common.h"
#include "graphics/D3DInteropHelper.h"
#include "ui/UIContainer.h"

//#define DEBUG_UICONTAINER

ui::UIContainer::UIContainer(void) :
    m_elements(),
    m_leftDownChild(),
    m_draggingChild(),
    m_mouseOverChild(),
    m_gestureChild()
{
    SetFocusable(false);
}

ui::UIContainer::~UIContainer(void)
{
    m_leftDownChild.reset();
    m_draggingChild.reset();
    m_mouseOverChild.reset();
    m_gestureChild.reset();
    m_elements.clear();
}

HRESULT ui::UIContainer::Initialize( graphics::D3DInteropHelper *pD3DInteropHelper )
{
    for (auto it = m_initializationCallback.begin(); it != m_initializationCallback.end(); ++it) {
        HRESULT hr = (*it)(pD3DInteropHelper);
        if (FAILED(hr)) {
            LOG(SEVERITY_LEVEL_ERROR) << L"initialization failed in initialization callback, hr = " << hr;
            return hr;
        }
    }

    for (size_t i = 0; i < GetNumberOfElements(); ++i) {
        auto element = GetElement(i);
        HRESULT hr = element->Initialize(pD3DInteropHelper);
        if (FAILED(hr)) {
            LOG(SEVERITY_LEVEL_ERROR) << L"failed to initialize a child element, hr = " << hr;
            LOG(SEVERITY_LEVEL_ERROR) << *element;
            return hr;
        }
    }
    return S_OK;
}

HRESULT ui::UIContainer::CreateDeviceDependentResources(
    graphics::D3DInteropHelper *pD3DInteropHelper,
    ID2D1RenderTarget *pRenderTarget )
{
    for (size_t i = 0; i < GetNumberOfElements(); ++i) {
        auto element = GetElement(i);
        HRESULT hr = element->CreateDeviceDependentResources(pD3DInteropHelper, pRenderTarget);
        if (FAILED(hr)) {
            LOG(SEVERITY_LEVEL_ERROR) << L"failed to create device dependent resources for a child element, hr = " << hr;
            LOG(SEVERITY_LEVEL_ERROR) << *element;
            return hr;
        }
    }
    return S_OK;
}

HRESULT ui::UIContainer::LayoutComponents()
{
    UISize containerSize(GetWidth() - GetInsets().left - GetInsets().right,
                         GetHeight() - GetInsets().top - GetInsets().bottom);
    for (size_t i = 0; i < GetNumberOfElements(); ++i) {
        auto element = GetElement(i);
        UIAnchor anchor = element->GetAnchor();
        UIRectangle bounds = element->GetBounds();

        if ((anchor.anchor & ANCHOR_LEFT) != 0) {
            bounds.x = anchor.left;
        }
        if ((anchor.anchor & ANCHOR_TOP) != 0) {
            bounds.y = anchor.top;
        }
        if ((anchor.anchor & ANCHOR_RIGHT) != 0) {
            if ((anchor.anchor & ANCHOR_LEFT) != 0) {
                bounds.width = containerSize.width - anchor.left - anchor.right;
            } else {
                bounds.x = containerSize.width - bounds.width - anchor.right;
            }
        }
        if ((anchor.anchor & ANCHOR_BOTTOM) != 0) {
            if ((anchor.anchor & ANCHOR_TOP) != 0) {
                bounds.height = containerSize.height - anchor.top - anchor.bottom;
            } else {
                bounds.y = containerSize.height - bounds.height - anchor.bottom;
            }
        }
        element->SetBounds(bounds);
    }
    for (size_t i = 0; i < GetNumberOfElements(); ++i) {
        auto element = GetElement(i);
        auto container = std::dynamic_pointer_cast<UIContainer>(element);
        if (container.get() != nullptr) {
            CHK_WARN_HRESULT(container->LayoutComponents());
        }
    }
    return S_OK;
}

HRESULT ui::UIContainer::Render(
    graphics::D3DInteropHelper *pD3DInteropHelper,
    ID2D1RenderTarget *pRenderTarget )
{
    for (size_t i = 0; i < GetNumberOfElements(); ++i) {
        auto element = GetElement(i);
        FLOAT x = element->GetX() + 0.5f;
        FLOAT y = element->GetY() + 0.5f;
        pD3DInteropHelper->PushMatrix(pRenderTarget, D2D1::Matrix3x2F::Translation(x, y));
        HRESULT hr = element->Render(pD3DInteropHelper, pRenderTarget);
        if (FAILED(hr)) {
            LOG(SEVERITY_LEVEL_ERROR) << L"render a child element failed, index = " << i << L", hr = " << hr;
        }
        pD3DInteropHelper->PopMatrix(pRenderTarget);
    }
    return S_OK;
}

void ui::UIContainer::DiscardDeviceDependentResources()
{
    for (size_t i = 0; i < GetNumberOfElements(); ++i) {
        auto element = GetElement(i);
        element->DiscardDeviceDependentResources();
    }
}

void ui::UIContainer::OnChar(
    HWND hWnd,
    WPARAM wParam,
    LPARAM lParam,
    CHAR_TYPE charType,
    wchar_t c,
    ULONGLONG timestampInMilliSeconds,
    OUT bool* eaten )
{
    for (size_t i = 0; i < GetNumberOfElements(); ++i) {
        if (GetElement(i)->HasFocus()) {
            GetElement(i)->OnChar(hWnd, wParam, lParam, charType, c, timestampInMilliSeconds, eaten);
            break;
        }
    }
}

void ui::UIContainer::OnLeftMouseDown(
    HWND hWnd,
    WPARAM wParam,
    LPARAM lParam,
    UIPoint clientPoint,
    ULONGLONG timestampInMilliSeconds,
    OUT bool* eaten )
{
#ifdef DEBUG_UICONTAINER
    LOG_ENTER(SEVERITY_LEVEL_DEBUG) << L"clientPoint = " << clientPoint;
#endif
    for (size_t i = 0; i < GetNumberOfElements(); ++i) {
        auto child = GetElement(i);
#ifdef DEBUG_UICONTAINER
        LOG(SEVERITY_LEVEL_DEBUG) << *child;
#endif
        if (child->HitTest(clientPoint, GetInsets().left, GetInsets().top)) {
#ifdef DEBUG_UICONTAINER
            LOG(SEVERITY_LEVEL_DEBUG) << L"Hit";
#endif
            m_leftDownChild = child;
            child->OnLeftMouseDown(
                hWnd,
                wParam,
                lParam,
                child->Translate(clientPoint, GetInsets().left, GetInsets().top),
                timestampInMilliSeconds,
                eaten);
            if (*eaten) {
                break;
            }
        }
    }
#ifdef DEBUG_UICONTAINER
    LOG_LEAVE(SEVERITY_LEVEL_DEBUG);
#endif
}

void ui::UIContainer::OnLeftMouseUp(
    HWND hWnd,
    WPARAM wParam,
    LPARAM lParam,
    UIPoint clientPoint,
    ULONGLONG timestampInMilliSeconds,
    OUT bool* eaten )
{
#ifdef DEBUG_UICONTAINER
    LOG_ENTER(SEVERITY_LEVEL_DEBUG) << L"clientPoint = " << clientPoint;
#endif
    if (!m_leftDownChild.expired()) {
        auto leftDownChild = m_leftDownChild.lock();
        leftDownChild->OnLeftMouseUp(
            hWnd,
            wParam,
            lParam,
            leftDownChild->Translate(clientPoint, GetInsets().left, GetInsets().top),
            timestampInMilliSeconds,
            eaten);
        m_leftDownChild.reset();
    }
#ifdef DEBUG_UICONTAINER
    LOG_LEAVE(SEVERITY_LEVEL_DEBUG);
#endif
}

void ui::UIContainer::OnSingleClick(
    HWND hWnd,
    WPARAM wParam,
    LPARAM lParam,
    UIPoint clientPoint,
    ULONGLONG timestampInMilliSeconds,
    OUT bool* eaten )
{
#ifdef DEBUG_UICONTAINER
    LOG_ENTER(SEVERITY_LEVEL_DEBUG) << L"clientPoint = " << clientPoint;
#endif
    for (size_t i = 0; i < GetNumberOfElements(); ++i) {
        auto child = GetElement(i);
        if (child->HitTest(clientPoint, GetInsets().left, GetInsets().top)) {
            child->OnSingleClick(
                hWnd,
                wParam,
                lParam,
                child->Translate(clientPoint, GetInsets().left, GetInsets().top),
                timestampInMilliSeconds,
                eaten);
            if (*eaten) {
                break;
            }
        }
    }
#ifdef DEBUG_UICONTAINER
    LOG_LEAVE(SEVERITY_LEVEL_DEBUG);
#endif
}

void ui::UIContainer::OnDoubleClick(
    HWND hWnd,
    WPARAM wParam,
    LPARAM lParam,
    UIPoint clientPoint,
    ULONGLONG timestampInMilliSeconds,
    OUT bool* eaten )
{
#ifdef DEBUG_UICONTAINER
    LOG_ENTER(SEVERITY_LEVEL_DEBUG) << L"clientPoint = " << clientPoint;
#endif
    for (size_t i = 0; i < GetNumberOfElements(); ++i) {
        auto child = GetElement(i);
        if (child->HitTest(clientPoint, GetInsets().left, GetInsets().top)) {
            child->OnDoubleClick(
                hWnd,
                wParam,
                lParam,
                child->Translate(clientPoint, GetInsets().left, GetInsets().top),
                timestampInMilliSeconds,
                eaten);
            if (*eaten) {
                break;
            }
        }
    }
#ifdef DEBUG_UICONTAINER
    LOG_LEAVE(SEVERITY_LEVEL_DEBUG);
#endif
}

void ui::UIContainer::OnDraggingStart(
    HWND hWnd,
    WPARAM wParam,
    LPARAM lParam,
    UI2Points clientDragRect,
    UIDelta delta,
    ULONGLONG timestampInMilliSeconds,
    OUT bool* eaten )
{
#ifdef DEBUG_UICONTAINER
    LOG_ENTER(SEVERITY_LEVEL_DEBUG) << L"clientDragRect = " << clientDragRect << L", delta = " << delta;
#endif
    for (size_t i = 0; i < GetNumberOfElements(); ++i) {
        auto child = GetElement(i);
        if (child->HitTest(clientDragRect.start, GetInsets().left, GetInsets().top)) {
            m_draggingChild = child;
            child->OnDraggingStart(
                hWnd,
                wParam,
                lParam,
                child->Translate(clientDragRect, GetInsets().left, GetInsets().top),
                delta,
                timestampInMilliSeconds,
                eaten);
            if (*eaten) {
                break;
            }
        }
    }
#ifdef DEBUG_UICONTAINER
    LOG_LEAVE(SEVERITY_LEVEL_DEBUG);
#endif
}

void ui::UIContainer::OnDragging(
    HWND hWnd,
    WPARAM wParam,
    LPARAM lParam,
    UI2Points clientDragRect,
    UIDelta delta,
    ULONGLONG timestampInMilliSeconds,
    OUT bool* eaten )
{
#ifdef DEBUG_UICONTAINER
    LOG_ENTER(SEVERITY_LEVEL_DEBUG) << L"clientPoint = " << clientDragRect << L", delta = " << delta;
#endif
    if (!m_draggingChild.expired()) {
        auto draggingChild = m_draggingChild.lock();
        draggingChild->OnDragging(
            hWnd,
            wParam,
            lParam,
            draggingChild->Translate(clientDragRect, GetInsets().left, GetInsets().top),
            delta,
            timestampInMilliSeconds,
            eaten);
    }
#ifdef DEBUG_UICONTAINER
    LOG_LEAVE(SEVERITY_LEVEL_DEBUG);
#endif
}

void ui::UIContainer::OnDraggingEnd(
    HWND hWnd,
    WPARAM wParam,
    LPARAM lParam,
    UI2Points clientDragRect,
    UIDelta delta,
    ULONGLONG timestampInMilliSeconds,
    OUT bool* eaten )
{
#ifdef DEBUG_UICONTAINER
    LOG_ENTER(SEVERITY_LEVEL_DEBUG) << L"clientDragRect = " << clientDragRect << L", delta = " << delta;
#endif
    if (!m_draggingChild.expired()) {
        auto draggingChild = m_draggingChild.lock();
        draggingChild->OnDraggingEnd(
            hWnd,
            wParam,
            lParam,
            draggingChild->Translate(clientDragRect, GetInsets().left, GetInsets().top),
            delta,
            timestampInMilliSeconds,
            eaten);
        m_draggingChild.reset();
    }
#ifdef DEBUG_UICONTAINER
    LOG_LEAVE(SEVERITY_LEVEL_DEBUG);
#endif
}

void ui::UIContainer::OnMouseOver(
    HWND hWnd,
    WPARAM wParam,
    LPARAM lParam,
    UIPoint clientPoint,
    UIDelta delta,
    ULONGLONG timestampInMilliSeconds,
    OUT bool* eaten )
{
#ifdef DEBUG_UICONTAINER
    LOG_ENTER(SEVERITY_LEVEL_DEBUG) << L"clientPoint = " << clientPoint << L", delta = " << delta;
#endif
    for (size_t i = 0; i < GetNumberOfElements(); ++i) {
        auto child = GetElement(i);
        if (child->HitTest(clientPoint, GetInsets().left, GetInsets().top)) {
            if (!m_mouseOverChild.expired()) {
                auto mouseOverChild = m_mouseOverChild.lock();
                if (mouseOverChild == child) {
                    mouseOverChild->OnMouseOver(
                        hWnd,
                        wParam,
                        lParam,
                        mouseOverChild->Translate(clientPoint, GetInsets().left, GetInsets().top),
                        delta,
                        timestampInMilliSeconds,
                        eaten);
                } else {
                    mouseOverChild->OnMouseOut(
                        hWnd,
                        wParam,
                        lParam,
                        mouseOverChild->Translate(clientPoint, GetInsets().left, GetInsets().top),
                        delta,
                        timestampInMilliSeconds,
                        eaten);
                    m_mouseOverChild = child;
                    child->OnMouseOver(
                        hWnd,
                        wParam,
                        lParam,
                        child->Translate(clientPoint, GetInsets().left, GetInsets().top),
                        delta,
                        timestampInMilliSeconds,
                        eaten);
                }
            } else {
                m_mouseOverChild = child;
                child->OnMouseOver(
                    hWnd,
                    wParam,
                    lParam,
                    child->Translate(clientPoint, GetInsets().left, GetInsets().top),
                    delta,
                    timestampInMilliSeconds,
                    eaten);
            }
            if (*eaten) {
                break;
            }
        }
    }
    if (!*eaten) {
        if (!m_mouseOverChild.expired()) {
            auto mouseOverChild = m_mouseOverChild.lock();
#ifdef DEBUG_UICONTAINER
            LOG(SEVERITY_LEVEL_DEBUG) << L"leave from [" << *mouseOverChild << L"]";
#endif
            mouseOverChild->OnMouseOut(
                hWnd,
                wParam,
                lParam,
                mouseOverChild->Translate(clientPoint, GetInsets().left, GetInsets().top),
                delta,
                timestampInMilliSeconds,
                eaten);
            m_mouseOverChild.reset();
        }
    }
#ifdef DEBUG_UICONTAINER
    LOG_LEAVE(SEVERITY_LEVEL_DEBUG);
#endif
}

void ui::UIContainer::OnMouseOut(
    HWND hWnd,
    WPARAM wParam,
    LPARAM lParam,
    UIPoint clientPoint,
    UIDelta delta,
    ULONGLONG timestampInMilliSeconds,
    OUT bool* eaten )
{
#ifdef DEBUG_UICONTAINER
    LOG_ENTER(SEVERITY_LEVEL_DEBUG) << L"clientPoint = " << clientPoint << L", delta = " << delta;
#endif
    if (!m_mouseOverChild.expired()) {
        auto mouseOverChild = m_mouseOverChild.lock();
        mouseOverChild->OnMouseOut(
            hWnd,
            wParam,
            lParam,
            mouseOverChild->Translate(clientPoint, GetInsets().left, GetInsets().top),
            delta,
            timestampInMilliSeconds,
            eaten);
        m_mouseOverChild.reset();
    }
#ifdef DEBUG_UICONTAINER
    LOG_LEAVE(SEVERITY_LEVEL_DEBUG);
#endif
}

void ui::UIContainer::OnRightSingleClick(
    HWND hWnd,
    WPARAM wParam,
    LPARAM lParam,
    UIPoint clientPoint,
    ULONGLONG timestampInMilliSeconds,
    OUT bool* eaten )
{
#ifdef DEBUG_UICONTAINER
    LOG_ENTER(SEVERITY_LEVEL_DEBUG) << L"clientPoint = " << clientPoint;
#endif
    for (size_t i = 0; i < GetNumberOfElements(); ++i) {
        auto child = GetElement(i);
        if (child->HitTest(clientPoint, GetInsets().left, GetInsets().top)) {
            child->OnRightSingleClick(
                hWnd,
                wParam,
                lParam,
                child->Translate(clientPoint, GetInsets().left, GetInsets().top),
                timestampInMilliSeconds,
                eaten);
            if (*eaten) {
                break;
            }
        }
    }
#ifdef DEBUG_UICONTAINER
    LOG_LEAVE(SEVERITY_LEVEL_DEBUG);
#endif
}

void ui::UIContainer::OnGestureBegin(
    HWND hWnd,
    WPARAM wParam,
    LPARAM lParam,
    UIPoint clientPoint,
    PGESTUREINFO pGestureInfo,
    ULONGLONG timestampInMilliSeconds,
    OUT bool* eaten )
{
#ifdef DEBUG_UICONTAINER
    LOG_ENTER(SEVERITY_LEVEL_DEBUG) << L"clientPoint = " << clientPoint;
#endif
    for (size_t i = 0; i < GetNumberOfElements(); ++i) {
        auto child = GetElement(i);
        if (child->HitTest(clientPoint, GetInsets().left, GetInsets().top)) {
            m_gestureChild = child;
            child->OnGestureBegin(
                hWnd,
                wParam,
                lParam,
                child->Translate(clientPoint, GetInsets().left, GetInsets().top),
                pGestureInfo,
                timestampInMilliSeconds,
                eaten);
            if (*eaten) {
                break;
            }
        }
    }
#ifdef DEBUG_UICONTAINER
    LOG_LEAVE(SEVERITY_LEVEL_DEBUG);
#endif
}

void ui::UIContainer::OnGestureEnd(
    HWND hWnd,
    WPARAM wParam,
    LPARAM lParam,
    UIPoint clientPoint,
    PGESTUREINFO pGestureInfo,
    ULONGLONG timestampInMilliSeconds,
    OUT bool* eaten )
{
#ifdef DEBUG_UICONTAINER
    LOG_ENTER(SEVERITY_LEVEL_DEBUG) << L"clientPoint = " << clientPoint;
#endif
    if (!m_gestureChild.expired()) {
        auto gestureChild = m_gestureChild.lock();
        gestureChild->OnGestureEnd(
            hWnd,
            wParam,
            lParam,
            gestureChild->Translate(clientPoint, GetInsets().left, GetInsets().top),
            pGestureInfo,
            timestampInMilliSeconds,
            eaten);
        m_gestureChild.reset();
    }
#ifdef DEBUG_UICONTAINER
    LOG_LEAVE(SEVERITY_LEVEL_DEBUG);
#endif
}

void ui::UIContainer::OnZoom(
    HWND hWnd,
    WPARAM wParam,
    LPARAM lParam,
    UIPoint clientPoint,
    PGESTUREINFO pGestureInfo,
    UIPoint zoomCenterPoint,
    FLOAT zoomFactor,
    ULONGLONG timestampInMilliSeconds,
    OUT bool* eaten )
{
#ifdef DEBUG_UICONTAINER
    LOG_ENTER(SEVERITY_LEVEL_DEBUG)
            << L"clientPoint = " << clientPoint
            << L", zoomCenterPoint = " << zoomCenterPoint
            << L", zoomFactor = " << zoomFactor;
#endif
    if (!m_gestureChild.expired()) {
        auto gestureChild = m_gestureChild.lock();
        gestureChild->OnZoom(
            hWnd,
            wParam,
            lParam,
            gestureChild->Translate(clientPoint, GetInsets().left, GetInsets().top),
            pGestureInfo,
            gestureChild->Translate(zoomCenterPoint, GetInsets().left, GetInsets().top),
            zoomFactor,
            timestampInMilliSeconds,
            eaten);
    }
#ifdef DEBUG_UICONTAINER
    LOG_LEAVE(SEVERITY_LEVEL_DEBUG);
#endif
}

void ui::UIContainer::OnPan(
    HWND hWnd,
    WPARAM wParam,
    LPARAM lParam,
    UIPoint clientPoint,
    PGESTUREINFO pGestureInfo,
    UIDelta delta,
    ULONGLONG timestampInMilliSeconds,
    OUT bool* eaten )
{
#ifdef DEBUG_UICONTAINER
    LOG_ENTER(SEVERITY_LEVEL_DEBUG)
            << L"clientPoint = " << clientPoint << L", delta = " << delta;
#endif
    if (!m_gestureChild.expired()) {
        auto gestureChild = m_gestureChild.lock();
        gestureChild->OnPan(
            hWnd,
            wParam,
            lParam,
            gestureChild->Translate(clientPoint, GetInsets().left, GetInsets().top),
            pGestureInfo,
            delta,
            timestampInMilliSeconds,
            eaten);
    }
#ifdef DEBUG_UICONTAINER
    LOG_LEAVE(SEVERITY_LEVEL_DEBUG);
#endif
}

void ui::UIContainer::OnRotate(
    HWND hWnd,
    WPARAM wParam,
    LPARAM lParam,
    UIPoint clientPoint,
    PGESTUREINFO pGestureInfo,
    UIPoint rotateCenterPoint,
    FLOAT rotateAngle,
    ULONGLONG timestampInMilliSeconds,
    OUT bool* eaten )
{
#ifdef DEBUG_UICONTAINER
    LOG_ENTER(SEVERITY_LEVEL_DEBUG)
            << L"clientPoint = " << clientPoint
            << L", rotateCenterPoint = " << rotateCenterPoint
            << L", rotateAngle = " << rotateAngle;
#endif
    if (!m_gestureChild.expired()) {
        auto gestureChild = m_gestureChild.lock();
        gestureChild->OnRotate(
            hWnd,
            wParam,
            lParam,
            gestureChild->Translate(clientPoint, GetInsets().left, GetInsets().top),
            pGestureInfo,
            gestureChild->Translate(rotateCenterPoint, GetInsets().left, GetInsets().top),
            rotateAngle,
            timestampInMilliSeconds,
            eaten);
    }
#ifdef DEBUG_UICONTAINER
    LOG_LEAVE(SEVERITY_LEVEL_DEBUG);
#endif
}

void ui::UIContainer::OnTwoFingerTap(
    HWND hWnd,
    WPARAM wParam,
    LPARAM lParam,
    UIPoint clientPoint,
    PGESTUREINFO pGestureInfo,
    ULONGLONG timestampInMilliSeconds,
    OUT bool* eaten )
{
#ifdef DEBUG_UICONTAINER
    LOG_ENTER(SEVERITY_LEVEL_DEBUG) << L"clientPoint = " << clientPoint;
#endif
    for (size_t i = 0; i < GetNumberOfElements(); ++i) {
        auto child = GetElement(i);
        if (child->HitTest(clientPoint, GetInsets().left, GetInsets().top)) {
            child->OnTwoFingerTap(
                hWnd,
                wParam,
                lParam,
                child->Translate(clientPoint, GetInsets().left, GetInsets().top),
                pGestureInfo,
                timestampInMilliSeconds,
                eaten);
            if (*eaten) {
                break;
            }
        }
    }
#ifdef DEBUG_UICONTAINER
    LOG_LEAVE(SEVERITY_LEVEL_DEBUG);
#endif
}

void ui::UIContainer::OnPressAndTap(
    HWND hWnd,
    WPARAM wParam,
    LPARAM lParam,
    UIPoint clientPoint,
    PGESTUREINFO pGestureInfo,
    ULONGLONG timestampInMilliSeconds,
    OUT bool* eaten )
{
#ifdef DEBUG_UICONTAINER
    LOG_ENTER(SEVERITY_LEVEL_DEBUG) << L"clientPoint = " << clientPoint;
#endif
    for (size_t i = 0; i < GetNumberOfElements(); ++i) {
        auto child = GetElement(i);
        if (child->HitTest(clientPoint, GetInsets().left, GetInsets().top)) {
            child->OnPressAndTap(
                hWnd,
                wParam,
                lParam,
                child->Translate(clientPoint, GetInsets().left, GetInsets().top),
                pGestureInfo,
                timestampInMilliSeconds,
                eaten);
            if (*eaten) {
                break;
            }
        }
    }
#ifdef DEBUG_UICONTAINER
    LOG_LEAVE(SEVERITY_LEVEL_DEBUG);
#endif
}

std::wostream& ui::UIContainer::Dump( std::wostream& os ) const
{
    UIBase::Dump(os);
    os << L",numberOfElements=" << m_elements.size();
    return os;
}

void ui::UIContainer::OnSetFocus(
    HWND hWnd,
    WPARAM wParam,
    LPARAM lParam,
    ULONGLONG timestampInMilliSeconds,
    OUT bool* eaten )
{
    UNREFERENCED_PARAMETER(hWnd);
    UNREFERENCED_PARAMETER(wParam);
    UNREFERENCED_PARAMETER(lParam);
    UNREFERENCED_PARAMETER(timestampInMilliSeconds);
    UNREFERENCED_PARAMETER(eaten);
}

void ui::UIContainer::OnKillFocus(
    HWND hWnd,
    WPARAM wParam,
    LPARAM lParam,
    ULONGLONG timestampInMilliSeconds,
    OUT bool* eaten )
{
    UNREFERENCED_PARAMETER(hWnd);
    UNREFERENCED_PARAMETER(wParam);
    UNREFERENCED_PARAMETER(lParam);
    UNREFERENCED_PARAMETER(timestampInMilliSeconds);
    UNREFERENCED_PARAMETER(eaten);
}

std::wostream& ui::operator<<( std::wostream& os, ui::UIContainer const& container )
{
    os << L"UIContainer[";
    container.Dump(os);
    os << L"]";
    return os;
}

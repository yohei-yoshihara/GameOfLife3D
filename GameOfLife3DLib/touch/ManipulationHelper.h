#ifndef TOUCH_MANIPULATIONHELPER_H_
#define TOUCH_MANIPULATIONHELPER_H_

#include "ui/UICommon.h"
#include "touch/AbstractManipulationHelper.h"

namespace touch
{
enum MANIPULATION_STATUS {
    MANIPULATION_STATUS_NORMAL,
    MANIPULATION_STATUS_MOVING_NODE,
    MANIPULATION_STATUS_MOVING_NODE_WITH_PAN_GESTURE,
    MANIPULATION_STATUS_RESIZING_NODE,
    MANIPULATION_STATUS_RESIZING_NODE_WITH_ZOOM_GESTURE,
    MANIPULATION_STATUS_SCROLLING,
    MANIPULATION_STATUS_SCROLLING_WITH_PAN_GESTURE
};

class ManipulationHelper: public AbstractManipulationHelper
{
private:
    MANIPULATION_STATUS m_manipulationStatus;
    // size of node before resizing
    D2D1_RECT_F m_rectBeforeResize;
    // preferred size of the resizing node
    D2D1_RECT_F m_rectOnResize;
    // scroll start client point
    POINT m_scrollStartPointInWindow;
    FLOAT m_hitTestMargin;
    POINTS m_panMovingPoint;
    std::weak_ptr<ui::UIContainer> m_container;

public:
    ManipulationHelper();
    virtual ~ManipulationHelper();
    virtual void SetUIContainer(std::weak_ptr<ui::UIContainer> container) {
        m_container = container;
    }
    virtual ManipulationResult OnLeftMouseDownExt(
        HWND hWnd,
        WPARAM wParam,
        LPARAM lParam,
        POINT windowPoint,
        ULONGLONG timestampInMilliSeconds);
    virtual ManipulationResult OnLeftMouseUpExt(
        HWND hWnd,
        WPARAM wParam,
        LPARAM lParam,
        POINT windowPoint,
        ULONGLONG timestampInMilliSeconds);
    virtual ManipulationResult OnSingleClick(
        HWND hWnd,
        WPARAM wParam,
        LPARAM lParam,
        POINT windowPoint,
        ULONGLONG timestampInMilliSeconds);
    virtual ManipulationResult OnDoubleClick(
        HWND hWnd,
        WPARAM wParam,
        LPARAM lParam,
        POINT windowPoint,
        ULONGLONG timestampInMilliSeconds);
    virtual ManipulationResult OnDraggingStart(
        HWND hWnd,
        WPARAM wParam,
        LPARAM lParam,
        RECT windowDragRect,
        LONG dx,
        LONG dy,
        ULONGLONG timestampInMilliSeconds);
    virtual ManipulationResult OnDragging(
        HWND hWnd,
        WPARAM wParam,
        LPARAM lParam,
        RECT windowDragRect,
        LONG dx,
        LONG dy,
        ULONGLONG timestampInMilliSeconds);
    virtual ManipulationResult OnDraggingEnd(
        HWND hWnd,
        WPARAM wParam,
        LPARAM lParam,
        RECT windowDragRect,
        LONG dx,
        LONG dy,
        ULONGLONG timestampInMilliSeconds);
    virtual ManipulationResult OnMouseOver(
        HWND hWnd,
        WPARAM wParam,
        LPARAM lParam,
        POINT windowPoint,
        LONG dx,
        LONG dy,
        ULONGLONG timestampInMilliSeconds);
    virtual ManipulationResult OnMouseOut(
        HWND hWnd,
        WPARAM wParam,
        LPARAM lParam,
        POINT windowPoint,
        LONG dx,
        LONG dy,
        ULONGLONG timestampInMilliSeconds);
    virtual ManipulationResult OnRightSingleClick(
        HWND hWnd,
        WPARAM wParam,
        LPARAM lParam,
        POINT windowPoint,
        ULONGLONG timestampInMilliSeconds);
    virtual ManipulationResult OnGestureBegin(
        HWND hWnd,
        WPARAM wParam,
        LPARAM lParam,
        POINT windowPoint,
        ULONGLONG timestampInMilliSeconds,
        PGESTUREINFO pGestureInfo);
    virtual ManipulationResult OnGestureEnd(
        HWND hWnd,
        WPARAM wParam,
        LPARAM lParam,
        POINT windowPoint,
        ULONGLONG timestampInMilliSeconds,
        PGESTUREINFO pGestureInfo);
    virtual ManipulationResult OnZoom(
        HWND hWnd,
        WPARAM wParam,
        LPARAM lParam,
        POINT windowPoint,
        ULONGLONG timestampInMilliSeconds,
        PGESTUREINFO pGestureInfo,
        POINT zoomCenterPoint,
        FLOAT zoomFactor);
    virtual ManipulationResult OnPan(
        HWND hWnd,
        WPARAM wParam,
        LPARAM lParam,
        POINT windowPoint,
        ULONGLONG timestampInMilliSeconds,
        PGESTUREINFO pGestureInfo,
        LONG dx,
        LONG dy);
    virtual ManipulationResult OnRotate(
        HWND hWnd,
        WPARAM wParam,
        LPARAM lParam,
        POINT windowPoint,
        ULONGLONG timestampInMilliSeconds,
        PGESTUREINFO pGestureInfo,
        POINT rotateCenterPoint,
        FLOAT rotateAngle);
    virtual ManipulationResult OnTwoFingerTap(
        HWND hWnd,
        WPARAM wParam,
        LPARAM lParam,
        POINT windowPoint,
        ULONGLONG timestampInMilliSeconds,
        PGESTUREINFO pGestureInfo);
    virtual ManipulationResult OnPressAndTap(
        HWND hWnd,
        WPARAM wParam,
        LPARAM lParam,
        POINT windowPoint,
        ULONGLONG timestampInMilliSeconds,
        PGESTUREINFO pGestureInfo);
    virtual ManipulationResult OnSetFocus(
        HWND hWnd,
        WPARAM wParam,
        LPARAM lParam,
        ULONGLONG timestampInMilliSeconds);
    virtual ManipulationResult OnKillFocus(
        HWND hWnd,
        WPARAM wParam,
        LPARAM lParam,
        ULONGLONG timestampInMilliSeconds);
    virtual ManipulationResult OnChar(
        HWND hWnd,
        WPARAM wParam,
        LPARAM lParam,
        CHAR_TYPE charType,
        wchar_t c,
        ULONGLONG timestampInMilliSeconds);
    MANIPULATION_STATUS GetManipulationStatus() const {
        return m_manipulationStatus;
    }
    bool IsResizing() const {
        return (m_manipulationStatus == MANIPULATION_STATUS_RESIZING_NODE ||
                m_manipulationStatus == MANIPULATION_STATUS_RESIZING_NODE_WITH_ZOOM_GESTURE);
    }
    D2D1_RECT_F GetResizingNodeRect() const {
        return m_rectOnResize;
    }
    void SetHitTestMargin(FLOAT hitTestMargin) {
        m_hitTestMargin = hitTestMargin;
    }
    FLOAT GetHitTestMargin() const {
        return m_hitTestMargin;
    }
    const POINTS& GetPanMovingPoint() const {
        return m_panMovingPoint;
    }
};
}

#endif // TOUCH_MANIPULATIONHELPER_H_
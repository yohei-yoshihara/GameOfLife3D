#ifndef UI_EVENT_IUIINPUTEVENT_H_
#define UI_EVENT_IUIINPUTEVENT_H_

#include "ui/UICommon.h"

namespace ui
{
namespace event
{
class IUIInputEvent
{
public:
    virtual void OnSetFocus(
        HWND hWnd,
        WPARAM wParam,
        LPARAM lParam,
        ULONGLONG timestampInMilliSeconds,
        OUT bool* eaten) = 0;
    virtual void OnKillFocus(
        HWND hWnd,
        WPARAM wParam,
        LPARAM lParam,
        ULONGLONG timestampInMilliSeconds,
        OUT bool* eaten) = 0;
    virtual void OnChar(
        HWND hWnd,
        WPARAM wParam,
        LPARAM lParam,
        CHAR_TYPE charType,
        wchar_t c,
        ULONGLONG timestampInMilliSeconds,
        OUT bool* eaten) = 0;
    virtual void OnLeftMouseDown(
        HWND hWnd,
        WPARAM wParam,
        LPARAM lParam,
        UIPoint clientPoint,
        ULONGLONG timestampInMilliSeconds,
        OUT bool* eaten) = 0;
    virtual void OnLeftMouseUp(
        HWND hWnd,
        WPARAM wParam,
        LPARAM lParam,
        UIPoint clientPoint,
        ULONGLONG timestampInMilliSeconds,
        OUT bool* eaten) = 0;
    virtual void OnSingleClick(
        HWND hWnd,
        WPARAM wParam,
        LPARAM lParam,
        UIPoint clientPoint,
        ULONGLONG timestampInMilliSeconds,
        OUT bool* eaten) = 0;
    virtual void OnDoubleClick(
        HWND hWnd,
        WPARAM wParam,
        LPARAM lParam,
        UIPoint clientPoint,
        ULONGLONG timestampInMilliSeconds,
        OUT bool* eaten) = 0;
    virtual void OnDraggingStart(
        HWND hWnd,
        WPARAM wParam,
        LPARAM lParam,
        UI2Points clientDragRect,
        UIDelta delta,
        ULONGLONG timestampInMilliSeconds,
        OUT bool* eaten) = 0;
    virtual void OnDragging(
        HWND hWnd,
        WPARAM wParam,
        LPARAM lParam,
        UI2Points clientDragRect,
        UIDelta delta,
        ULONGLONG timestampInMilliSeconds,
        OUT bool* eaten) = 0;
    virtual void OnDraggingEnd(
        HWND hWnd,
        WPARAM wParam,
        LPARAM lParam,
        UI2Points clientDragRect,
        UIDelta delta,
        ULONGLONG timestampInMilliSeconds,
        OUT bool* eaten) = 0;
    virtual void OnMouseOver(
        HWND hWnd,
        WPARAM wParam,
        LPARAM lParam,
        UIPoint clientPoint,
        UIDelta delta,
        ULONGLONG timestampInMilliSeconds,
        OUT bool* eaten) = 0;
    virtual void OnMouseOut(
        HWND hWnd,
        WPARAM wParam,
        LPARAM lParam,
        UIPoint clientPoint,
        UIDelta delta,
        ULONGLONG timestampInMilliSeconds,
        OUT bool* eaten) = 0;
    virtual void OnRightSingleClick(
        HWND hWnd,
        WPARAM wParam,
        LPARAM lParam,
        UIPoint clientPoint,
        ULONGLONG timestampInMilliSeconds,
        OUT bool* eaten) = 0;
    virtual void OnGestureBegin(
        HWND hWnd,
        WPARAM wParam,
        LPARAM lParam,
        UIPoint clientPoint,
        PGESTUREINFO pGestureInfo,
        ULONGLONG timestampInMilliSeconds,
        OUT bool* eaten) = 0;
    virtual void OnGestureEnd(
        HWND hWnd,
        WPARAM wParam,
        LPARAM lParam,
        UIPoint clientPoint,
        PGESTUREINFO pGestureInfo,
        ULONGLONG timestampInMilliSeconds,
        OUT bool* eaten) = 0;
    virtual void OnZoom(
        HWND hWnd,
        WPARAM wParam,
        LPARAM lParam,
        UIPoint clientPoint,
        PGESTUREINFO pGestureInfo,
        UIPoint zoomCenterPoint,
        FLOAT zoomFactor,
        ULONGLONG timestampInMilliSeconds,
        OUT bool* eaten) = 0;
    virtual void OnPan(
        HWND hWnd,
        WPARAM wParam,
        LPARAM lParam,
        UIPoint clientPoint,
        PGESTUREINFO pGestureInfo,
        UIDelta delta,
        ULONGLONG timestampInMilliSeconds,
        OUT bool* eaten) = 0;
    virtual void OnRotate(
        HWND hWnd,
        WPARAM wParam,
        LPARAM lParam,
        UIPoint clientPoint,
        PGESTUREINFO pGestureInfo,
        UIPoint rotateCenterPoint,
        FLOAT rotateAngle,
        ULONGLONG timestampInMilliSeconds,
        OUT bool* eaten) = 0;
    virtual void OnTwoFingerTap(
        HWND hWnd,
        WPARAM wParam,
        LPARAM lParam,
        UIPoint clientPoint,
        PGESTUREINFO pGestureInfo,
        ULONGLONG timestampInMilliSeconds,
        OUT bool* eaten) = 0;
    virtual void OnPressAndTap(
        HWND hWnd,
        WPARAM wParam,
        LPARAM lParam,
        UIPoint clientPoint,
        PGESTUREINFO pGestureInfo,
        ULONGLONG timestampInMilliSeconds,
        OUT bool* eaten) = 0;
};
}
}

#endif // UI_EVENT_IUIINPUTEVENT_H_
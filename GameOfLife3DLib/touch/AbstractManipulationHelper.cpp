#include "StdAfx.h"
#include "touch/AbstractManipulationHelper.h"
#include "util/Utils.h"

using namespace touch;

UINT_PTR AbstractManipulationHelper::m_masterTimerId = 1;

AbstractManipulationHelper::AbstractManipulationHelper(void) :
    m_draggingStatus(DRAGGING_STATUS_NOT_DRAGGING),
    m_timerIdForDoubleClick(m_masterTimerId++),
    m_hCursor(nullptr),
    m_hCursor_Arrow(nullptr),
    m_hCursor_SizeNWSE(nullptr),
    m_hCursor_SizeNESW(nullptr),
    m_hCursor_SizeWE(nullptr),
    m_hCursor_SizeNS(nullptr),
    m_hCursor_SizeALL(nullptr),
    m_hCursor_No(nullptr),
    m_hCursor_Hand(nullptr)
{
    m_lastPoint.x = -1;
    m_lastPoint.y = -1;
}

AbstractManipulationHelper::~AbstractManipulationHelper(void)
{
}

ManipulationResult AbstractManipulationHelper::_OnLeftMouseDown(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    UNREFERENCED_PARAMETER(message);

    ULONGLONG currentTimestampInMilliSeconds = GetCurrentTimestampInMilliSeconds();
    m_draggingStatus = DRAGGING_STATUS_PREPARE;
    m_lastPoint.x = GET_X_LPARAM(lParam);
    m_lastPoint.y = GET_Y_LPARAM(lParam);
    ::SetCapture(hWnd);

    POINT windowPoint;
    windowPoint.x = GET_X_LPARAM(lParam);
    windowPoint.y = GET_Y_LPARAM(lParam);
    return OnLeftMouseDownExt(hWnd, wParam, lParam, windowPoint, currentTimestampInMilliSeconds);
}

ManipulationResult AbstractManipulationHelper::_OnMouseMove(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    UNREFERENCED_PARAMETER(message);

    bool leftButtonDown = (wParam & MK_LBUTTON) != 0;

    ManipulationResult result;
    ULONGLONG currentTimestampInMilliSeconds = GetCurrentTimestampInMilliSeconds();
    if (leftButtonDown && m_draggingStatus == DRAGGING_STATUS_PREPARE) {
        // start mouse dragging
        m_dragRect.left = m_lastPoint.x;
        m_dragRect.top = m_lastPoint.y;
        m_dragRect.right = GET_X_LPARAM(lParam);
        m_dragRect.bottom = GET_Y_LPARAM(lParam);
        LONG dx = 0;
        LONG dy = 0;
        if (m_lastPoint.x != -1 && m_lastPoint.y != -1) {
            dx = m_dragRect.right - m_lastPoint.x;
            dy = m_dragRect.bottom - m_lastPoint.y;
        }
        result = OnDraggingStart(hWnd, wParam, lParam, m_dragRect, dx, dy, currentTimestampInMilliSeconds);
        ::SetCapture(hWnd);
        m_draggingStatus = DRAGGING_STATUS_DRAGGING;
    } else if (leftButtonDown && m_draggingStatus == DRAGGING_STATUS_DRAGGING) {
        // Mouse dragging
        m_dragRect.right = GET_X_LPARAM(lParam);
        m_dragRect.bottom = GET_Y_LPARAM(lParam);
        LONG dx = 0;
        LONG dy = 0;
        if (m_lastPoint.x != -1 && m_lastPoint.y != -1) {
            dx = m_dragRect.right - m_lastPoint.x;
            dy = m_dragRect.bottom - m_lastPoint.y;
        }
        result = OnDragging(hWnd, wParam, lParam, m_dragRect, dx, dy, currentTimestampInMilliSeconds);
        ::SetCapture(hWnd);
    } else if (!leftButtonDown && m_draggingStatus == DRAGGING_STATUS_DRAGGING) {
        // mouse left button up but WM_LBUTTONUP event has not been received.
        // usually this situation does not happen
        m_dragRect.right = GET_X_LPARAM(lParam);
        m_dragRect.bottom = GET_Y_LPARAM(lParam);
        LONG dx = 0;
        LONG dy = 0;
        if (m_lastPoint.x != -1 && m_lastPoint.y != -1) {
            dx = m_dragRect.right - m_lastPoint.x;
            dy = m_dragRect.bottom - m_lastPoint.y;
        }
        result = OnDraggingEnd(hWnd, wParam, lParam, m_dragRect, dx, dy, currentTimestampInMilliSeconds);
        ::ReleaseCapture();
        m_draggingStatus = DRAGGING_STATUS_NOT_DRAGGING;
    } else {
        // Mouse move
        POINT windowPoint;
        windowPoint.x = GET_X_LPARAM(lParam);
        windowPoint.y = GET_Y_LPARAM(lParam);
        LONG dx = 0;
        LONG dy = 0;
        if (m_lastPoint.x != -1 && m_lastPoint.y != -1) {
            dx = windowPoint.x - m_lastPoint.x;
            dy = windowPoint.y - m_lastPoint.y;
        }
        RECT rect;
        GetClientRect(hWnd, &rect);
        int width = rect.right - rect.left;
        int height = rect.bottom - rect.top;
        if (0 <= windowPoint.x && windowPoint.x <= width &&
                0 <= windowPoint.y && windowPoint.y <= height) {
            result = OnMouseOver(hWnd, wParam, lParam, windowPoint, dx, dy, currentTimestampInMilliSeconds);
            ::SetCapture(hWnd);
        } else {
            result = OnMouseOut(hWnd, wParam, lParam, windowPoint, dx, dy, currentTimestampInMilliSeconds);
            ::ReleaseCapture();
        }
        m_draggingStatus = DRAGGING_STATUS_NOT_DRAGGING;
    }
    m_lastPoint.x = GET_X_LPARAM(lParam);
    m_lastPoint.y = GET_Y_LPARAM(lParam);

    return result;
}

ManipulationResult AbstractManipulationHelper::_OnLeftMouseDoubleClick(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    UNREFERENCED_PARAMETER(message);

    ULONGLONG currentTimestampInMilliSeconds = GetCurrentTimestampInMilliSeconds();
    m_draggingStatus = DRAGGING_STATUS_NOT_DRAGGING;
    // Left mouse button double click
    if (! ::KillTimer(hWnd, m_timerIdForDoubleClick)) {
        DWORD lastError = GetLastError();
        LOG(SEVERITY_LEVEL_ERROR) << L"KillTimer failed with error " << lastError;
        ManipulationResult result(false, false);
        return result;
    }
    POINT windowPoint;
    windowPoint.x = GET_X_LPARAM(lParam);
    windowPoint.y = GET_Y_LPARAM(lParam);
    return OnDoubleClick(hWnd, wParam, lParam, windowPoint, currentTimestampInMilliSeconds);
}

ManipulationResult AbstractManipulationHelper::_OnLeftMouseUp(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    UNREFERENCED_PARAMETER(message);

    ManipulationResult result1;
    ULONGLONG currentTimestampInMilliSeconds = GetCurrentTimestampInMilliSeconds();
    ::ReleaseCapture();
    POINT windowPoint;
    windowPoint.x = GET_X_LPARAM(lParam);
    windowPoint.y = GET_Y_LPARAM(lParam);
    if (IsDragging()) {
        // Mouse dragging end
        m_dragRect.right = windowPoint.x;
        m_dragRect.bottom = windowPoint.y;
        LONG dx = 0;
        LONG dy = 0;
        if (m_lastPoint.x != -1 && m_lastPoint.y != -1) {
            dx = m_dragRect.right - m_lastPoint.x;
            dy = m_dragRect.bottom - m_lastPoint.y;
        }
        result1 = OnDraggingEnd(hWnd, wParam, lParam, m_dragRect, dx, dy, currentTimestampInMilliSeconds);
    } else {
        m_lBtnUpPoint.x = windowPoint.x;
        m_lBtnUpPoint.y = windowPoint.y;
        UINT_PTR ret;
        // NOTE: the first SetTimer request is always slow (over 500ms)
        if ((ret = ::SetTimer(hWnd, m_timerIdForDoubleClick, GetDoubleClickTime(), nullptr)) == 0) {
            DWORD lastError = GetLastError();
            LOG(SEVERITY_LEVEL_ERROR) << L"SetTimer failed with error " << lastError;
            ManipulationResult result(false, false);
            return result;
        }
    }
    ManipulationResult result2 = OnLeftMouseUpExt(hWnd, wParam, lParam, windowPoint, currentTimestampInMilliSeconds);
    m_draggingStatus = DRAGGING_STATUS_NOT_DRAGGING;
    return ManipulationResult(
               result1.wasHandled || result2.wasHandled,
               result1.requireInvalidateRect || result1.requireInvalidateRect);
}

ManipulationResult AbstractManipulationHelper::_OnTimer(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    UNREFERENCED_PARAMETER(message);

    ManipulationResult result;
    ULONGLONG currentTimestampInMilliSeconds = GetCurrentTimestampInMilliSeconds();
    if (wParam == m_timerIdForDoubleClick) {
        if (! ::KillTimer(hWnd, m_timerIdForDoubleClick)) {
            LOG(SEVERITY_LEVEL_ERROR) << L"KillTimer failed";
            ManipulationResult result(false, false);
            return result;
        }
        m_draggingStatus = DRAGGING_STATUS_NOT_DRAGGING;
        // Left mouse button single click
        result = OnSingleClick(hWnd, wParam, lParam, m_lBtnUpPoint, currentTimestampInMilliSeconds);
    } else {
        result.requireInvalidateRect = true;
        result.wasHandled = true;
    }
    return result;
}

ManipulationResult AbstractManipulationHelper::_OnTouch(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    UNREFERENCED_PARAMETER(hWnd);
    UNREFERENCED_PARAMETER(message);
    UNREFERENCED_PARAMETER(wParam);
    UNREFERENCED_PARAMETER(lParam);

    ManipulationResult result;
    result.requireInvalidateRect = true;
    result.wasHandled = true;
    return result;
}


ManipulationResult AbstractManipulationHelper::_OnRightMouseDown(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    UNREFERENCED_PARAMETER(message);

    ManipulationResult result;
    ULONGLONG currentTimestampInMilliSeconds = GetCurrentTimestampInMilliSeconds();
    POINT windowPoint;
    windowPoint.x = GET_X_LPARAM(lParam);
    windowPoint.y = GET_Y_LPARAM(lParam);
    return OnRightSingleClick(hWnd, wParam, lParam, windowPoint, currentTimestampInMilliSeconds);
}

bool AbstractManipulationHelper::IsDragging()
{
    return m_draggingStatus == DRAGGING_STATUS_DRAGGING;
}

ULONGLONG AbstractManipulationHelper::GetCurrentTimestampInMilliSeconds()
{
    SYSTEMTIME st;
    GetSystemTime(&st);
    FILETIME ft;
    SystemTimeToFileTime(&st, &ft);
    ULARGE_INTEGER ui;
    ui.HighPart = ft.dwHighDateTime;
    ui.LowPart = ft.dwLowDateTime;
    return ui.QuadPart / 10;
}

ManipulationResult AbstractManipulationHelper::_OnGesture( HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam )
{
    UNREFERENCED_PARAMETER(message);
    ManipulationResult result;
    ULONGLONG currentTimestampInMilliSeconds = GetCurrentTimestampInMilliSeconds();
    GESTUREINFO gi = {0};
    gi.cbSize = sizeof(gi);
    BOOL bResult = GetGestureInfo((HGESTUREINFO)lParam, &gi);
    if (!bResult) {
        LOG(SEVERITY_LEVEL_ERROR) << L"GetGestureInfo failed, err=" << GetLastError();
        result.requireInvalidateRect = false;
        result.wasHandled = false;
        return result;
    }
    POINT windowPoint;
    windowPoint.x = gi.ptsLocation.x;
    windowPoint.y = gi.ptsLocation.y;
    ScreenToClient(hWnd, &windowPoint);

    switch (gi.dwID) {
    case GID_BEGIN:
        result = OnGestureBegin(hWnd, wParam, lParam, windowPoint, currentTimestampInMilliSeconds, &gi);
        break;
    case GID_END:
        result = OnGestureEnd(hWnd, wParam, lParam, windowPoint, currentTimestampInMilliSeconds, &gi);
        break;
    case GID_ZOOM:
        switch (gi.dwFlags) {
        case GF_BEGIN:
            m_gestureArgument = LODWORD(gi.ullArguments);
            m_lastPoint = windowPoint;
            result = OnZoom(hWnd, wParam, lParam, windowPoint, currentTimestampInMilliSeconds, &gi, windowPoint, 0.0f);
            break;
        default:
            POINT zoomCenterPoint;
            zoomCenterPoint.x = (m_lastPoint.x + windowPoint.x) / 2;
            zoomCenterPoint.y = (m_lastPoint.y + windowPoint.y) / 2;
            FLOAT zoomFactor = static_cast<FLOAT>(LODWORD(gi.ullArguments)) / static_cast<FLOAT>(m_gestureArgument);
            result = OnZoom(hWnd, wParam, lParam, windowPoint, currentTimestampInMilliSeconds, &gi, zoomCenterPoint, zoomFactor);
            m_gestureArgument = LODWORD(gi.ullArguments);
            m_lastPoint = windowPoint;
            break;
        }
        break;
    case GID_PAN:
        switch (gi.dwFlags) {
        case GF_BEGIN:
            m_lastPoint = windowPoint;
            result = OnPan(hWnd, wParam, lParam, windowPoint, currentTimestampInMilliSeconds, &gi, 0, 0);
            break;
        default:
            result = OnPan(hWnd, wParam, lParam, windowPoint, currentTimestampInMilliSeconds, &gi,
                           windowPoint.x - m_lastPoint.x,
                           windowPoint.y - m_lastPoint.y);
            m_lastPoint = windowPoint;
            break;
        }
        break;
    case GID_ROTATE:
        switch (gi.dwFlags) {
        case GF_BEGIN:
            m_lastPoint = windowPoint;
            m_gestureArgument = 0;
            result = OnRotate(hWnd, wParam, lParam, windowPoint, currentTimestampInMilliSeconds, &gi, windowPoint, 0.0f);
            break;
        default:
            FLOAT rotateAngle = static_cast<FLOAT>(GID_ROTATE_ANGLE_FROM_ARGUMENT(LODWORD(gi.ullArguments)) - GID_ROTATE_ANGLE_FROM_ARGUMENT(m_gestureArgument));
            result = OnRotate(hWnd, wParam, lParam, windowPoint, currentTimestampInMilliSeconds, &gi, windowPoint, rotateAngle);
            m_gestureArgument = LODWORD(gi.ullArguments);
            break;
        }
        break;
    case GID_TWOFINGERTAP:
        result = OnTwoFingerTap(hWnd, wParam, lParam, windowPoint, currentTimestampInMilliSeconds, &gi);
        break;
    case GID_PRESSANDTAP:
        result = OnPressAndTap(hWnd, wParam, lParam, windowPoint, currentTimestampInMilliSeconds, &gi);
        break;
    }

    CloseGestureInfoHandle((HGESTUREINFO)lParam);
    return result;
}

void AbstractManipulationHelper::SetMouseCursor( MOUSE_CURSOR_TYPE mouseCursorType )
{
    switch (mouseCursorType) {
    case MOUSE_CURSOR_TYPE_ARROW:
        m_hCursor = LoadCursor(nullptr, IDC_ARROW);
        break;
    case MOUSE_CURSOR_TYPE_SIZENWSE:
        m_hCursor = LoadCursor(nullptr, IDC_SIZENWSE);
        break;
    case MOUSE_CURSOR_TYPE_SIZENESW:
        m_hCursor = LoadCursor(nullptr, IDC_SIZENESW);
        break;
    case MOUSE_CURSOR_TYPE_SIZEWE:
        m_hCursor = LoadCursor(nullptr, IDC_SIZEWE);
        break;
    case MOUSE_CURSOR_TYPE_SIZENS:
        m_hCursor = LoadCursor(nullptr, IDC_SIZENS);
        break;
    case MOUSE_CURSOR_TYPE_SIZEALL:
        m_hCursor = LoadCursor(nullptr, IDC_SIZEALL);
        break;
    case MOUSE_CURSOR_TYPE_NO:
        m_hCursor = LoadCursor(nullptr, IDC_NO);
        break;
    case MOUSE_CURSOR_TYPE_HAND:
        m_hCursor = LoadCursor(nullptr, IDC_HAND);
        break;
    case MOUSE_CURSOR_TYPE_DEFAULT:
    default:
        m_hCursor = LoadCursor(nullptr, IDC_ARROW);
        break;
    }
    if (m_hCursor != nullptr) {
        SetCursor(m_hCursor);
    }
}

ManipulationResult AbstractManipulationHelper::_OnSetCursor( HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam )
{
    UNREFERENCED_PARAMETER(hWnd);
    UNREFERENCED_PARAMETER(message);
    UNREFERENCED_PARAMETER(wParam);
    UNREFERENCED_PARAMETER(lParam);

    SetCursor(m_hCursor);
    ManipulationResult result;
    result.requireInvalidateRect = true;
    result.wasHandled = true;
    return result;
}

touch::ManipulationResult touch::AbstractManipulationHelper::_OnSetFocus( HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam )
{
    UNREFERENCED_PARAMETER(message);
    return OnSetFocus(hWnd, wParam, lParam, GetCurrentTimestampInMilliSeconds());
}

touch::ManipulationResult touch::AbstractManipulationHelper::_OnKillFocus( HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam )
{
    UNREFERENCED_PARAMETER(message);
    return OnKillFocus(hWnd, wParam, lParam, GetCurrentTimestampInMilliSeconds());
}

touch::ManipulationResult touch::AbstractManipulationHelper::_OnChar( HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam )
{
    UNREFERENCED_PARAMETER(message);
    ULONGLONG currentTimestampInMilliSeconds = GetCurrentTimestampInMilliSeconds();
    if (wParam == 8) {
        return OnChar(hWnd, wParam, lParam, CHAR_TYPE_BACKSPACE, static_cast<wchar_t>(wParam), currentTimestampInMilliSeconds);
    } else {
        return OnChar(hWnd, wParam, lParam, CHAR_TYPE_NORMAL, static_cast<wchar_t>(wParam), currentTimestampInMilliSeconds);
    }
}

touch::ManipulationResult touch::AbstractManipulationHelper::_OnKeyDown( HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam )
{
    UNREFERENCED_PARAMETER(message);
    ULONGLONG currentTimestampInMilliSeconds = GetCurrentTimestampInMilliSeconds();
    switch (wParam) {
    case VK_UP:
        return OnChar(hWnd, wParam, lParam, CHAR_TYPE_CURSOR_UP, static_cast<wchar_t>(wParam), currentTimestampInMilliSeconds);
    case VK_DOWN:
        return OnChar(hWnd, wParam, lParam, CHAR_TYPE_CURSOR_DOWN, static_cast<wchar_t>(wParam), currentTimestampInMilliSeconds);
    case VK_RIGHT:
        return OnChar(hWnd, wParam, lParam, CHAR_TYPE_CURSOR_RIGHT, static_cast<wchar_t>(wParam), currentTimestampInMilliSeconds);
    case VK_LEFT:
        return OnChar(hWnd, wParam, lParam, CHAR_TYPE_CURSOR_LEFT, static_cast<wchar_t>(wParam), currentTimestampInMilliSeconds);
    case VK_DELETE:
        return OnChar(hWnd, wParam, lParam, CHAR_TYPE_DELETE, static_cast<wchar_t>(wParam), currentTimestampInMilliSeconds);
    }
    return ManipulationResult(false, false);
}

touch::ManipulationResult touch::AbstractManipulationHelper::WndProc( HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam )
{
    ManipulationResult manipulationResult(false, false);
    switch (message) {
    case WM_CHAR:
        manipulationResult = _OnChar(hWnd, message, wParam, lParam);
        break;
    case WM_KEYDOWN:
        manipulationResult = _OnKeyDown(hWnd, message, wParam, lParam);
        break;
    case WM_ACTIVATE:
        if (WA_INACTIVE == LOWORD(wParam)) {
            // do nothing
            break;
        }
        // do SetFocus
    case WM_SETFOCUS:
        manipulationResult = _OnSetFocus(hWnd, message, wParam, lParam);
        break;
    case WM_KILLFOCUS:
        manipulationResult = _OnKillFocus(hWnd, message, wParam, lParam);
        break;
    case WM_SETCURSOR:
        manipulationResult = _OnSetCursor(hWnd, message, wParam, lParam);
        break;

    case WM_GESTURE:
        manipulationResult = _OnGesture(hWnd, message, wParam, lParam);
        break;

    case WM_MOUSEMOVE:
        manipulationResult = _OnMouseMove(hWnd, message, wParam, lParam);
        break;

    case WM_RBUTTONDOWN:
        manipulationResult = _OnRightMouseDown(hWnd, message, wParam, lParam);
        break;

    case WM_LBUTTONDOWN:
        manipulationResult = _OnLeftMouseDown(hWnd, message, wParam, lParam);
        break;

    case WM_LBUTTONUP:
        manipulationResult = _OnLeftMouseUp(hWnd, message, wParam, lParam);
        break;

    case WM_LBUTTONDBLCLK:
        manipulationResult = _OnLeftMouseDoubleClick(hWnd, message, wParam, lParam);
        break;

    case WM_TIMER:
        manipulationResult = _OnTimer(hWnd, message, wParam, lParam);
        break;
    }
    return manipulationResult;
}

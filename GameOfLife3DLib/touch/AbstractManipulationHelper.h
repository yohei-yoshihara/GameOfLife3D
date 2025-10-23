#ifndef TOUCH_ABSTRACTMANIPULATIONHELPER_H_
#define TOUCH_ABSTRACTMANIPULATIONHELPER_H_

namespace touch {

  enum DRAGGING_STATUS { DRAGGING_STATUS_NOT_DRAGGING, DRAGGING_STATUS_PREPARE, DRAGGING_STATUS_DRAGGING };

  enum MOUSE_CURSOR_TYPE {
    MOUSE_CURSOR_TYPE_DEFAULT,
    MOUSE_CURSOR_TYPE_ARROW,
    MOUSE_CURSOR_TYPE_SIZENWSE,
    MOUSE_CURSOR_TYPE_SIZENESW,
    MOUSE_CURSOR_TYPE_SIZEWE,
    MOUSE_CURSOR_TYPE_SIZENS,
    MOUSE_CURSOR_TYPE_SIZEALL,
    MOUSE_CURSOR_TYPE_NO,
    MOUSE_CURSOR_TYPE_HAND
  };

  enum CHAR_TYPE {
    CHAR_TYPE_NORMAL,
    CHAR_TYPE_CURSOR_UP,
    CHAR_TYPE_CURSOR_DOWN,
    CHAR_TYPE_CURSOR_LEFT,
    CHAR_TYPE_CURSOR_RIGHT,
    CHAR_TYPE_BACKSPACE,
    CHAR_TYPE_DELETE,
  };

  class ManipulationResult {
  public:
    ManipulationResult()
        : wasHandled(false)
        , requireInvalidateRect(false) {
    }
    ManipulationResult(bool _wasHandled, bool _requireInvalidateRect)
        : wasHandled(_wasHandled)
        , requireInvalidateRect(_requireInvalidateRect) {
    }
    bool wasHandled;
    bool requireInvalidateRect;
  };

  class AbstractManipulationHelper {
  private:
    static UINT_PTR m_masterTimerId;
    POINT m_lastPoint;
    DRAGGING_STATUS m_draggingStatus;
    RECT m_dragRect;
    POINT m_lBtnUpPoint;
    UINT_PTR m_timerIdForDoubleClick;
    DWORD m_gestureArgument;
    HCURSOR m_hCursor;
    HCURSOR m_hCursor_Arrow;
    HCURSOR m_hCursor_SizeNWSE;
    HCURSOR m_hCursor_SizeNESW;
    HCURSOR m_hCursor_SizeWE;
    HCURSOR m_hCursor_SizeNS;
    HCURSOR m_hCursor_SizeALL;
    HCURSOR m_hCursor_No;
    HCURSOR m_hCursor_Hand;

  protected:
    virtual ManipulationResult _OnLeftMouseDown(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
    virtual ManipulationResult _OnMouseMove(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
    virtual ManipulationResult _OnLeftMouseUp(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
    virtual ManipulationResult _OnLeftMouseDoubleClick(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
    virtual ManipulationResult _OnRightMouseDown(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
    virtual ManipulationResult _OnTimer(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
    virtual ManipulationResult _OnTouch(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
    virtual ManipulationResult _OnGesture(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
    virtual ManipulationResult _OnSetCursor(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
    virtual ManipulationResult _OnSetFocus(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
    virtual ManipulationResult _OnKillFocus(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
    virtual ManipulationResult _OnChar(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
    virtual ManipulationResult _OnKeyDown(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

  public:
    AbstractManipulationHelper(void);
    virtual ~AbstractManipulationHelper(void);

    void SetMouseCursor(MOUSE_CURSOR_TYPE mouseCursorType);

    virtual ULONGLONG GetCurrentTimestampInMilliSeconds();

    virtual ManipulationResult WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

    virtual bool IsDragging();

    virtual ManipulationResult OnLeftMouseDownExt(HWND hWnd, WPARAM wParam, LPARAM lParam, POINT windowPoint,
                                                  ULONGLONG timestampInMilliSeconds)
        = 0;
    virtual ManipulationResult OnLeftMouseUpExt(HWND hWnd, WPARAM wParam, LPARAM lParam, POINT windowPoint,
                                                ULONGLONG timestampInMilliSeconds)
        = 0;
    virtual ManipulationResult OnSingleClick(HWND hWnd, WPARAM wParam, LPARAM lParam, POINT windowPoint,
                                             ULONGLONG timestampInMilliSeconds)
        = 0;
    virtual ManipulationResult OnDoubleClick(HWND hWnd, WPARAM wParam, LPARAM lParam, POINT windowPoint,
                                             ULONGLONG timestampInMilliSeconds)
        = 0;
    virtual ManipulationResult OnDraggingStart(HWND hWnd, WPARAM wParam, LPARAM lParam, RECT windowDragRect, LONG dx,
                                               LONG dy, ULONGLONG timestampInMilliSeconds)
        = 0;
    virtual ManipulationResult OnDragging(HWND hWnd, WPARAM wParam, LPARAM lParam, RECT windowDragRect, LONG dx,
                                          LONG dy, ULONGLONG timestampInMilliSeconds)
        = 0;
    virtual ManipulationResult OnDraggingEnd(HWND hWnd, WPARAM wParam, LPARAM lParam, RECT windowDragRect, LONG dx,
                                             LONG dy, ULONGLONG timestampInMilliSeconds)
        = 0;
    virtual ManipulationResult OnMouseOver(HWND hWnd, WPARAM wParam, LPARAM lParam, POINT windowPoint, LONG dx, LONG dy,
                                           ULONGLONG timestampInMilliSeconds)
        = 0;
    virtual ManipulationResult OnMouseOut(HWND hWnd, WPARAM wParam, LPARAM lParam, POINT windowPoint, LONG dx, LONG dy,
                                          ULONGLONG timestampInMilliSeconds)
        = 0;
    virtual ManipulationResult OnRightSingleClick(HWND hWnd, WPARAM wParam, LPARAM lParam, POINT windowPoint,
                                                  ULONGLONG timestampInMilliSeconds)
        = 0;
    virtual ManipulationResult OnGestureBegin(HWND hWnd, WPARAM wParam, LPARAM lParam, POINT windowPoint,
                                              ULONGLONG timestampInMilliSeconds, PGESTUREINFO pGestureInfo)
        = 0;
    virtual ManipulationResult OnGestureEnd(HWND hWnd, WPARAM wParam, LPARAM lParam, POINT windowPoint,
                                            ULONGLONG timestampInMilliSeconds, PGESTUREINFO pGestureInfo)
        = 0;
    virtual ManipulationResult OnZoom(HWND hWnd, WPARAM wParam, LPARAM lParam, POINT windowPoint,
                                      ULONGLONG timestampInMilliSeconds, PGESTUREINFO pGestureInfo,
                                      POINT zoomCenterPoint, FLOAT zoomFactor)
        = 0;
    virtual ManipulationResult OnPan(HWND hWnd, WPARAM wParam, LPARAM lParam, POINT windowPoint,
                                     ULONGLONG timestampInMilliSeconds, PGESTUREINFO pGestureInfo, LONG dx, LONG dy)
        = 0;
    virtual ManipulationResult OnRotate(HWND hWnd, WPARAM wParam, LPARAM lParam, POINT windowPoint,
                                        ULONGLONG timestampInMilliSeconds, PGESTUREINFO pGestureInfo,
                                        POINT rotateCenterPoint, FLOAT rotateAngle)
        = 0;
    virtual ManipulationResult OnTwoFingerTap(HWND hWnd, WPARAM wParam, LPARAM lParam, POINT windowPoint,
                                              ULONGLONG timestampInMilliSeconds, PGESTUREINFO pGestureInfo)
        = 0;
    virtual ManipulationResult OnPressAndTap(HWND hWnd, WPARAM wParam, LPARAM lParam, POINT windowPoint,
                                             ULONGLONG timestampInMilliSeconds, PGESTUREINFO pGestureInfo)
        = 0;
    virtual ManipulationResult OnSetFocus(HWND hWnd, WPARAM wParam, LPARAM lParam, ULONGLONG timestampInMilliSeconds)
        = 0;
    virtual ManipulationResult OnKillFocus(HWND hWnd, WPARAM wParam, LPARAM lParam, ULONGLONG timestampInMilliSeconds)
        = 0;
    virtual ManipulationResult OnChar(HWND hWnd, WPARAM wParam, LPARAM lParam, CHAR_TYPE charType, wchar_t c,
                                      ULONGLONG timestampInMilliSeconds)
        = 0;
  };

  inline std::wostream &operator<<(std::wostream &strm, const GESTUREINFO &gi) {
    wchar_t *s_id = nullptr;
    switch (gi.dwID) {
    case GID_BEGIN:
      s_id = L"GID_BEGIN";
      break;
    case GID_END:
      s_id = L"GID_END";
      break;
    case GID_ZOOM:
      s_id = L"GID_ZOOM";
      break;
    case GID_PAN:
      s_id = L"GID_PAN";
      break;
    case GID_ROTATE:
      s_id = L"GID_ROTATE";
      break;
    case GID_TWOFINGERTAP:
      s_id = L"GID_TWOFINGERTAP";
      break;
    case GID_PRESSANDTAP:
      s_id = L"GID_PRESSANDTAP";
      break;
    default:
      wchar_t buf[1024];
      wsprintf(buf, L"UNKNOWN(%d)", gi.dwID);
    }

    std::wstring s_flags;
    if ((gi.dwFlags & GF_BEGIN) != 0) {
      s_flags += L"GF_BEGIN ";
    }
    if ((gi.dwFlags & GF_INERTIA) != 0) {
      s_flags += L"GF_INERTIA ";
    }
    if ((gi.dwFlags & GF_END) != 0) {
      s_flags += L"GF_END ";
    }

    strm << L"GESTUREINFO(ID=" << s_id << L",Flags=" << gi.dwFlags << L"[" << s_flags << L"]" << L",Location=("
         << gi.ptsLocation.x << L"," << gi.ptsLocation.y << L"))";
    return strm;
  }

  inline std::wostream &operator<<(std::wostream &strm, const GESTURECONFIG &gi) {
    wchar_t *s_id = nullptr;
    switch (gi.dwID) {
    case GID_BEGIN:
      s_id = L"GID_BEGIN";
      break;
    case GID_END:
      s_id = L"GID_END";
      break;
    case GID_ZOOM:
      s_id = L"GID_ZOOM";
      break;
    case GID_PAN:
      s_id = L"GID_PAN";
      break;
    case GID_ROTATE:
      s_id = L"GID_ROTATE";
      break;
    case GID_TWOFINGERTAP:
      s_id = L"GID_TWOFINGERTAP";
      break;
    case GID_PRESSANDTAP:
      s_id = L"GID_PRESSANDTAP";
      break;
    default:
      wchar_t buf[1024];
      wsprintf(buf, L"UNKNOWN(%d)", gi.dwID);
    }
    strm << L"GESTURECONFIG(ID=" << s_id << L",Want=" << gi.dwWant << L",Block=" << gi.dwBlock;
    return strm;
  }
}

#endif // TOUCH_ABSTRACTMANIPULATIONHELPER_H_
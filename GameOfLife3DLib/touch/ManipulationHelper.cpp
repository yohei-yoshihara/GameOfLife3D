#include "Stdafx.h"
#include "Common.h"
#include "touch/ManipulationHelper.h"
#include "ui/UIContainer.h"

//#define DEBUG_EVENT

using namespace touch;
using namespace ui;

touch::ManipulationHelper::ManipulationHelper()
    : // m_canvasPanel(),
    m_manipulationStatus(MANIPULATION_STATUS_NORMAL)
    , m_rectBeforeResize(D2D1::RectF())
    , m_rectOnResize(D2D1::RectF())
    , m_hitTestMargin(5.0f) {
}

touch::ManipulationHelper::~ManipulationHelper() {
}

// void touch::ManipulationHelper::SetCanvasPanel(nxsftp::CanvasPanelWeakPtr canvasPanel)
// {
//     m_canvasPanel = canvasPanel;
// }

ManipulationResult touch::ManipulationHelper::OnLeftMouseDownExt(HWND hWnd, WPARAM wParam, LPARAM lParam,
                                                                 POINT windowPoint, ULONGLONG timestampInMilliSeconds) {
#ifdef DEBUG_EVENT
  ATLTRACE("ManipulationHelper::OnLeftMouseDownExt\n");
#endif
  bool eaten = false;
  if (!m_container.expired()) {
    m_container.lock()->OnLeftMouseDown(hWnd, wParam, lParam, static_cast<UIPoint>(windowPoint),
                                        timestampInMilliSeconds, &eaten);
  }
  return eaten ? ManipulationResult(true, true) : ManipulationResult(false, false);
}

ManipulationResult touch::ManipulationHelper::OnLeftMouseUpExt(HWND hWnd, WPARAM wParam, LPARAM lParam,
                                                               POINT windowPoint, ULONGLONG timestampInMilliSeconds) {
#ifdef DEBUG_EVENT
  ATLTRACE("ManipulationHelper::OnLeftMouseUpExt\n");
#endif
  bool eaten = false;
  if (!m_container.expired()) {
    m_container.lock()->OnLeftMouseUp(hWnd, wParam, lParam, static_cast<UIPoint>(windowPoint), timestampInMilliSeconds,
                                      &eaten);
  }
  return eaten ? ManipulationResult(true, true) : ManipulationResult(false, false);
}

ManipulationResult touch::ManipulationHelper::OnSingleClick(HWND hWnd, WPARAM wParam, LPARAM lParam, POINT windowPoint,
                                                            ULONGLONG timestampInMilliSeconds) {
#ifdef DEBUG_EVENT
  ATLTRACE("ManipulationHelper::OnSingleClick\n");
#endif
  bool eaten = false;
  if (!m_container.expired()) {
    m_container.lock()->OnSingleClick(hWnd, wParam, lParam, static_cast<UIPoint>(windowPoint), timestampInMilliSeconds,
                                      &eaten);
  }
  return eaten ? ManipulationResult(true, true) : ManipulationResult(false, false);
}

ManipulationResult touch::ManipulationHelper::OnDoubleClick(HWND hWnd, WPARAM wParam, LPARAM lParam, POINT windowPoint,
                                                            ULONGLONG timestampInMilliSeconds) {
#ifdef DEBUG_EVENT
  ATLTRACE("ManipulationHelper::OnDoubleClick\n");
#endif
  bool eaten = false;
  if (!m_container.expired()) {
    m_container.lock()->OnDoubleClick(hWnd, wParam, lParam, static_cast<UIPoint>(windowPoint), timestampInMilliSeconds,
                                      &eaten);
  }
  return eaten ? ManipulationResult(true, true) : ManipulationResult(false, false);
}

ManipulationResult touch::ManipulationHelper::OnDraggingStart(HWND hWnd, WPARAM wParam, LPARAM lParam,
                                                              RECT windowDragRect, LONG dx, LONG dy,
                                                              ULONGLONG timestampInMilliSeconds) {
#ifdef DEBUG_EVENT
  ATLTRACE("ManipulationHelper::OnDraggingStart\n");
#endif
  bool eaten = false;
  if (!m_container.expired()) {
    m_container.lock()->OnDraggingStart(hWnd, wParam, lParam, static_cast<UI2Points>(windowDragRect),
                                        UIDelta(static_cast<FLOAT>(dx), static_cast<FLOAT>(dy)),
                                        timestampInMilliSeconds, &eaten);
  }
  return eaten ? ManipulationResult(true, true) : ManipulationResult(false, false);
}

ManipulationResult touch::ManipulationHelper::OnDragging(HWND hWnd, WPARAM wParam, LPARAM lParam, RECT windowDragRect,
                                                         LONG dx, LONG dy, ULONGLONG timestampInMilliSeconds) {
#ifdef DEBUG_EVENT
  ATLTRACE("ManipulationHelper::OnDragging\n");
#endif
  bool eaten = false;
  if (!m_container.expired()) {
    m_container.lock()->OnDragging(hWnd, wParam, lParam, static_cast<UI2Points>(windowDragRect),
                                   UIDelta(static_cast<FLOAT>(dx), static_cast<FLOAT>(dy)), timestampInMilliSeconds,
                                   &eaten);
  }
  return eaten ? ManipulationResult(true, true) : ManipulationResult(false, false);
}

ManipulationResult touch::ManipulationHelper::OnDraggingEnd(HWND hWnd, WPARAM wParam, LPARAM lParam,
                                                            RECT windowDragRect, LONG dx, LONG dy,
                                                            ULONGLONG timestampInMilliSeconds) {
#ifdef DEBUG_EVENT
  ATLTRACE("ManipulationHelper::OnDraggingEnd\n");
#endif
  bool eaten = false;
  if (!m_container.expired()) {
    m_container.lock()->OnDraggingEnd(hWnd, wParam, lParam, static_cast<UI2Points>(windowDragRect),
                                      UIDelta(static_cast<FLOAT>(dx), static_cast<FLOAT>(dy)), timestampInMilliSeconds,
                                      &eaten);
  }
  return eaten ? ManipulationResult(true, true) : ManipulationResult(false, false);
}

ManipulationResult touch::ManipulationHelper::OnMouseOver(HWND hWnd, WPARAM wParam, LPARAM lParam, POINT windowPoint,
                                                          LONG dx, LONG dy, ULONGLONG timestampInMilliSeconds) {
#ifdef DEBUG_EVENT
  ATLTRACE(".");
// ATLTRACE("ManipulationHelper::OnMouseOver\n");
#endif
  SetMouseCursor(MOUSE_CURSOR_TYPE_DEFAULT);
  bool eaten = false;
  if (!m_container.expired()) {
    m_container.lock()->OnMouseOver(hWnd, wParam, lParam, static_cast<UIPoint>(windowPoint),
                                    UIDelta(static_cast<FLOAT>(dx), static_cast<FLOAT>(dy)), timestampInMilliSeconds,
                                    &eaten);
  }
  return eaten ? ManipulationResult(true, true) : ManipulationResult(false, false);
}

ManipulationResult touch::ManipulationHelper::OnMouseOut(HWND hWnd, WPARAM wParam, LPARAM lParam, POINT windowPoint,
                                                         LONG dx, LONG dy, ULONGLONG timestampInMilliSeconds) {
#ifdef DEBUG_EVENT
  ATLTRACE("ManipulationHelper::OnMouseOut\n");
#endif
  bool eaten = false;
  if (!m_container.expired()) {
    m_container.lock()->OnMouseOut(hWnd, wParam, lParam, static_cast<UIPoint>(windowPoint),
                                   UIDelta(static_cast<FLOAT>(dx), static_cast<FLOAT>(dy)), timestampInMilliSeconds,
                                   &eaten);
  }
  return eaten ? ManipulationResult(true, true) : ManipulationResult(false, false);
}

ManipulationResult touch::ManipulationHelper::OnRightSingleClick(HWND hWnd, WPARAM wParam, LPARAM lParam,
                                                                 POINT windowPoint, ULONGLONG timestampInMilliSeconds) {
#ifdef DEBUG_EVENT
  ATLTRACE("ManipulationHelper::OnRightSingleClick\n");
#endif
  bool eaten = false;
  if (!m_container.expired()) {
    m_container.lock()->OnRightSingleClick(hWnd, wParam, lParam, static_cast<UIPoint>(windowPoint),
                                           timestampInMilliSeconds, &eaten);
  }
  return eaten ? ManipulationResult(true, true) : ManipulationResult(false, false);
}

ManipulationResult touch::ManipulationHelper::OnGestureBegin(HWND hWnd, WPARAM wParam, LPARAM lParam, POINT windowPoint,
                                                             ULONGLONG timestampInMilliSeconds,
                                                             PGESTUREINFO pGestureInfo) {
#ifdef DEBUG_EVENT
  ATLTRACE("ManipulationHelper::OnGestureBegin\n");
#endif
  bool eaten = false;
  if (!m_container.expired()) {
    m_container.lock()->OnGestureBegin(hWnd, wParam, lParam, static_cast<UIPoint>(windowPoint), pGestureInfo,
                                       timestampInMilliSeconds, &eaten);
  }
  return eaten ? ManipulationResult(true, true) : ManipulationResult(false, false);
}

ManipulationResult touch::ManipulationHelper::OnGestureEnd(HWND hWnd, WPARAM wParam, LPARAM lParam, POINT windowPoint,
                                                           ULONGLONG timestampInMilliSeconds,
                                                           PGESTUREINFO pGestureInfo) {
#ifdef DEBUG_EVENT
  ATLTRACE("ManipulationHelper::OnGestureEnd\n");
#endif
  bool eaten = false;
  if (!m_container.expired()) {
    m_container.lock()->OnGestureEnd(hWnd, wParam, lParam, static_cast<UIPoint>(windowPoint), pGestureInfo,
                                     timestampInMilliSeconds, &eaten);
  }
  return eaten ? ManipulationResult(true, true) : ManipulationResult(false, false);
}

ManipulationResult touch::ManipulationHelper::OnZoom(HWND hWnd, WPARAM wParam, LPARAM lParam, POINT windowPoint,
                                                     ULONGLONG timestampInMilliSeconds, PGESTUREINFO pGestureInfo,
                                                     POINT zoomCenterPoint, FLOAT zoomFactor) {
#ifdef DEBUG_EVENT
  ATLTRACE("ManipulationHelper::OnZoom\n");
#endif
  bool eaten = false;
  if (!m_container.expired()) {
    m_container.lock()->OnZoom(hWnd, wParam, lParam, static_cast<UIPoint>(windowPoint), pGestureInfo,
                               UIPoint(zoomCenterPoint), zoomFactor, timestampInMilliSeconds, &eaten);
  }
  return eaten ? ManipulationResult(true, true) : ManipulationResult(false, false);
}

ManipulationResult touch::ManipulationHelper::OnPan(HWND hWnd, WPARAM wParam, LPARAM lParam, POINT windowPoint,
                                                    ULONGLONG timestampInMilliSeconds, PGESTUREINFO pGestureInfo,
                                                    LONG dx, LONG dy) {
#ifdef DEBUG_EVENT
  ATLTRACE("ManipulationHelper::OnPan\n");
#endif
  bool eaten = false;
  if (!m_container.expired()) {
    m_container.lock()->OnPan(hWnd, wParam, lParam, static_cast<UIPoint>(windowPoint), pGestureInfo,
                              UIDelta(static_cast<FLOAT>(dx), static_cast<FLOAT>(dy)), timestampInMilliSeconds, &eaten);
  }
  return eaten ? ManipulationResult(true, true) : ManipulationResult(false, false);
}

ManipulationResult touch::ManipulationHelper::OnRotate(HWND hWnd, WPARAM wParam, LPARAM lParam, POINT windowPoint,
                                                       ULONGLONG timestampInMilliSeconds, PGESTUREINFO pGestureInfo,
                                                       POINT rotateCenterPoint, FLOAT rotateAngle) {
#ifdef DEBUG_EVENT
  ATLTRACE("ManipulationHelper::OnRotate\n");
#endif
  bool eaten = false;
  if (!m_container.expired()) {
    m_container.lock()->OnRotate(hWnd, wParam, lParam, static_cast<UIPoint>(windowPoint), pGestureInfo,
                                 UIPoint(rotateCenterPoint), rotateAngle, timestampInMilliSeconds, &eaten);
  }
  return eaten ? ManipulationResult(true, true) : ManipulationResult(false, false);
}

ManipulationResult touch::ManipulationHelper::OnTwoFingerTap(HWND hWnd, WPARAM wParam, LPARAM lParam, POINT windowPoint,
                                                             ULONGLONG timestampInMilliSeconds,
                                                             PGESTUREINFO pGestureInfo) {
#ifdef DEBUG_EVENT
  ATLTRACE("ManipulationHelper::OnTwoFingerTap\n");
#endif
  bool eaten = false;
  if (!m_container.expired()) {
    m_container.lock()->OnTwoFingerTap(hWnd, wParam, lParam, static_cast<UIPoint>(windowPoint), pGestureInfo,
                                       timestampInMilliSeconds, &eaten);
  }
  return eaten ? ManipulationResult(true, true) : ManipulationResult(false, false);
}

ManipulationResult touch::ManipulationHelper::OnPressAndTap(HWND hWnd, WPARAM wParam, LPARAM lParam, POINT windowPoint,
                                                            ULONGLONG timestampInMilliSeconds,
                                                            PGESTUREINFO pGestureInfo) {
#ifdef DEBUG_EVENT
  ATLTRACE("ManipulationHelper::OnPressAndTap\n");
#endif
  bool eaten = false;
  if (!m_container.expired()) {
    m_container.lock()->OnPressAndTap(hWnd, wParam, lParam, static_cast<UIPoint>(windowPoint), pGestureInfo,
                                      timestampInMilliSeconds, &eaten);
  }
  return eaten ? ManipulationResult(true, true) : ManipulationResult(false, false);
}

ui::CHAR_TYPE Convert(touch::CHAR_TYPE charType) {
  switch (charType) {
  case touch::CHAR_TYPE_NORMAL:
    return ui::CHAR_TYPE_NORMAL;
  case touch::CHAR_TYPE_BACKSPACE:
    return ui::CHAR_TYPE_BACKSPACE;
  case touch::CHAR_TYPE_DELETE:
    return ui::CHAR_TYPE_DELETE;
  case touch::CHAR_TYPE_CURSOR_UP:
    return ui::CHAR_TYPE_CURSOR_UP;
  case touch::CHAR_TYPE_CURSOR_DOWN:
    return ui::CHAR_TYPE_CURSOR_DOWN;
  case touch::CHAR_TYPE_CURSOR_LEFT:
    return ui::CHAR_TYPE_CURSOR_LEFT;
  case touch::CHAR_TYPE_CURSOR_RIGHT:
    return ui::CHAR_TYPE_CURSOR_RIGHT;
  }
  return ui::CHAR_TYPE_NORMAL;
}

touch::ManipulationResult touch::ManipulationHelper::OnChar(HWND hWnd, WPARAM wParam, LPARAM lParam, CHAR_TYPE charType,
                                                            wchar_t c, ULONGLONG timestampInMilliSeconds) {
#ifdef DEBUG_EVENT
  ATLTRACE("ManipulationHelper::OnChar\n");
#endif
  bool eaten = false;
  if (!m_container.expired()) {
    m_container.lock()->OnChar(hWnd, wParam, lParam, Convert(charType), c, timestampInMilliSeconds, &eaten);
  }
  return eaten ? ManipulationResult(true, true) : ManipulationResult(false, false);
}

touch::ManipulationResult touch::ManipulationHelper::OnSetFocus(HWND hWnd, WPARAM wParam, LPARAM lParam,
                                                                ULONGLONG timestampInMilliSeconds) {
#ifdef DEBUG_EVENT
  ATLTRACE("ManipulationHelper::OnSetFocus\n");
#endif
  bool eaten = false;
  if (!m_container.expired()) {
    m_container.lock()->OnSetFocus(hWnd, wParam, lParam, timestampInMilliSeconds, &eaten);
  }
  return eaten ? ManipulationResult(true, true) : ManipulationResult(false, false);
}

touch::ManipulationResult touch::ManipulationHelper::OnKillFocus(HWND hWnd, WPARAM wParam, LPARAM lParam,
                                                                 ULONGLONG timestampInMilliSeconds) {
#ifdef DEBUG_EVENT
  ATLTRACE("ManipulationHelper::OnKillFocus\n");
#endif
  bool eaten = false;
  if (!m_container.expired()) {
    m_container.lock()->OnKillFocus(hWnd, wParam, lParam, timestampInMilliSeconds, &eaten);
  }
  return eaten ? ManipulationResult(true, true) : ManipulationResult(false, false);
}

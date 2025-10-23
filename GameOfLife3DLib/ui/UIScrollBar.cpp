#include "stdafx.h"
#include "Common.h"
#include "graphics/GraphicsCommon.h"
#include "graphics/brush/SolidColorBrush.h"
#include "graphics/brush/LinearGradientBrush.h"
#include "graphics/color/SolidColor.h"
#include "graphics/figure/RectangleFigure.h"
#include "graphics/figure/EllipseFigure.h"
#include "graphics/figure/TriangleFigure.h"
#include "ui/UIScrollBar.h"

// #define DEBUG_UISCROLLBAR

ui::UIScrollBar::UIScrollBar()
    : m_direction(SCROLLBAR_DIRECTION_HORIZONTAL)
    , m_minValue(0.0f)
    , m_maxValue(100.0f)
    , m_pageSize(10.0f)
    , m_currentValue(0.0f)
    , m_firstInitialization(true)
    , m_barPositionChanged(false)
    , m_propertyChanged(false)
    , m_dragging(false)
    , m_body(std::make_unique<graphics::figure::RectangleFigure>())
    , m_arrow1(std::make_unique<graphics::figure::TriangleFigure>())
    , m_arrow2(std::make_unique<graphics::figure::TriangleFigure>())
    , m_slideArea(std::make_unique<graphics::figure::RectangleFigure>())
    , m_bar(std::make_unique<graphics::figure::RectangleFigure>())
    , m_positionChangedCallbacks() {
#ifdef DEBUG_UISCROLLBAR
  LOG_ENTER(SEVERITY_LEVEL_DEBUG);
  LOG_LEAVE(SEVERITY_LEVEL_DEBUG);
#endif
}

ui::UIScrollBar::~UIScrollBar() {
#ifdef DEBUG_UISCROLLBAR
  LOG_ENTER(SEVERITY_LEVEL_DEBUG);
  LOG_LEAVE(SEVERITY_LEVEL_DEBUG);
#endif
}

HRESULT ui::UIScrollBar::Initialize(graphics::D3DInteropHelper *pD3DInteropHelper) {
#ifdef DEBUG_UISCROLLBAR
  LOG_ENTER(SEVERITY_LEVEL_DEBUG);
  LOG(SEVERITY_LEVEL_DEBUG) << L"GetTotalWidth() = " << GetTotalThick();
  LOG(SEVERITY_LEVEL_DEBUG) << L"GetTotalLength() = " << GetTotalLength();
  LOG(SEVERITY_LEVEL_DEBUG) << L"GetSlideAreaWidth() = " << GetSlideAreaThick();
  LOG(SEVERITY_LEVEL_DEBUG) << L"GetSlideAreaLength() = " << GetSlideAreaLength();
  LOG(SEVERITY_LEVEL_DEBUG) << L"GetSlideAreaMargin() = " << GetSlideAreaMargin();
  LOG(SEVERITY_LEVEL_DEBUG) << L"GetBarPosition() = " << GetBarPosition();
  LOG(SEVERITY_LEVEL_DEBUG) << L"GetBarLength() = " << GetBarLength();
  LOG(SEVERITY_LEVEL_DEBUG) << L"GetBarWidth() = " << GetBarThick();
  LOG(SEVERITY_LEVEL_DEBUG) << L"GetBarOffset() = " << GetBarOffset();
  LOG(SEVERITY_LEVEL_DEBUG) << L"GetArrowLength() = " << GetArrowLength();
  LOG(SEVERITY_LEVEL_DEBUG) << L"GetArrowWidth() = " << GetArrowThick();
  LOG(SEVERITY_LEVEL_DEBUG) << L"GetArrowMargin() = " << GetArrowMargin();
  LOG(SEVERITY_LEVEL_DEBUG) << L"GetArrow1Offset() = " << GetArrow1Offset();
  LOG(SEVERITY_LEVEL_DEBUG) << L"GetArrow2Offset() = " << GetArrow2Offset();
  LOG(SEVERITY_LEVEL_DEBUG) << L"GetSplitterMargin() = " << GetSplitterMargin();
  LOG(SEVERITY_LEVEL_DEBUG) << L"GetSplitterWidth() = " << GetSplitterThick();
  LOG(SEVERITY_LEVEL_DEBUG) << L"GetSplitterHeight() = " << GetSplitterHeight();
  LOG(SEVERITY_LEVEL_DEBUG) << L"GetSplitterOffset() = " << GetSplitterOffset();
#endif
  m_body->SetX(0.0f);
  m_body->SetY(0.0f);
  m_body->SetWidth(GetWidth());
  m_body->SetHeight(GetHeight());
  m_body->SetRadius(m_direction == SCROLLBAR_DIRECTION_HORIZONTAL ? GetHeight() / 2.0f : GetWidth() / 2.0f);
  m_body->SetColor(graphics::color::ColorValue(graphics::color::ColorValue::COLOR_TYPE_HSBA, 0.0f, 0.0f, 0.2f, 1.0f),
                   graphics::color::COLOR_PATTERN_CONVEX);
  m_body->SetShadow(true);
  m_body->SetShadowColor(std::make_shared<graphics::color::SolidColor>(
      graphics::color::ColorValue(graphics::color::ColorValue::COLOR_TYPE_HSBA, 190.0f, 1.0f, 1.0f, 1.0f)));
  m_body->Initialize(pD3DInteropHelper);

  if (m_direction == SCROLLBAR_DIRECTION_HORIZONTAL) {
    m_arrow1->SetX(GetArrow1Offset());
    m_arrow1->SetY(GetArrowMargin());
    m_arrow1->SetWidth(GetArrowLength());
    m_arrow1->SetHeight(GetArrowThick());
    m_arrow1->SetDirection(graphics::figure::TriangleFigure::TRIANGLE_DIRECTION_LEFT);
  } else {
    m_arrow1->SetX(GetArrowMargin());
    m_arrow1->SetY(GetArrow1Offset());
    m_arrow1->SetWidth(GetArrowThick());
    m_arrow1->SetHeight(GetArrowLength());
    m_arrow1->SetDirection(graphics::figure::TriangleFigure::TRIANGLE_DIRECTION_TOP);
  }
  m_arrow1->SetColor(graphics::color::ColorValue(graphics::color::ColorValue::COLOR_TYPE_HSBA, 32.0f, 0.0f, 0.5f, 1.0f),
                     graphics::color::COLOR_PATTERN_CONVEX);
  m_arrow1->Initialize(pD3DInteropHelper);

  if (m_direction == SCROLLBAR_DIRECTION_HORIZONTAL) {
    m_arrow2->SetX(GetArrow2Offset());
    m_arrow2->SetY(GetArrowMargin());
    m_arrow2->SetWidth(GetArrowLength());
    m_arrow2->SetHeight(GetArrowThick());
    m_arrow2->SetDirection(graphics::figure::TriangleFigure::TRIANGLE_DIRECTION_RIGHT);
  } else {
    m_arrow2->SetX(GetArrowMargin());
    m_arrow2->SetY(GetArrow2Offset());
    m_arrow2->SetWidth(GetArrowThick());
    m_arrow2->SetHeight(GetArrowLength());
    m_arrow2->SetDirection(graphics::figure::TriangleFigure::TRIANGLE_DIRECTION_BOTTOM);
  }
  m_arrow2->SetColor(graphics::color::ColorValue(graphics::color::ColorValue::COLOR_TYPE_HSBA, 32.0f, 0.0f, 0.5f, 1.0f),
                     graphics::color::COLOR_PATTERN_CONVEX);
  m_arrow2->Initialize(pD3DInteropHelper);

  if (m_direction == SCROLLBAR_DIRECTION_HORIZONTAL) {
    m_slideArea->SetX(GetSlideAreaOffset());
    m_slideArea->SetY(GetSlideAreaMargin());
    m_slideArea->SetWidth(GetSlideAreaLength());
    m_slideArea->SetHeight(GetSlideAreaThick());
  } else {
    m_slideArea->SetX(GetSlideAreaMargin());
    m_slideArea->SetY(GetSlideAreaOffset());
    m_slideArea->SetWidth(GetSlideAreaThick());
    m_slideArea->SetHeight(GetSlideAreaLength());
  }
  m_slideArea->SetColor(
      graphics::color::ColorValue(graphics::color::ColorValue::COLOR_TYPE_HSBA, 190.0f, 1.0f, 0.3f, 1.0f),
      graphics::color::COLOR_PATTERN_CONVEX);
  m_slideArea->Initialize(pD3DInteropHelper);

  if (m_direction == SCROLLBAR_DIRECTION_HORIZONTAL) {
    m_bar->SetX(GetBarOffset());
    m_bar->SetY(GetSlideAreaMargin());
    m_bar->SetWidth(GetBarLength());
    m_bar->SetHeight(GetBarThick());
  } else {
    m_bar->SetX(GetSlideAreaMargin());
    m_bar->SetY(GetBarOffset());
    m_bar->SetWidth(GetBarThick());
    m_bar->SetHeight(GetBarLength());
  }
  m_bar->SetColor(graphics::color::ColorValue(graphics::color::ColorValue::COLOR_TYPE_HSBA, 190.0f, 1.0f, 0.7f, 1.0f),
                  graphics::color::COLOR_PATTERN_CONVEX_METAL);
  m_bar->SetShadow(true);
  m_bar->SetShadowColor(std::make_shared<graphics::color::SolidColor>(
      graphics::color::ColorValue(graphics::color::ColorValue::COLOR_TYPE_HSBA, 190.0f, 1.0f, 1.0f, 1.0f)));
  m_bar->Initialize(pD3DInteropHelper);

#ifdef DEBUG_UISCROLLBAR
  LOG_LEAVE(SEVERITY_LEVEL_DEBUG);
#endif
  return S_OK;
}

HRESULT ui::UIScrollBar::UpdateBarPosition() {
#ifdef DEBUG_UISCROLLBAR
  LOG_ENTER(SEVERITY_LEVEL_DEBUG);
#endif
  if (m_direction == SCROLLBAR_DIRECTION_HORIZONTAL) {
    m_bar->SetX(GetBarOffset());
    m_bar->SetY(GetSlideAreaMargin());
  } else {
    m_bar->SetX(GetSlideAreaMargin());
    m_bar->SetY(GetBarOffset());
  }
#ifdef DEBUG_UISCROLLBAR
  LOG_LEAVE(SEVERITY_LEVEL_DEBUG);
#endif
  return S_OK;
}

HRESULT ui::UIScrollBar::CreateDeviceDependentResources(graphics::D3DInteropHelper *pD3DInteropHelper,
                                                        ID2D1RenderTarget *pRenderTarget) {
#ifdef DEBUG_UISCROLLBAR
  LOG_ENTER(SEVERITY_LEVEL_DEBUG);
#endif
  if (m_firstInitialization) {
    m_firstInitialization = false;
    m_propertyChanged = false;
  }

  m_body->CreateDeviceDependentResources(pD3DInteropHelper, pRenderTarget);
  m_arrow1->CreateDeviceDependentResources(pD3DInteropHelper, pRenderTarget);
  m_arrow2->CreateDeviceDependentResources(pD3DInteropHelper, pRenderTarget);
  m_slideArea->CreateDeviceDependentResources(pD3DInteropHelper, pRenderTarget);
  m_bar->CreateDeviceDependentResources(pD3DInteropHelper, pRenderTarget);

#ifdef DEBUG_UISCROLLBAR
  LOG_LEAVE(SEVERITY_LEVEL_DEBUG);
#endif
  return S_OK;
}

HRESULT ui::UIScrollBar::Render(graphics::D3DInteropHelper *pD3DInteropHelper, ID2D1RenderTarget *pRenderTarget) {
#ifdef DEBUG_UISCROLLBAR
  LOG_ENTER(SEVERITY_LEVEL_DEBUG) << L"propertyChanged = " << m_propertyChanged;
#endif
  if (m_propertyChanged || IsPropertyChanged()) {
    CHK_FATAL_HRESULT(Initialize(pD3DInteropHelper));
    CHK_FATAL_HRESULT(CreateDeviceDependentResources(pD3DInteropHelper, pRenderTarget));
    m_propertyChanged = false;
    SetPropertyChanged(false);
  }
  if (m_barPositionChanged) {
    UpdateBarPosition();
    m_barPositionChanged = false;
  }

  m_body->Render(pD3DInteropHelper, pRenderTarget, 1.0f);
  m_arrow1->Render(pD3DInteropHelper, pRenderTarget, 1.0f);
  m_arrow2->Render(pD3DInteropHelper, pRenderTarget, 1.0f);
  m_slideArea->Render(pD3DInteropHelper, pRenderTarget, 1.0f);
  m_bar->Render(pD3DInteropHelper, pRenderTarget, 1.0f);

#ifdef DEBUG_UISCROLLBAR
  LOG_LEAVE(SEVERITY_LEVEL_DEBUG);
#endif
  return S_OK;
}

void ui::UIScrollBar::DiscardDeviceDependentResources() {
#ifdef DEBUG_UISCROLLBAR
  LOG_ENTER(SEVERITY_LEVEL_DEBUG);
#endif
  m_body->DiscardDeviceDependentResources();
  m_arrow1->DiscardDeviceDependentResources();
  m_arrow2->DiscardDeviceDependentResources();
  m_slideArea->DiscardDeviceDependentResources();
  m_bar->DiscardDeviceDependentResources();
#ifdef DEBUG_UISCROLLBAR
  LOG_LEAVE(SEVERITY_LEVEL_DEBUG);
#endif
}

void ui::UIScrollBar::OnLeftMouseDown(HWND hWnd, WPARAM wParam, LPARAM lParam, UIPoint clientPoint,
                                      ULONGLONG timestampInMilliSeconds, OUT bool *eaten) {
#ifdef DEBUG_UISCROLLBAR
  LOG_ENTER(SEVERITY_LEVEL_DEBUG);
#endif
  UNREFERENCED_PARAMETER(hWnd);
  UNREFERENCED_PARAMETER(wParam);
  UNREFERENCED_PARAMETER(lParam);
  UNREFERENCED_PARAMETER(timestampInMilliSeconds);
  UNREFERENCED_PARAMETER(eaten);
  if (m_arrow1->HitTest(clientPoint.x, clientPoint.y)) {
    MoveBarPosition(-m_pageSize / 2.0f);
  } else if (m_arrow2->HitTest(clientPoint.x, clientPoint.y)) {
    MoveBarPosition(m_pageSize / 2.0f);
  } else if (m_slideArea->HitTest(clientPoint.x, clientPoint.y)) {
    if (m_direction == SCROLLBAR_DIRECTION_HORIZONTAL) {
      if (clientPoint.x < m_bar->GetX()) {
        MoveBarPosition(-m_pageSize);
      } else if (m_bar->GetX() + m_bar->GetWidth() < clientPoint.x) {
        MoveBarPosition(m_pageSize);
      }
    } else {
      if (clientPoint.y < m_bar->GetY()) {
        MoveBarPosition(-m_pageSize);
      } else if (m_bar->GetY() + m_bar->GetHeight() < clientPoint.y) {
        MoveBarPosition(m_pageSize);
      }
    }
  }
#ifdef DEBUG_UISCROLLBAR
  LOG_LEAVE(SEVERITY_LEVEL_DEBUG);
#endif
}

void ui::UIScrollBar::OnDraggingStart(HWND hWnd, WPARAM wParam, LPARAM lParam, UI2Points clientDragRect, UIDelta delta,
                                      ULONGLONG timestampInMilliSeconds, OUT bool *eaten) {
#ifdef DEBUG_UISCROLLBAR
  LOG_ENTER(SEVERITY_LEVEL_DEBUG);
#endif
  UNREFERENCED_PARAMETER(hWnd);
  UNREFERENCED_PARAMETER(wParam);
  UNREFERENCED_PARAMETER(lParam);
  UNREFERENCED_PARAMETER(delta);
  UNREFERENCED_PARAMETER(timestampInMilliSeconds);
  UNREFERENCED_PARAMETER(eaten);
  if (m_bar->HitTest(clientDragRect.start.x, clientDragRect.start.y)) {
    m_dragging = true;
    m_dragPosInBar.x = clientDragRect.start.x - m_bar->GetX();
    m_dragPosInBar.y = clientDragRect.start.y - m_bar->GetY();
#ifdef DEBUG_UISCROLLBAR
    LOG(SEVERITY_LEVEL_DEBUG) << L"m_dragPosInBar = " << m_dragPosInBar;
#endif
  }
#ifdef DEBUG_UISCROLLBAR
  LOG_LEAVE(SEVERITY_LEVEL_DEBUG);
#endif
}

void ui::UIScrollBar::OnDragging(HWND hWnd, WPARAM wParam, LPARAM lParam, UI2Points clientDragRect, UIDelta delta,
                                 ULONGLONG timestampInMilliSeconds, OUT bool *eaten) {
#ifdef DEBUG_UISCROLLBAR
  LOG_ENTER(SEVERITY_LEVEL_DEBUG);
  LOG(SEVERITY_LEVEL_DEBUG) << L"clientDragRect=" << clientDragRect;
  LOG(SEVERITY_LEVEL_DEBUG) << L"delta=" << delta;
  LOG(SEVERITY_LEVEL_DEBUG) << L"bar[" << m_bar->GetX() << L"," << m_bar->GetY() << L"," << m_bar->GetWidth() << L"x"
                            << m_bar->GetHeight() << L"]";
#endif
  UNREFERENCED_PARAMETER(hWnd);
  UNREFERENCED_PARAMETER(wParam);
  UNREFERENCED_PARAMETER(lParam);
  UNREFERENCED_PARAMETER(clientDragRect);
  UNREFERENCED_PARAMETER(delta);
  UNREFERENCED_PARAMETER(timestampInMilliSeconds);
  UNREFERENCED_PARAMETER(eaten);
  if (m_dragging) {
    if (m_direction == SCROLLBAR_DIRECTION_HORIZONTAL) {
      FLOAT sx = (clientDragRect.end.x - m_dragPosInBar.x) - GetSlideAreaOffset();
      FLOAT x = (m_maxValue - m_minValue) / (GetSlideAreaLength() - GetBarLength()) * sx;
      SetBarPosition(x);
#ifdef DEBUG_UISCROLLBAR
      LOG(SEVERITY_LEVEL_DEBUG) << L"sx = " << sx << L", x = " << x;
#endif
    } else {
      FLOAT sy = (clientDragRect.end.y - m_dragPosInBar.y) - GetSlideAreaOffset();
      FLOAT y = (m_maxValue - m_minValue) / (GetSlideAreaLength() - GetBarLength()) * sy;
      SetBarPosition(y);
#ifdef DEBUG_UISCROLLBAR
      LOG(SEVERITY_LEVEL_DEBUG) << L"sy = " << sy << L", y = " << y;
#endif
    }
  }
#ifdef DEBUG_UISCROLLBAR
  LOG_LEAVE(SEVERITY_LEVEL_DEBUG);
#endif
}

void ui::UIScrollBar::OnDraggingEnd(HWND hWnd, WPARAM wParam, LPARAM lParam, UI2Points clientDragRect, UIDelta delta,
                                    ULONGLONG timestampInMilliSeconds, OUT bool *eaten) {
#ifdef DEBUG_UISCROLLBAR
  LOG_ENTER(SEVERITY_LEVEL_DEBUG);
#endif
  UNREFERENCED_PARAMETER(hWnd);
  UNREFERENCED_PARAMETER(wParam);
  UNREFERENCED_PARAMETER(lParam);
  UNREFERENCED_PARAMETER(delta);
  UNREFERENCED_PARAMETER(timestampInMilliSeconds);
  UNREFERENCED_PARAMETER(eaten);
  if (m_dragging && m_bar->HitTest(clientDragRect.end.x, clientDragRect.end.y)) {
    if (m_direction == SCROLLBAR_DIRECTION_HORIZONTAL) {
      FLOAT sx = (clientDragRect.end.x - m_dragPosInBar.x) - GetSlideAreaOffset();
      FLOAT x = (m_maxValue - m_minValue) / (GetSlideAreaLength() - GetBarLength()) * sx;
      SetBarPosition(x);
#ifdef DEBUG_UISCROLLBAR
      LOG(SEVERITY_LEVEL_DEBUG) << L"sx = " << sx << L", x = " << x;
#endif
    } else {
      FLOAT sy = (clientDragRect.end.y - m_dragPosInBar.y) - GetSlideAreaOffset();
      FLOAT y = (m_maxValue - m_minValue) / (GetSlideAreaLength() - GetBarLength()) * sy;
      SetBarPosition(y);
#ifdef DEBUG_UISCROLLBAR
      LOG(SEVERITY_LEVEL_DEBUG) << L"sy = " << sy << L", y = " << y;
#endif
    }
  }
  m_dragging = false;
#ifdef DEBUG_UISCROLLBAR
  LOG_LEAVE(SEVERITY_LEVEL_DEBUG);
#endif
}

void ui::UIScrollBar::OnMouseOver(HWND hWnd, WPARAM wParam, LPARAM lParam, UIPoint clientPoint, UIDelta delta,
                                  ULONGLONG timestampInMilliSeconds, OUT bool *eaten) {
#ifdef DEBUG_UISCROLLBAR
  LOG_ENTER(SEVERITY_LEVEL_DEBUG) << L"clientPoint=" << clientPoint;
#endif
  UNREFERENCED_PARAMETER(hWnd);
  UNREFERENCED_PARAMETER(wParam);
  UNREFERENCED_PARAMETER(lParam);
  UNREFERENCED_PARAMETER(clientPoint);
  UNREFERENCED_PARAMETER(delta);
  UNREFERENCED_PARAMETER(timestampInMilliSeconds);
  UNREFERENCED_PARAMETER(eaten);
  //     if(m_arrow1->HitTest(clientPoint.x, clientPoint.y)) {
  //         LOG(SEVERITY_LEVEL_DEBUG) << L"enter into arrow1";
  //         m_arrow1->SetColor(graphics::COLOR_PATTERN_CONVEX, graphics::HSVColor(32.0f, 0.0f, 1.0f, 1.0f));
  //         m_propertyChanged = true;
  //     }
  //     else {
  //         m_arrow1->SetColor(graphics::COLOR_PATTERN_CONVEX, graphics::HSVColor(32.0f, 0.0f, 0.5f, 1.0f));
  //         m_propertyChanged = true;
  //     }
  Invalidate();
#ifdef DEBUG_UISCROLLBAR
  LOG_LEAVE(SEVERITY_LEVEL_DEBUG);
#endif
}

void ui::UIScrollBar::OnMouseOut(HWND hWnd, WPARAM wParam, LPARAM lParam, UIPoint clientPoint, UIDelta delta,
                                 ULONGLONG timestampInMilliSeconds, OUT bool *eaten) {
#ifdef DEBUG_UISCROLLBAR
  LOG_ENTER(SEVERITY_LEVEL_DEBUG) << L"clientPoint=" << clientPoint;
#endif
  //     m_arrow1->SetColor(graphics::COLOR_PATTERN_CONVEX, graphics::HSVColor(32.0f, 0.0f, 0.5f, 1.0f));
  //     m_propertyChanged = true;
  //     Invalidate();
  UNREFERENCED_PARAMETER(hWnd);
  UNREFERENCED_PARAMETER(wParam);
  UNREFERENCED_PARAMETER(lParam);
  UNREFERENCED_PARAMETER(clientPoint);
  UNREFERENCED_PARAMETER(delta);
  UNREFERENCED_PARAMETER(timestampInMilliSeconds);
  UNREFERENCED_PARAMETER(eaten);
#ifdef DEBUG_UISCROLLBAR
  LOG_LEAVE(SEVERITY_LEVEL_DEBUG);
#endif
}

void ui::UIScrollBar::MoveBarPosition(FLOAT d) {
#ifdef DEBUG_UISCROLLBAR
  LOG_ENTER(SEVERITY_LEVEL_DEBUG);
#endif
  FLOAT newValue = m_currentValue + d;
  if (newValue < m_minValue) {
    newValue = m_minValue;
  }
  if (newValue + m_pageSize > m_maxValue) {
    newValue = m_maxValue - m_pageSize;
  }
  if (m_currentValue != newValue) {
    m_currentValue = newValue;
    m_barPositionChanged = true;
    Invalidate();
    FirePositionChangedEvent();
  }
#ifdef DEBUG_UISCROLLBAR
  LOG_LEAVE(SEVERITY_LEVEL_DEBUG);
#endif
}

void ui::UIScrollBar::SetBarPosition(FLOAT pos) {
#ifdef DEBUG_UISCROLLBAR
  LOG_ENTER(SEVERITY_LEVEL_DEBUG);
#endif
  if (pos < m_minValue) {
    pos = m_minValue;
  }
  if (pos + m_pageSize > m_maxValue) {
    pos = m_maxValue - m_pageSize;
  }
  if (m_currentValue != pos) {
    m_currentValue = pos;
    m_barPositionChanged = true;
    Invalidate();
    FirePositionChangedEvent();
  }
#ifdef DEBUG_UISCROLLBAR
  LOG_LEAVE(SEVERITY_LEVEL_DEBUG);
#endif
}

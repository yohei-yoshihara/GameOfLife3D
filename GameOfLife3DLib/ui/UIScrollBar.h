#ifndef UI_UISCROLLBAR_H_
#define UI_UISCROLLBAR_H_

#include "graphics/GraphicsCommon.h"
#include "graphics/color/ColorUtil.h"
#include "ui/UIBase.h"

namespace ui {
  typedef std::function<void(FLOAT)> UIScrollBarPositionChangedCallback;

  class UIScrollBar : public UIBase, public std::enable_shared_from_this<UIScrollBar> {
  public:
    enum SCROLLBAR_DIRECTION {
      SCROLLBAR_DIRECTION_HORIZONTAL,
      SCROLLBAR_DIRECTION_VERTICAL,
    };

  private:
    SCROLLBAR_DIRECTION m_direction;
    FLOAT m_minValue;
    FLOAT m_maxValue;
    FLOAT m_pageSize;
    FLOAT m_currentValue;
    bool m_firstInitialization;
    bool m_barPositionChanged;
    bool m_propertyChanged;
    bool m_dragging;
    ui::UIPoint m_dragPosInBar;

    std::unique_ptr<graphics::figure::RectangleFigure> m_body;
    std::unique_ptr<graphics::figure::TriangleFigure> m_arrow1;
    std::unique_ptr<graphics::figure::TriangleFigure> m_arrow2;
    std::unique_ptr<graphics::figure::RectangleFigure> m_slideArea;
    std::unique_ptr<graphics::figure::RectangleFigure> m_bar;

    std::vector<UIScrollBarPositionChangedCallback> m_positionChangedCallbacks;

  public:
    UIScrollBar();
    virtual ~UIScrollBar();
    void SetDirection(SCROLLBAR_DIRECTION direction) {
      m_direction = direction;
    }
    SCROLLBAR_DIRECTION GetDirection() const {
      return m_direction;
    }
    void SetMinValue(FLOAT minValue) {
      m_minValue = minValue;
      m_propertyChanged = true;
    }
    FLOAT GetMinValue() const {
      return m_minValue;
    }
    void SetMaxValue(FLOAT maxValue) {
      m_maxValue = maxValue;
      m_propertyChanged = true;
    }
    FLOAT GetMaxValue() const {
      return m_maxValue;
    }
    void SetPageSize(FLOAT pageSize) {
      m_pageSize = pageSize;
      m_propertyChanged = true;
    }
    FLOAT GetPageSize() const {
      return m_pageSize;
    }
    void SetCurrentValue(FLOAT currentValue) {
      m_currentValue = currentValue;
      m_propertyChanged = true;
    }
    FLOAT GetCurrentValue() const {
      return m_currentValue;
    }

    FLOAT GetTotalThick() const {
      return m_direction == SCROLLBAR_DIRECTION_HORIZONTAL ? GetHeight() : GetWidth();
    }
    FLOAT GetTotalLength() const {
      return m_direction == SCROLLBAR_DIRECTION_HORIZONTAL ? GetWidth() : GetHeight();
    }
    FLOAT GetSlideAreaThick() const {
      return GetTotalThick() - GetSlideAreaMargin() * 2.0f;
    }
    FLOAT GetSlideAreaLength() const {
      return GetTotalLength()
             - (GetArrowMargin() + GetArrowLength() + GetSplitterMargin() + GetSplitterThick() + GetSlideAreaMargin())
                   * 2.0f;
    }
    FLOAT GetSlideAreaMargin() const {
      return 2.0f;
    }
    FLOAT GetSlideAreaOffset() const {
      return GetArrowMargin() + GetArrowLength() + GetSplitterMargin() + GetSplitterThick() + GetSlideAreaMargin();
    }
    // bar
    FLOAT GetBarPosition() const {
      return GetSlideAreaLength() * GetCurrentValue() / (GetMaxValue() - GetMinValue());
    }
    FLOAT GetBarLength() const {
      return GetSlideAreaLength() * GetPageSize() / (GetMaxValue() - GetMinValue());
    }
    FLOAT GetBarThick() const {
      return GetSlideAreaThick();
    }
    FLOAT GetBarOffset() const {
      return GetArrowMargin() + GetArrowLength() + GetSplitterMargin() + GetSplitterThick() + GetSlideAreaMargin()
             + GetBarPosition();
    }
    // arrow
    FLOAT GetArrowLength() const {
      return GetArrowThick();
    }
    FLOAT GetArrowThick() const {
      return GetTotalThick() - GetArrowMargin() * 2.0f;
    }
    FLOAT GetArrowMargin() const {
      return 2.0f;
    }
    FLOAT GetArrow1Offset() const {
      return GetArrowMargin();
    }
    FLOAT GetArrow2Offset() const {
      return GetArrowMargin() + GetArrowLength() + GetSplitterMargin() * 2.0f + GetSplitterThick() * 2.0f
             + GetSlideAreaMargin() * 2.0f + GetSlideAreaLength();
    }
    // splitter
    FLOAT GetSplitterMargin() const {
      return 2.0f;
    }
    FLOAT GetSplitterThick() const {
      return 1.0f;
    }
    FLOAT GetSplitterHeight() const {
      return GetBarThick() - GetSplitterMargin() * 2.0f;
    }
    FLOAT GetSplitterOffset() const {
      return GetArrowMargin() + GetArrowLength() + GetSplitterMargin();
    }
    void AddPositionChangedCallback(UIScrollBarPositionChangedCallback callback) {
      m_positionChangedCallbacks.push_back(callback);
    }
    void ClearPositionChangedCallback() {
      m_positionChangedCallbacks.clear();
    }
    void FirePositionChangedEvent() {
      for (auto it = m_positionChangedCallbacks.begin(); it != m_positionChangedCallbacks.end(); ++it) {
        (*it)(m_currentValue);
      }
    }

    virtual HRESULT Initialize(graphics::D3DInteropHelper *pD3DInteropHelper);
    virtual HRESULT CreateDeviceDependentResources(graphics::D3DInteropHelper *pD3DInteropHelper,
                                                   ID2D1RenderTarget *pRenderTarget);
    virtual HRESULT Render(graphics::D3DInteropHelper *pD3DInteropHelper, ID2D1RenderTarget *pRenderTarget);
    virtual void DiscardDeviceDependentResources();

    virtual void OnLeftMouseDown(HWND hWnd, WPARAM wParam, LPARAM lParam, UIPoint clientPoint,
                                 ULONGLONG timestampInMilliSeconds, OUT bool *eaten);
    virtual void OnDraggingStart(HWND hWnd, WPARAM wParam, LPARAM lParam, UI2Points clientDragRect, UIDelta delta,
                                 ULONGLONG timestampInMilliSeconds, OUT bool *eaten);
    virtual void OnDragging(HWND hWnd, WPARAM wParam, LPARAM lParam, UI2Points clientDragRect, UIDelta delta,
                            ULONGLONG timestampInMilliSeconds, OUT bool *eaten);
    virtual void OnDraggingEnd(HWND hWnd, WPARAM wParam, LPARAM lParam, UI2Points clientDragRect, UIDelta delta,
                               ULONGLONG timestampInMilliSeconds, OUT bool *eaten);
    virtual void OnMouseOver(HWND hWnd, WPARAM wParam, LPARAM lParam, UIPoint clientPoint, UIDelta delta,
                             ULONGLONG timestampInMilliSeconds, OUT bool *eaten);
    virtual void OnMouseOut(HWND hWnd, WPARAM wParam, LPARAM lParam, UIPoint clientPoint, UIDelta delta,
                            ULONGLONG timestampInMilliSeconds, OUT bool *eaten);
    HRESULT UpdateBarPosition();
    void MoveBarPosition(FLOAT d);
    void SetBarPosition(FLOAT pos);
  };
}
#endif // UI_UISCROLLBAR_H_
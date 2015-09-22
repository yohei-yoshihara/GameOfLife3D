#ifndef UI_UICONTAINER_H_
#define UI_UICONTAINER_H_

#include "ui/UICommon.h"
#include "ui/UIBase.h"

namespace ui {
  typedef std::function<bool(std::shared_ptr<UIBase>)> UIContainerCallback;
  typedef std::function<HRESULT(graphics::D3DInteropHelper *pD3DInteropHelper)> UIContainerInitializationCallback;

  class UIContainer : public UIBase {
  private:
    std::vector<std::shared_ptr<UIBase>> m_elements;
    std::weak_ptr<UIBase> m_leftDownChild;
    std::weak_ptr<UIBase> m_draggingChild;
    std::weak_ptr<UIBase> m_mouseOverChild;
    std::weak_ptr<UIBase> m_gestureChild;
    UIInsets m_insets;
    std::vector<UIContainerCallback> m_beforeAddElementCallback;
    std::vector<UIContainerCallback> m_beforeRemoveElementCallback;
    std::vector<UIContainerInitializationCallback> m_initializationCallback;

  public:
    UIContainer(void);
    virtual ~UIContainer(void);
    virtual HRESULT Initialize(graphics::D3DInteropHelper *pD3DInteropHelper);
    virtual HRESULT LayoutComponents();
    virtual HRESULT CreateDeviceDependentResources(graphics::D3DInteropHelper *pD3DInteropHelper,
                                                   ID2D1RenderTarget *pRenderTarget);
    virtual HRESULT Render(graphics::D3DInteropHelper *pD3DInteropHelper, ID2D1RenderTarget *pRenderTarget);
    virtual void DiscardDeviceDependentResources();
    void AddElement(const std::shared_ptr<UIBase> &element) {
      for (auto it = m_beforeAddElementCallback.begin(); it != m_beforeAddElementCallback.end(); ++it) {
        if (!(*it)(element)) {
          return;
        }
      }
      m_elements.push_back(element);
    }
    std::shared_ptr<UIBase> GetElement(size_t index) const {
      return m_elements.at(index);
    }
    size_t GetNumberOfElements() const {
      return m_elements.size();
    }
    bool IsChild(const std::shared_ptr<UIBase> &element) const {
      return std::find_if(m_elements.begin(), m_elements.end(), [&](const std::shared_ptr<UIBase> e) -> bool {
               return e.get() == element.get();
             }) != m_elements.end();
    }
    void RemoveElement(const std::shared_ptr<UIBase> &element) {
      if (IsChild(element)) {
        for (auto it = m_beforeRemoveElementCallback.begin(); it != m_beforeRemoveElementCallback.end(); ++it) {
          if (!(*it)(element)) {
            return;
          }
        }
        m_elements.erase(
            std::remove_if(m_elements.begin(), m_elements.end(), [&](const std::shared_ptr<UIBase> e) -> bool {
              return e.get() == element.get();
            }), m_elements.end());
      }
    }
    void RemoveElement(size_t index) {
      RemoveElement(m_elements[index]);
    }
    void AddBeforeAddElementCallback(UIContainerCallback callback) {
      m_beforeAddElementCallback.push_back(callback);
    }
    void ClearBeforeAddElementCallbacks() {
      m_beforeAddElementCallback.clear();
    }
    void AddBeforeRemoveElementCallback(UIContainerCallback callback) {
      m_beforeRemoveElementCallback.push_back(callback);
    }
    void ClearBeforeRemoveElementCallbacks() {
      m_beforeRemoveElementCallback.clear();
    }
    void AddInitializationCallback(UIContainerInitializationCallback callback) {
      m_initializationCallback.push_back(callback);
    }
    void ClearInitializationCallback() {
      m_initializationCallback.clear();
    }
    void SetInsets(UIInsets insets) {
      m_insets = insets;
    }
    UIInsets GetInsets() const {
      return m_insets;
    }
    virtual HWND GetHWnd() const {
      if (!GetParentContainer().expired()) {
        return GetParentContainer().lock()->GetHWnd();
      } else {
        throw std::runtime_error("no root container");
      }
    }
    virtual void NotifyFocusObtained(const std::shared_ptr<UIBase> &uiBase) = 0;

    // events
    virtual void OnSetFocus(HWND hWnd, WPARAM wParam, LPARAM lParam, ULONGLONG timestampInMilliSeconds,
                            OUT bool *eaten);
    virtual void OnKillFocus(HWND hWnd, WPARAM wParam, LPARAM lParam, ULONGLONG timestampInMilliSeconds,
                             OUT bool *eaten);
    virtual void OnChar(HWND hWnd, WPARAM wParam, LPARAM lParam, CHAR_TYPE charType, wchar_t c,
                        ULONGLONG timestampInMilliSeconds, OUT bool *eaten);
    virtual void OnLeftMouseDown(HWND hWnd, WPARAM wParam, LPARAM lParam, UIPoint clientPoint,
                                 ULONGLONG timestampInMilliSeconds, OUT bool *eaten);
    virtual void OnLeftMouseUp(HWND hWnd, WPARAM wParam, LPARAM lParam, UIPoint clientPoint,
                               ULONGLONG timestampInMilliSeconds, OUT bool *eaten);
    virtual void OnSingleClick(HWND hWnd, WPARAM wParam, LPARAM lParam, UIPoint clientPoint,
                               ULONGLONG timestampInMilliSeconds, OUT bool *eaten);
    virtual void OnDoubleClick(HWND hWnd, WPARAM wParam, LPARAM lParam, UIPoint clientPoint,
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
    virtual void OnRightSingleClick(HWND hWnd, WPARAM wParam, LPARAM lParam, UIPoint clientPoint,
                                    ULONGLONG timestampInMilliSeconds, OUT bool *eaten);
    virtual void OnGestureBegin(HWND hWnd, WPARAM wParam, LPARAM lParam, UIPoint clientPoint, PGESTUREINFO pGestureInfo,
                                ULONGLONG timestampInMilliSeconds, OUT bool *eaten);
    virtual void OnGestureEnd(HWND hWnd, WPARAM wParam, LPARAM lParam, UIPoint clientPoint, PGESTUREINFO pGestureInfo,
                              ULONGLONG timestampInMilliSeconds, OUT bool *eaten);
    virtual void OnZoom(HWND hWnd, WPARAM wParam, LPARAM lParam, UIPoint clientPoint, PGESTUREINFO pGestureInfo,
                        UIPoint zoomCenterPoint, FLOAT zoomFactor, ULONGLONG timestampInMilliSeconds, OUT bool *eaten);
    virtual void OnPan(HWND hWnd, WPARAM wParam, LPARAM lParam, UIPoint clientPoint, PGESTUREINFO pGestureInfo,
                       UIDelta delta, ULONGLONG timestampInMilliSeconds, OUT bool *eaten);
    virtual void OnRotate(HWND hWnd, WPARAM wParam, LPARAM lParam, UIPoint clientPoint, PGESTUREINFO pGestureInfo,
                          UIPoint rotateCenterPoint, FLOAT rotateAngle, ULONGLONG timestampInMilliSeconds,
                          OUT bool *eaten);
    virtual void OnTwoFingerTap(HWND hWnd, WPARAM wParam, LPARAM lParam, UIPoint clientPoint, PGESTUREINFO pGestureInfo,
                                ULONGLONG timestampInMilliSeconds, OUT bool *eaten);
    virtual void OnPressAndTap(HWND hWnd, WPARAM wParam, LPARAM lParam, UIPoint clientPoint, PGESTUREINFO pGestureInfo,
                               ULONGLONG timestampInMilliSeconds, OUT bool *eaten);
    virtual std::wostream &Dump(std::wostream &os) const;
  };
  std::wostream &operator<<(std::wostream &os, UIContainer const &container);
}

#endif // UI_UICONTAINER_H_
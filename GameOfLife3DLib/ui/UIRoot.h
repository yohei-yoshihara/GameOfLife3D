#ifndef UI_UIROOT_H_
#define UI_UIROOT_H_

#include "graphics/GraphicsCommon.h"
#include "ui/UICommon.h"
#include "ui/UIBase.h"
#include "ui/UIContainer.h"

namespace ui
{
class UIRoot :
    public UIContainer,
    public std::enable_shared_from_this<UIRoot>
{
private:
    HWND m_hWnd;
    std::unordered_map<std::wstring, std::shared_ptr<ui::IUIDialog>> m_dialogs;
    std::wstring m_displayingDialogName;
    CComPtr<ID2D1SolidColorBrush> m_blackBrush;

public:
    UIRoot(void);
    virtual ~UIRoot(void);
    virtual HRESULT Initialize(
        graphics::D3DInteropHelper *pD3DInteropHelper);
    virtual HRESULT LayoutComponents();
    virtual HRESULT CreateDeviceDependentResources(
        graphics::D3DInteropHelper *pD3DInteropHelper,
        ID2D1RenderTarget *pRenderTarget);
    virtual HRESULT Render(
        graphics::D3DInteropHelper *pD3DInteropHelper,
        ID2D1RenderTarget *pRenderTarget);
    virtual void DiscardDeviceDependentResources();

    virtual void SetHWnd(HWND hWnd) {
        m_hWnd = hWnd;
    }
    virtual HWND GetHWnd() const {
        return m_hWnd;
    }
    virtual void NotifyFocusObtained(const std::shared_ptr<UIBase> &uiBase);
    virtual void OnResize(UISize size);
    virtual HRESULT Invalidate();

    // internal dialog
    virtual void AddDialog(const std::wstring &name, const std::shared_ptr<ui::IUIDialog> &dialog);
    virtual void ShowDialog(const std::wstring &name);
    virtual void NotifyDialogClosed();

    // events
    virtual void OnSetFocus(HWND hWnd, WPARAM wParam, LPARAM lParam, ULONGLONG timestampInMilliSeconds, OUT bool* eaten);
    virtual void OnKillFocus(HWND hWnd, WPARAM wParam, LPARAM lParam, ULONGLONG timestampInMilliSeconds, OUT bool* eaten);
    virtual void OnChar(HWND hWnd, WPARAM wParam, LPARAM lParam, CHAR_TYPE charType, wchar_t c, ULONGLONG timestampInMilliSeconds, OUT bool* eaten);
    virtual void OnLeftMouseDown(HWND hWnd, WPARAM wParam, LPARAM lParam, UIPoint clientPoint, ULONGLONG timestampInMilliSeconds, OUT bool* eaten);
    virtual void OnLeftMouseUp(HWND hWnd, WPARAM wParam, LPARAM lParam, UIPoint clientPoint, ULONGLONG timestampInMilliSeconds, OUT bool* eaten);
    virtual void OnSingleClick(HWND hWnd, WPARAM wParam, LPARAM lParam, UIPoint clientPoint, ULONGLONG timestampInMilliSeconds, OUT bool* eaten);
    virtual void OnDoubleClick(HWND hWnd, WPARAM wParam, LPARAM lParam, UIPoint clientPoint, ULONGLONG timestampInMilliSeconds, OUT bool* eaten);
    virtual void OnDraggingStart(HWND hWnd, WPARAM wParam, LPARAM lParam, UI2Points clientDragRect, UIDelta delta, ULONGLONG timestampInMilliSeconds, OUT bool* eaten);
    virtual void OnDragging(HWND hWnd, WPARAM wParam, LPARAM lParam, UI2Points clientDragRect, UIDelta delta, ULONGLONG timestampInMilliSeconds, OUT bool* eaten);
    virtual void OnDraggingEnd(HWND hWnd, WPARAM wParam, LPARAM lParam, UI2Points clientDragRect, UIDelta delta, ULONGLONG timestampInMilliSeconds, OUT bool* eaten);
    virtual void OnMouseOver(HWND hWnd, WPARAM wParam, LPARAM lParam, UIPoint clientPoint, UIDelta delta, ULONGLONG timestampInMilliSeconds, OUT bool* eaten);
    virtual void OnMouseOut(HWND hWnd, WPARAM wParam, LPARAM lParam, UIPoint clientPoint, UIDelta delta, ULONGLONG timestampInMilliSeconds, OUT bool* eaten);
    virtual void OnRightSingleClick(HWND hWnd, WPARAM wParam, LPARAM lParam, UIPoint clientPoint, ULONGLONG timestampInMilliSeconds, OUT bool* eaten);
    virtual void OnGestureBegin(HWND hWnd, WPARAM wParam, LPARAM lParam, UIPoint clientPoint, PGESTUREINFO pGestureInfo, ULONGLONG timestampInMilliSeconds, OUT bool* eaten);
    virtual void OnGestureEnd(HWND hWnd, WPARAM wParam, LPARAM lParam, UIPoint clientPoint, PGESTUREINFO pGestureInfo, ULONGLONG timestampInMilliSeconds, OUT bool* eaten);
    virtual void OnZoom(HWND hWnd, WPARAM wParam, LPARAM lParam, UIPoint clientPoint, PGESTUREINFO pGestureInfo, UIPoint zoomCenterPoint, FLOAT zoomFactor, ULONGLONG timestampInMilliSeconds, OUT bool* eaten);
    virtual void OnPan(HWND hWnd, WPARAM wParam, LPARAM lParam, UIPoint clientPoint, PGESTUREINFO pGestureInfo, UIDelta delta, ULONGLONG timestampInMilliSeconds, OUT bool* eaten);
    virtual void OnRotate(HWND hWnd, WPARAM wParam, LPARAM lParam, UIPoint clientPoint, PGESTUREINFO pGestureInfo, UIPoint rotateCenterPoint, FLOAT rotateAngle, ULONGLONG timestampInMilliSeconds, OUT bool* eaten);
    virtual void OnTwoFingerTap(HWND hWnd, WPARAM wParam, LPARAM lParam, UIPoint clientPoint, PGESTUREINFO pGestureInfo, ULONGLONG timestampInMilliSeconds, OUT bool* eaten);
    virtual void OnPressAndTap(HWND hWnd, WPARAM wParam, LPARAM lParam, UIPoint clientPoint, PGESTUREINFO pGestureInfo, ULONGLONG timestampInMilliSeconds, OUT bool* eaten);
};
}

#endif // UI_UIROOT_H_
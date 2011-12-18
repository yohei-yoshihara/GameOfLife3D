#ifndef UI_UIBASE_H_
#define UI_UIBASE_H_

#include "graphics/GraphicsCommon.h"
#include "ui/UICommon.h"
#include "ui/event/IUIInputEvent.h"

namespace ui
{
class UIBase : public ui::event::IUIInputEvent
{
private:
    std::weak_ptr<UIContainer> m_parentContainer;
    ui::UIRectangle m_bounds;
    bool m_enable;
    bool m_focusable;
    bool m_focus;
    ui::UIAnchor m_anchor;
    bool m_propertyChanged;

public:
    UIBase();
    virtual ~UIBase();
    virtual HRESULT Initialize(
        graphics::D3DInteropHelper *pD3DInteropHelper) = 0;
    virtual HRESULT CreateDeviceDependentResources(
        graphics::D3DInteropHelper *pD3DInteropHelper,
        ID2D1RenderTarget *pRenderTarget) = 0;
    virtual HRESULT Render(
        graphics::D3DInteropHelper *pD3DInteropHelper,
        ID2D1RenderTarget *pRenderTarget) = 0;
    virtual void DiscardDeviceDependentResources() = 0;
    virtual HRESULT Invalidate();

    void SetPropertyChanged(bool propertyChanged) {
        m_propertyChanged = propertyChanged;
    }
    bool IsPropertyChanged() const {
        return m_propertyChanged;
    }
    void SetParentContainer(std::weak_ptr<UIContainer> parentContainer) {
        m_parentContainer = parentContainer;
    }
    std::weak_ptr<UIContainer> GetParentContainer() const {
        return m_parentContainer;
    }
    FLOAT GetX() const {
        return m_bounds.x;
    }
    FLOAT GetY() const {
        return m_bounds.y;
    }
    FLOAT GetWidth() const {
        return m_bounds.width;
    }
    FLOAT GetHeight() const {
        return m_bounds.height;
    }
    void SetBounds(FLOAT x, FLOAT y, FLOAT width, FLOAT height) {
        SetBounds(ui::UIRectangle(x, y, width, height));
    }
    void SetBounds(const ui::UIRectangle &bounds) {
        m_bounds = bounds;
        SetPropertyChanged(true);
    }
    const ui::UIRectangle& GetBounds() const {
        return m_bounds;
    }
    void SetEnabled(bool enable) {
        m_enable = enable;
        SetPropertyChanged(true);
    }
    bool IsEnabled() const {
        return m_enable;
    }
    void SetFocusable(bool focusable) {
        m_focusable = focusable;
        SetPropertyChanged(true);
    }
    bool IsFocusable() const {
        return m_focusable;
    }
    void RequestFocus() {
        m_focus = true;
    }
    void LostFocus() {
        m_focus = false;
    }
    bool HasFocus() const {
        return m_focus;
    }
    void SetAnchor(UIAnchor anchor) {
        m_anchor = anchor;
        SetPropertyChanged(true);
    }
    UIAnchor GetAnchor() const {
        return m_anchor;
    }

    HRESULT ClientToScreen(UIPoint clientPoint, UIPoint &screenPoint) const;
    HRESULT ClientToScreen(UIRectangle clientRect, UIRectangle &screenRect) const;
    HRESULT ClientToWindow(UIPoint clientPoint, UIPoint &windowPoint) const;
    HRESULT ClientToWindow(UIRectangle clientRect, UIRectangle &windowRect) const;

    // event
    virtual void OnSetFocus(
        HWND,
        WPARAM,
        LPARAM,
        ULONGLONG,
        OUT bool* eaten) {
        *eaten = false;
    }
    virtual void OnKillFocus(
        HWND,
        WPARAM,
        LPARAM,
        ULONGLONG,
        OUT bool* eaten) {
        *eaten = false;
    }
    virtual void OnChar(
        HWND,
        WPARAM,
        LPARAM,
        CHAR_TYPE,
        wchar_t,
        ULONGLONG,
        OUT bool* eaten) {
        *eaten = false;
    }
    virtual void OnLeftMouseDown(
        HWND,
        WPARAM,
        LPARAM,
        UIPoint,
        ULONGLONG,
        OUT bool* eaten) {
        *eaten = false;
    }
    virtual void OnLeftMouseUp(
        HWND,
        WPARAM,
        LPARAM,
        UIPoint,
        ULONGLONG,
        OUT bool* eaten) {
        *eaten = false;
    }
    virtual void OnSingleClick(
        HWND,
        WPARAM,
        LPARAM,
        UIPoint,
        ULONGLONG,
        OUT bool* eaten) {
        *eaten = false;
    }
    virtual void OnDoubleClick(
        HWND,
        WPARAM,
        LPARAM,
        UIPoint,
        ULONGLONG,
        OUT bool* eaten) {
        *eaten = false;
    }
    virtual void OnDraggingStart(
        HWND,
        WPARAM,
        LPARAM,
        UI2Points,
        UIDelta,
        ULONGLONG,
        OUT bool* eaten) {
        *eaten = false;
    }
    virtual void OnDragging(
        HWND,
        WPARAM,
        LPARAM,
        UI2Points,
        UIDelta,
        ULONGLONG,
        OUT bool* eaten) {
        *eaten = false;
    }
    virtual void OnDraggingEnd(
        HWND,
        WPARAM,
        LPARAM,
        UI2Points,
        UIDelta,
        ULONGLONG,
        OUT bool* eaten) {
        *eaten = false;
    }
    virtual void OnMouseOver(
        HWND,
        WPARAM,
        LPARAM,
        UIPoint,
        UIDelta,
        ULONGLONG,
        OUT bool* eaten) {
        *eaten = false;
    }
    virtual void OnMouseOut(
        HWND,
        WPARAM,
        LPARAM,
        UIPoint,
        UIDelta,
        ULONGLONG,
        OUT bool* eaten) {
        *eaten = false;
    }
    virtual void OnRightSingleClick(
        HWND,
        WPARAM,
        LPARAM,
        UIPoint,
        ULONGLONG,
        OUT bool* eaten) {
        *eaten = false;
    }
    virtual void OnGestureBegin(
        HWND,
        WPARAM,
        LPARAM,
        UIPoint,
        PGESTUREINFO,
        ULONGLONG,
        OUT bool* eaten) {
        *eaten = false;
    }
    virtual void OnGestureEnd(
        HWND,
        WPARAM,
        LPARAM, UIPoint,
        PGESTUREINFO,
        ULONGLONG,
        OUT bool* eaten) {
        *eaten = false;
    }
    virtual void OnZoom(
        HWND,
        WPARAM,
        LPARAM,
        UIPoint,
        PGESTUREINFO,
        UIPoint,
        FLOAT,
        ULONGLONG,
        OUT bool* eaten) {
        *eaten = false;
    }
    virtual void OnPan(
        HWND,
        WPARAM,
        LPARAM,
        UIPoint,
        PGESTUREINFO,
        UIDelta,
        ULONGLONG,
        OUT bool* eaten) {
        *eaten = false;
    }
    virtual void OnRotate(
        HWND,
        WPARAM,
        LPARAM,
        UIPoint,
        PGESTUREINFO,
        UIPoint,
        FLOAT,
        ULONGLONG,
        OUT bool* eaten) {
        *eaten = false;
    }
    virtual void OnTwoFingerTap(
        HWND,
        WPARAM,
        LPARAM,
        UIPoint,
        PGESTUREINFO,
        ULONGLONG,
        OUT bool* eaten) {
        *eaten = false;
    }
    virtual void OnPressAndTap(
        HWND,
        WPARAM,
        LPARAM,
        UIPoint,
        PGESTUREINFO,
        ULONGLONG,
        OUT bool* eaten) {
        *eaten = false;
    }

    // utility methods
    bool HitTest(UIPoint point) const {
        return GetX() <= point.x && point.x <= GetX() + GetWidth() &&
               GetY() <= point.y && point.y <= GetY() + GetHeight();
    }
    bool HitTest(UIPoint point, FLOAT offsetX, FLOAT offsetY) const {
        return (GetX() + offsetX) <= point.x && point.x <= (GetX() + GetWidth() + offsetX) &&
               (GetY() + offsetY) <= point.y && point.y <= (GetY() + GetHeight() + offsetY);
    }
    UIPoint Translate(UIPoint point) {
        return UIPoint(
                   point.x - GetX(),
                   point.y - GetY());
    }
    UIPoint Translate(UIPoint point, FLOAT offsetX, FLOAT offsetY) {
        return UIPoint(
                   point.x - GetX() - offsetX,
                   point.y - GetY() - offsetY);
    }
    UIRectangle Translate(UIRectangle rect) {
        return UIRectangle(
                   rect.x - GetX(),
                   rect.y - GetY(),
                   rect.width,
                   rect.height);
    }
    UIRectangle Translate(UIRectangle rect, FLOAT offsetX, FLOAT offsetY) {
        return UIRectangle(
                   rect.x - GetX() - offsetX,
                   rect.y - GetY() - offsetY,
                   rect.width,
                   rect.height);
    }
    UI2Points Translate(UI2Points points) {
        return UI2Points(
                   points.start.x - GetX(),
                   points.start.y - GetY(),
                   points.end.x - GetX(),
                   points.end.y - GetY());
    }
    UI2Points Translate(UI2Points points, FLOAT offsetX, FLOAT offsetY) {
        return UI2Points(
                   points.start.x - GetX() - offsetX,
                   points.start.y - GetY() - offsetY,
                   points.end.x - GetX() - offsetX,
                   points.end.y - GetY() - offsetY);
    }
    virtual std::wostream& Dump(std::wostream& os) const;
    friend std::wostream& operator<< (std::wostream& os, UIBase const& uiBase);
};
}

#endif // UI_UIBASE_H_
#ifndef UI_UIBUTTON_H_
#define UI_UIBUTTON_H_

#include "graphics/GraphicsCommon.h"
#include "graphics/FontAttribute.h"
#include "ui/UICommon.h"
#include "ui/UIBase.h"
#include "ui/event/UIActionEvent.h"
#include "ui/event/EventSupport.h"

namespace ui
{
class UIButton :
    public UIBase,
    public std::enable_shared_from_this<UIButton>
{
public:
    enum BUTTON_STATUS {
        BUTTON_STATUS_NORMAL,
        BUTTON_STATUS_DOWN,
        BUTTON_STATUS_MOUSEOVER,
    };
private:
    std::wstring m_text;
    graphics::FontAttribute m_fontAttribute;
    std::shared_ptr<graphics::brush::LinearGradientBrush> m_fgGradientBrush;
    std::shared_ptr<graphics::brush::LinearGradientBrush> m_bgGradientBrush;
    std::shared_ptr<graphics::brush::LinearGradientBrush> m_bgDownGradientBrush;
    std::shared_ptr<graphics::brush::LinearGradientBrush> m_bgMouseOverGradientBrush;

    CComPtr<IDWriteTextFormat> m_textFormat;
    CComPtr<IDWriteTextLayout> m_textLayout;

    DWRITE_TEXT_METRICS m_textMetrics;

    D2D1_COLOR_F m_shineColor;
    D2D1_COLOR_F m_shadowColor;
    CComPtr<ID2D1SolidColorBrush> m_shineColorBrush;
    CComPtr<ID2D1SolidColorBrush> m_shadowColorBrush;

    ui::event::EventSupport<ui::event::UIActionListenerFunctor, ui::event::UIActionEvent> m_actionListeners;
    BUTTON_STATUS m_status;

protected:
    virtual void ClearResources();

public:
    UIButton(void);
    virtual ~UIButton(void);
    virtual HRESULT Initialize(
        graphics::D3DInteropHelper *pD3DInteropHelper);
    virtual HRESULT CreateDeviceDependentResources(
        graphics::D3DInteropHelper *pD3DInteropHelper,
        ID2D1RenderTarget *pRenderTarget);
    virtual HRESULT Render(
        graphics::D3DInteropHelper *pD3DInteropHelper,
        ID2D1RenderTarget *pRenderTarget);
    virtual void DiscardDeviceDependentResources();

    virtual void SetText(const std::wstring &text);
    virtual void SetForegroundColor(
        const std::vector<D2D1_GRADIENT_STOP> &foregroundColors,
        graphics::GRADIENT_DIRECTION fgGradientDirection);
    virtual void SetBackgroundColor(
        const std::vector<D2D1_GRADIENT_STOP> &backgroundColors,
        graphics::GRADIENT_DIRECTION bgGradientDirection);
    virtual void SetFontAttribute(const graphics::FontAttribute &fontAttribute);
    // events
    virtual void OnLeftMouseDown(
        HWND hWnd,
        WPARAM wParam,
        LPARAM lParam,
        UIPoint clientPoint,
        ULONGLONG timestampInMilliSeconds,
        OUT bool* eaten);
    virtual void OnLeftMouseUp(
        HWND hWnd,
        WPARAM wParam,
        LPARAM lParam,
        UIPoint clientPoint,
        ULONGLONG timestampInMilliSeconds,
        OUT bool* eaten);
    virtual void OnMouseOver(
        HWND hWnd,
        WPARAM wParam,
        LPARAM lParam,
        UIPoint clientPoint,
        UIDelta delta,
        ULONGLONG timestampInMilliSeconds,
        OUT bool* eaten);
    virtual void OnMouseOut(
        HWND hWnd,
        WPARAM wParam,
        LPARAM lParam,
        UIPoint clientPoint,
        UIDelta delta,
        ULONGLONG timestampInMilliSeconds,
        OUT bool* eaten);
    // event listeners
    virtual void AddActionListener(ui::event::UIActionListenerFunctor listener) {
        m_actionListeners.AddListener(listener);
    }
    virtual std::wostream& Dump(std::wostream& os) const;
};
std::wostream& operator<< (std::wostream& os, UIButton const& button);
}

#endif // UI_UIBUTTON_H_
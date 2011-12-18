#ifndef UI_UIRADIOBUTTON_H_
#define UI_UIRADIOBUTTON_H_

#include "graphics/GraphicsCommon.h"
#include "graphics/FontAttribute.h"
#include "ui/UICommon.h"
#include "ui/UIBase.h"

namespace ui
{
class UIRadioButton :
    public UIBase,
    public std::enable_shared_from_this<UIRadioButton>
{
private:
    size_t m_index;
    std::weak_ptr<UIRadioButtonGroup> m_radioButtonGroup;
    bool m_selected;
    std::wstring m_text;
    graphics::FontAttribute m_fontAttribute;
    CComPtr<IDWriteTextFormat> m_textFormat;
    CComPtr<IDWriteTextLayout> m_textLayout;
    DWRITE_TEXT_METRICS m_textMetrics;

    std::shared_ptr<graphics::brush::LinearGradientBrush> m_outerShineBrush;
    std::shared_ptr<graphics::brush::LinearGradientBrush> m_innerShadowBrush;
    std::shared_ptr<graphics::brush::LinearGradientBrush> m_innerShineBrush;
    std::shared_ptr<graphics::brush::LinearGradientBrush> m_bodyBrush;
    std::shared_ptr<graphics::brush::LinearGradientBrush> m_selectedMarkBrush;
    std::shared_ptr<graphics::brush::LinearGradientBrush> m_foregroundBrush;

protected:
    virtual void ClearResources();

public:
    UIRadioButton(void);
    virtual ~UIRadioButton(void);
    virtual HRESULT Initialize(
        graphics::D3DInteropHelper *pD3DInteropHelper);
    virtual HRESULT CreateDeviceDependentResources(
        graphics::D3DInteropHelper *pD3DInteropHelper,
        ID2D1RenderTarget *pRenderTarget);
    virtual HRESULT Render(
        graphics::D3DInteropHelper *pD3DInteropHelper,
        ID2D1RenderTarget *pRenderTarget);
    virtual void DiscardDeviceDependentResources();

    virtual void SetIndex(size_t index) {
        m_index = index;
    }
    virtual void SetRadioButtonGroup(std::weak_ptr<UIRadioButtonGroup> radioButtonGroup);
    virtual void SetSelected(bool selected);
    virtual bool IsSelected() const {
        return m_selected;
    }
    virtual void SetText(const std::wstring &text) {
        m_text = text;
    }
    virtual const std::wstring & GetText() const {
        return m_text;
    }
    virtual UISize GetPreferredSize() const;
    // events
    virtual void OnLeftMouseDown(
        HWND hWnd, WPARAM wParam, LPARAM lParam, UIPoint clientPoint, ULONGLONG timestampInMilliSeconds, OUT bool* eaten);
};
}

#endif // UI_UIRADIOBUTTON_H_

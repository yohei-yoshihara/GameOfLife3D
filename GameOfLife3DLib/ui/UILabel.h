#ifndef UI_UILABEL_H_
#define UI_UILABEL_H_

#include "graphics/GraphicsCommon.h"
#include "graphics/FontAttribute.h"
#include "ui/UICommon.h"
#include "ui/UIBase.h"

namespace ui
{
class UILabel :
    public UIBase,
    public std::enable_shared_from_this<UILabel>
{
private:
    std::wstring m_text;
    graphics::FontAttribute m_fontAttribute;
    std::shared_ptr<graphics::brush::LinearGradientBrush> m_foregroundColorBrush;
    CComPtr<IDWriteTextFormat> m_textFormat;
    CComPtr<IDWriteTextLayout> m_textLayout;
    DWRITE_TEXT_METRICS m_textMetrics;

protected:
    virtual void ClearResources();

public:
    UILabel(void);
    virtual ~UILabel(void);
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
    virtual void SetFontAttribute(const graphics::FontAttribute &fontAttribute);
};
}

#endif // UI_UILABEL_H_
#ifndef UI_UILISTBOXITEM_H_
#define UI_UILISTBOXITEM_H_

#include "graphics/GraphicsCommon.h"
#include "graphics/FontAttribute.h"
#include "graphics/Bitmap.h"
#include "ui/UICommon.h"
#include "ui/UIBase.h"

namespace ui {
  class UIListBoxItem : public UIBase, public std::enable_shared_from_this<UIListBoxItem> {
  public:
    enum LAYOUT_DIRECTION { LAYOUT_DIRECTION_HORIZONTAL, LAYOUT_DIRECTION_VERTICAL };

  private:
    ui::UIListBoxItem::LAYOUT_DIRECTION m_layoutDirection;
    UISize m_preferredBitmapSize;
    UISize m_preferredTextAreaSize;
    FLOAT m_marginBetweenBitmapAndText;

    std::shared_ptr<graphics::color::ColorSet> m_textColorSet;
    std::unique_ptr<graphics::brush::BrushSet> m_textBrushSet;

    std::shared_ptr<graphics::color::ColorSet> m_bodyColorSet;
    std::unique_ptr<graphics::brush::BrushSet> m_bodyBrushSet;
    std::unique_ptr<graphics::figure::RectangleFigure> m_bodyRect;

    std::wstring m_mainText;
    graphics::FontAttribute m_mainTextFontAttribute;
    CComPtr<IDWriteTextFormat> m_mainTextFormat;
    CComPtr<IDWriteTextLayout> m_mainTextLayout;
    DWRITE_TEXT_METRICS m_mainTextMetrics;
    UIRectangle m_mainTextLayoutRect;

    std::wstring m_subText1;
    graphics::FontAttribute m_subText1FontAttribute;
    CComPtr<IDWriteTextFormat> m_subText1Format;
    CComPtr<IDWriteTextLayout> m_subText1Layout;
    DWRITE_TEXT_METRICS m_subText1Metrics;
    UIRectangle m_subText1LayoutRect;

    std::wstring m_subText2;
    graphics::FontAttribute m_subText2FontAttribute;
    CComPtr<IDWriteTextFormat> m_subText2Format;
    CComPtr<IDWriteTextLayout> m_subText2Layout;
    DWRITE_TEXT_METRICS m_subText2Metrics;
    UIRectangle m_subText2LayoutRect;

    std::shared_ptr<graphics::Bitmap> m_bitmap;
    UIRectangle m_bitmapLayoutRect;

    UIRectangle m_textLayoutRect;

    bool m_textChanged;
    bool m_resourceChanged;
    size_t m_mainTextNumberOfLines; // number of lines to display text

    std::unique_ptr<ui::UIScrollBar> m_scrollBar;

  protected:
    HRESULT __InitializeMainText(graphics::D3DInteropHelper *pD3DInteropHelper);
    HRESULT __InitializeSubText(graphics::D3DInteropHelper *pD3DInteropHelper, const std::wstring &text,
                                const graphics::FontAttribute &fontAttribute, CComPtr<IDWriteTextFormat> &textFormat,
                                CComPtr<IDWriteTextLayout> &textLayout, DWRITE_TEXT_METRICS &textMetrics,
                                UIRectangle &textRect);
    HRESULT __RecreateAllTextLayout(graphics::D3DInteropHelper *pD3DInteropHelper, ID2D1RenderTarget *pRenderTarget);
    HRESULT __RecreateMainTextLayout(graphics::D3DInteropHelper *pD3DInteropHelper, ID2D1RenderTarget *pRenderTarget,
                                     const std::wstring &text, CComPtr<IDWriteTextLayout> &textLayout,
                                     DWRITE_TEXT_METRICS &textMetrics, UIRectangle &textRect, size_t numberOfLines);
    HRESULT __RecreateSubTextLayout(graphics::D3DInteropHelper *pD3DInteropHelper, ID2D1RenderTarget *pRenderTarget,
                                    const std::wstring &text, CComPtr<IDWriteTextLayout> &textLayout,
                                    DWRITE_TEXT_METRICS &textMetrics, UIRectangle &textRect);
    void __CalculateLayoutRect();

  public:
    UIListBoxItem(void);
    virtual ~UIListBoxItem(void);
    virtual HRESULT Initialize(graphics::D3DInteropHelper *pD3DInteropHelper);
    virtual HRESULT CreateDeviceDependentResources(graphics::D3DInteropHelper *pD3DInteropHelper,
                                                   ID2D1RenderTarget *pRenderTarget);
    virtual HRESULT Render(graphics::D3DInteropHelper *pD3DInteropHelper, ID2D1RenderTarget *pRenderTarget);
    virtual void DiscardDeviceDependentResources();

    void SetLayoutDirection(ui::UIListBoxItem::LAYOUT_DIRECTION layout) {
      m_layoutDirection = layout;
    }
    ui::UIListBoxItem::LAYOUT_DIRECTION GetLayout() const {
      return m_layoutDirection;
    }
    void SetMainText(const std::wstring &text);
    const std::wstring &GetMainText() const {
      return m_mainText;
    }
    void SetSubText1(const std::wstring &subText1);
    const std::wstring &GetSubText1() const {
      return m_subText1;
    }
    void SetSubText2(const std::wstring &subText2);
    const std::wstring &GetSubText2() const {
      return m_subText2;
    }
    void SetBitmapFileName(LPCTSTR fileName);
    void SetBitmapResource(LPCTSTR resourceName, LPCTSTR resourceType);
    void SetPreferredBitmapSize(UISize size) {
      m_preferredBitmapSize = size;
      m_bitmapLayoutRect.x = 0.0f;
      m_bitmapLayoutRect.y = 0.0f;
      m_bitmapLayoutRect.width = size.width;
      m_bitmapLayoutRect.height = size.height;
    }
    void SetPreferredTextAreaSize(UISize textAreaSize) {
      m_preferredTextAreaSize = textAreaSize;
      m_textLayoutRect.x = 0.0f;
      m_textLayoutRect.y = 0.0f;
      m_textLayoutRect.width = textAreaSize.width;
      m_textLayoutRect.height = textAreaSize.height;
    }
    void SetNumberOfMainTextLines(size_t numberOfLines) {
      m_mainTextNumberOfLines = numberOfLines;
    }
  };
}

#endif // UI_UILISTBOXITEM_H_
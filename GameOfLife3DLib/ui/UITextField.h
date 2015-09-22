#ifndef UI_UITEXTFIELD_H_
#define UI_UITEXTFIELD_H_

#include "graphics/GraphicsCommon.h"
#include "graphics/FontAttribute.h"
#include "ui/UICommon.h"
#include "ui/UIBase.h"
#include "ui/UITextEditSink.h"
#include "ui/UITextStoreACP.h"
#include "tsf/TSFManager.h"

namespace ui {
#define UI_UITEXTFIELD_SOFTWARE_CARET

  class CompositionAttribute {
  public:
    LONG startPos;
    LONG endPos;
    TF_DISPLAYATTRIBUTE displayAttribute;
  };

  class UITextField : public UIBase, public IUITextField, public std::enable_shared_from_this<UITextField> {
  public:
    static const FLOAT INNER_MARGIN;

  private:
    // set true when Initialize finished
    bool m_initialized;
    // set true when CreateDeviceDependentResources finished
    bool m_resourceCreated;
    // the horizontal position of the display area
    // (distance from the left position of the text)
    FLOAT m_textDisplayAreaHPos;
    // the size of the text (obtained from text metrics)
    UISize m_textSize;
    // the font attribute of the text
    graphics::FontAttribute m_fontAttribute;
    // if SetText is invoked before an initialization, this variable store a given text
    std::wstring m_presetText;
    // set true if text has changed and need to recreate the text format and the text layout
    bool m_textChanged;

    // TSF related members
    CComPtr<UITextStoreACP> m_textStoreACP;
    CComPtr<UITextEditSink> m_textEditSink;
    std::shared_ptr<tsf::TSFManager> m_tsfManager;
    CComPtr<ITfDocumentMgr> m_pDocMgr;
    CComPtr<ITfContext> m_pContext;
    TfEditCookie m_editCookie;

    // colors
    D2D1_COLOR_F m_foregroundColor;
    D2D1_COLOR_F m_backgroundColor;
    D2D1_COLOR_F m_borderLineColor;
    D2D1_COLOR_F m_focusBorderLineColor;
    D2D1_COLOR_F m_highlightColor;
    D2D1_COLOR_F m_compositionUnderlineColor;

    CComPtr<ID2D1SolidColorBrush> m_foregroundColorBrush;
    CComPtr<ID2D1SolidColorBrush> m_backgroundColorBrush;
    CComPtr<ID2D1SolidColorBrush> m_borderLineColorBrush;
    CComPtr<ID2D1SolidColorBrush> m_focusBorderLineColorBrush;
    CComPtr<ID2D1SolidColorBrush> m_highlightColorBrush;
    CComPtr<ID2D1SolidColorBrush> m_compositionUnderlineBrush;

    CComPtr<ID2D1StrokeStyle> m_compositionSolidUnderlineStrokeStyle;
    CComPtr<ID2D1StrokeStyle> m_compositionDotUnderlineStrokeStyle;
    CComPtr<ID2D1StrokeStyle> m_compositionDashUnderlineStrokeStyle;
    CComPtr<ID2D1StrokeStyle> m_compositionSquiggleUnderlineStrokeStyle;

    CComPtr<IDWriteTextFormat> m_textFormat;
    CComPtr<IDWriteTextLayout> m_textLayout;
    DWRITE_TEXT_METRICS m_textMetrics;

    std::vector<CompositionAttribute> m_compositionAttributes;

    ui::UIPoint m_caretPosInTextCoordinate;
    FLOAT m_caretHeight;

  public:
    UITextField(void);
    virtual ~UITextField(void);
    virtual void SetTSFManager(const std::shared_ptr<tsf::TSFManager> &tsfManager) {
      m_tsfManager = tsfManager;
    }
    virtual HRESULT Initialize(graphics::D3DInteropHelper *pD3DInteropHelper);
    virtual HRESULT CreateDeviceIndependentResources(graphics::D3DInteropHelper *pD3DInteropHelper);
    virtual HRESULT CreateDeviceDependentResources(graphics::D3DInteropHelper *pD3DInteropHelper,
                                                   ID2D1RenderTarget *pRenderTarget);
    virtual HRESULT Render(graphics::D3DInteropHelper *pD3DInteropHelper, ID2D1RenderTarget *pRenderTarget);
    virtual void DiscardDeviceDependentResources();

    virtual void SetText(const std::wstring &text);
    virtual const std::wstring &GetText() const;
    virtual void SetFontAttribute(const graphics::FontAttribute &fontAttribute) {
      m_fontAttribute = fontAttribute;
    }
    virtual UIRectangle GetTextDisplayArea() const {
      return UIRectangle(GetX() + INNER_MARGIN, GetY() + INNER_MARGIN, GetWidth() - INNER_MARGIN * 2.0f,
                         GetHeight() - INNER_MARGIN * 2.0f);
    }
    virtual UIPoint GetTextDisplayAreaStartPoint() const {
      return UIPoint(GetX() + INNER_MARGIN, GetY() + INNER_MARGIN);
    }
    virtual UISize GetTextDisplayAreaSize() const {
      return UISize(GetWidth() - INNER_MARGIN * 2.0f, GetHeight() - INNER_MARGIN * 2.0f);
    }
    virtual HRESULT UpdateDisplayAreaPosition();

    // focus
    virtual void _SetFocus();
    virtual void _KillFocus();
    virtual void RequestFocus();
    virtual void LostFocus();

    // caret
    virtual void UpdateCaret();

    // IUITextField (start)
    virtual HWND GetHWnd() const;
    virtual ITfContext *GetContext() const {
      return m_pContext;
    }
    virtual LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
    virtual HRESULT GetTextExt(LONG acpStart, LONG acpEnd, RECT *prc, BOOL *pfClipped);
    virtual void NotifySelectionHasChanged();
    virtual void NotifyTextReplace(LONG oldTotalLength, LONG startPos, LONG replacedTextLength,
                                   LONG textToInsertLength);
    virtual void NotifyTextInsert(LONG oldTotalLength, LONG startPos, LONG textToInsertLength);
    virtual void NotifyTextRemove(LONG oldTotalLength, LONG startPos, LONG removedTextLength);
    virtual void AddCompositionRenderInfo(LONG startPos, LONG endPos, const TF_DISPLAYATTRIBUTE *pDisplayAttribute);
    virtual void NotifyCompositionRenderInfoChange();
    virtual void ClearCompositionRenderInfo();
    virtual void CalculateSelectionFromDragRect(FLOAT startX, FLOAT startY, FLOAT endX, FLOAT endY, OUT bool *inside,
                                                OUT LONG *acpStart, OUT LONG *acpEnd, OUT TsActiveSelEnd *activeSelEnd);
    // IUITextField (end)

    HRESULT _RecreateTextLayout(graphics::D3DInteropHelper *pD3DInteropHelper, ID2D1RenderTarget *pRenderTarget);
    void _RenderBackground(graphics::D3DInteropHelper *pD3DInteropHelper, ID2D1RenderTarget *pRenderTarget);
    void _RenderBox(graphics::D3DInteropHelper *pD3DInteropHelper, ID2D1RenderTarget *pRenderTarget);
    HRESULT _RenderText(graphics::D3DInteropHelper *pD3DInteropHelper, ID2D1RenderTarget *pRenderTarget,
                        UIRectangle textRect);
    void _RenderHighlight(graphics::D3DInteropHelper *pD3DInteropHelper, ID2D1RenderTarget *pRenderTarget,
                          UIRectangle textRect);
    HRESULT _CalculateCaretSizeAndPosition(OUT UIPoint &caretPosInTextCoordinate,
                                           OUT DWRITE_HIT_TEST_METRICS *pHitTestMetrics);
    HRESULT _CalculateTextSize(OUT UISize &textSize);

    // events (start)
    virtual void OnChar(HWND hWnd, WPARAM wParam, LPARAM lParam, CHAR_TYPE charType, wchar_t c,
                        ULONGLONG timestampInMilliSeconds, OUT bool *eaten);
    virtual void OnLeftMouseDown(HWND hWnd, WPARAM wParam, LPARAM lParam, UIPoint clientPoint,
                                 ULONGLONG timestampInMilliSeconds, OUT bool *eaten);
    virtual void OnDraggingStart(HWND hWnd, WPARAM wParam, LPARAM lParam, UI2Points clientDragRect, UIDelta delta,
                                 ULONGLONG timestampInMilliSeconds, OUT bool *eaten);
    virtual void OnDragging(HWND hWnd, WPARAM wParam, LPARAM lParam, UI2Points clientDragRect, UIDelta delta,
                            ULONGLONG timestampInMilliSeconds, OUT bool *eaten);
    virtual void OnDraggingEnd(HWND hWnd, WPARAM wParam, LPARAM lParam, UI2Points clientDragRect, UIDelta delta,
                               ULONGLONG timestampInMilliSeconds, OUT bool *eaten);
    virtual void OnSetFocus(HWND hWnd, WPARAM wParam, LPARAM lParam, ULONGLONG timestampInMilliSeconds,
                            OUT bool *eaten);
    virtual void OnKillFocus(HWND hWnd, WPARAM wParam, LPARAM lParam, ULONGLONG timestampInMilliSeconds,
                             OUT bool *eaten);
    // events (end)
  };
}

#endif // UI_UITEXTFIELD_H_
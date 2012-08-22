#ifndef UI_UIINTERNALDIALOG_H_
#define UI_UIINTERNALDIALOG_H_

#include "graphics/GraphicsCommon.h"
#include "graphics/FontAttribute.h"
#include "ui/UICommon.h"
#include "ui/UIBase.h"
#include "ui/UIContainer.h"
#include "ui/IUIDialog.h"
#include "ui/event/UIActionEvent.h"

namespace ui
{
class UIInternalDialog :
    public IUIDialog,
    public std::enable_shared_from_this<UIInternalDialog>
{
public:
    static void CloseDialog(ui::event::UIActionEvent event);
private:
    DIALOG_STATUS m_dialogStatus;
    DIALOG_RESULT m_dialogResult;
    std::wstring m_title;
    graphics::FontAttribute m_fontAttribute;
    std::vector<BeforeDialogDisplayedCallback> m_beforeDisplayCallbacks;
    std::vector<DialogClosedCallback>  m_dialogClosedCallbacks;

    std::shared_ptr<graphics::brush::LinearGradientBrush> m_outerShineBrush;
    std::shared_ptr<graphics::brush::LinearGradientBrush> m_innerShadowBrush;
    std::shared_ptr<graphics::brush::LinearGradientBrush> m_backgroundBrush;
    std::shared_ptr<graphics::brush::LinearGradientBrush> m_titleForegroundBrush;
    std::shared_ptr<graphics::brush::LinearGradientBrush> m_titleBackgroundBrush;

    CComPtr<IDWriteTextFormat> m_textFormat;
    CComPtr<IDWriteTextLayout> m_textLayout;
    DWRITE_TEXT_METRICS m_textMetrics;

public:
    UIInternalDialog(void);
    virtual ~UIInternalDialog(void);
    virtual HRESULT Initialize(
        graphics::D3DInteropHelper *pD3DInteropHelper);
    virtual HRESULT CreateDeviceDependentResources(
        graphics::D3DInteropHelper *pD3DInteropHelper,
        ID2D1RenderTarget *pRenderTarget);
    virtual HRESULT Render(
        graphics::D3DInteropHelper *pD3DInteropHelper,
        ID2D1RenderTarget *pRenderTarget);
    virtual void DiscardDeviceDependentResources();

    virtual void SetTitle(const std::wstring &title) {
        m_title = title;
    }
    virtual const std::wstring & GetTitle() const {
        return m_title;
    }
    virtual void ShowDialog() {
        FireBeforeDialogDisplayedCallbacks();
        m_dialogStatus = DIALOG_STATUS_DISPLAY;
    }
    virtual void CloseDialog();
    virtual DIALOG_STATUS GetDialogStatus() const {
        return m_dialogStatus;
    }
    virtual DIALOG_RESULT GetDialogResult() const {
        return m_dialogResult;
    }
    virtual void NotifyFocusObtained(const std::shared_ptr<UIBase> &uiBase);

    virtual void AddBeforeDialogDisplayedCallback(BeforeDialogDisplayedCallback callback) {
        m_beforeDisplayCallbacks.push_back(callback);
    }
    virtual void FireBeforeDialogDisplayedCallbacks() {
        for (auto it = m_beforeDisplayCallbacks.begin(); it != m_beforeDisplayCallbacks.end(); ++it) {
            (*it)(shared_from_this());
        }
    }
    virtual void ClearBeforeDialogDisplayedCallbacks() {
        m_beforeDisplayCallbacks.clear();
    }

    virtual void AddDialogClosedCallback(DialogClosedCallback callback) {
        m_dialogClosedCallbacks.push_back(callback);
    }
    virtual void FireDialogClosedCallbacks() {
        for (auto it = m_dialogClosedCallbacks.begin(); it != m_dialogClosedCallbacks.end(); ++it) {
            (*it)(shared_from_this());
        }
    }
    virtual void ClearDialogClosedCallbacks() {
        m_dialogClosedCallbacks.clear();
    }
};
}

#endif // UI_UIINTERNALDIALOG_H_
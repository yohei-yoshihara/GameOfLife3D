#ifndef UI_UIPANEL_H_
#define UI_UIPANEL_H_

#include "graphics/GraphicsCommon.h"
#include "ui/UICommon.h"
#include "ui/UIBase.h"
#include "ui/UIContainer.h"

namespace ui {
  class UIPanel : public UIContainer, public std::enable_shared_from_this<UIPanel> {
  private:
  public:
    UIPanel(void);
    virtual ~UIPanel(void);
    static std::shared_ptr<UIPanel> Create() {
      return std::make_shared<UIPanel>();
    }
    virtual void NotifyFocusObtained(const std::shared_ptr<UIBase> &uiBase);
    virtual HRESULT Render(graphics::D3DInteropHelper *pD3DInteropHelper, ID2D1RenderTarget *pRenderTarget);
  };
}

#endif // UI_UIPANEL_H_
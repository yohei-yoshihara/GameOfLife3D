#ifndef UI_UIRADIOBUTTONGROUP_H_
#define UI_UIRADIOBUTTONGROUP_H_

#include "graphics/GraphicsCommon.h"
#include "ui/UICommon.h"
#include "ui/UIBase.h"
#include "ui/UIContainer.h"
#include "ui/UIRadioButton.h"

namespace ui {
  class UIRadioButtonGroup : public UIContainer, public std::enable_shared_from_this<UIRadioButtonGroup> {
  public:
    UIRadioButtonGroup(void);
    virtual ~UIRadioButtonGroup(void);
    virtual HRESULT CreateDeviceDependentResources(graphics::D3DInteropHelper *pD3DInteropHelper,
                                                   ID2D1RenderTarget *pRenderTarget);
    virtual HRESULT Render(graphics::D3DInteropHelper *pD3DInteropHelper, ID2D1RenderTarget *pRenderTarget);
    virtual void NotifyFocusObtained(const std::shared_ptr<UIBase> &uiBase);
    virtual void NotifySelectionChange(size_t index) {
      for (size_t i = 0; i < GetNumberOfElements(); ++i) {
        if (i != index) {
          std::shared_ptr<UIRadioButton> element = std::dynamic_pointer_cast<UIRadioButton>(GetElement(i));
          element->SetSelected(false);
        }
      }
    }
  };
}

#endif // UI_UIRADIOBUTTONGROUP_H_
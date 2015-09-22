#include "StdAfx.h"
#include "ui/UIPanel.h"

using namespace ui;

ui::UIPanel::UIPanel(void) {
  AddBeforeAddElementCallback([&](std::shared_ptr<UIBase> element) -> bool {
    element->SetParentContainer(shared_from_this());
    return true;
  });
  SetFocusable(false);
}

ui::UIPanel::~UIPanel(void) {
}

HRESULT ui::UIPanel::Render(graphics::D3DInteropHelper *pD3DInteropHelper, ID2D1RenderTarget *pRenderTarget) {
  UNREFERENCED_PARAMETER(pD3DInteropHelper);
  UNREFERENCED_PARAMETER(pRenderTarget);
  return S_OK;
}

void ui::UIPanel::NotifyFocusObtained(const std::shared_ptr<UIBase> &uiBase) {
  for (size_t i = 0; i < GetNumberOfElements(); ++i) {
    auto child = GetElement(i);
    if (uiBase != child) {
      child->LostFocus();
    }
  }
  if (!GetParentContainer().expired()) {
    GetParentContainer().lock()->NotifyFocusObtained(shared_from_this());
  }
}

#include "StdAfx.h"
#include "Common.h"
#include "ui/UIRadioButtonGroup.h"

// #define DEBUG_UIRADIOBUTTONGROUP

const FLOAT MARGIN_BUTTONS = 4.0f;

ui::UIRadioButtonGroup::UIRadioButtonGroup(void) {
  AddBeforeAddElementCallback([&](std::shared_ptr<UIBase> element) -> bool {
    auto radioButton = std::dynamic_pointer_cast<UIRadioButton>(element);
    if (radioButton != nullptr) {
      radioButton->SetIndex(GetNumberOfElements());
      radioButton->SetRadioButtonGroup(shared_from_this());
      return true;
    } else {
      return false;
    }
  });
  SetFocusable(true);
}

ui::UIRadioButtonGroup::~UIRadioButtonGroup(void) {
}

HRESULT ui::UIRadioButtonGroup::CreateDeviceDependentResources(graphics::D3DInteropHelper *pD3DInteropHelper,
                                                               ID2D1RenderTarget *pRenderTarget) {
  CHK_WARN_HRESULT(UIContainer::CreateDeviceDependentResources(pD3DInteropHelper, pRenderTarget));
  FLOAT y = 0.0f;
  for (size_t i = 0; i < GetNumberOfElements(); ++i) {
    auto radioButton = std::dynamic_pointer_cast<UIRadioButton>(GetElement(i));
    UISize size = radioButton->GetPreferredSize();
#ifdef DEBUG_UIRADIOBUTTONGROUP
    SPDLOG_DEBUG( << L"preferred size = " << size;
#endif
    radioButton->SetBounds(0.0f, y, GetWidth(), size.height);
    y += radioButton->GetHeight() + MARGIN_BUTTONS;
  }
  ui::UIRectangle bounds = GetBounds();
  if (bounds.height < y) {
    bounds.height = y;
    SetBounds(bounds);
  }
  return S_OK;
}

HRESULT ui::UIRadioButtonGroup::Render(graphics::D3DInteropHelper *pD3DInteropHelper,
                                       ID2D1RenderTarget *pRenderTarget) {
  for (size_t i = 0; i < GetNumberOfElements(); ++i) {
    auto element = std::dynamic_pointer_cast<UIRadioButton>(GetElement(i));
    D2D1_MATRIX_3X2_F origMatrix;
    pRenderTarget->GetTransform(&origMatrix);
    D2D1_MATRIX_3X2_F matrix = origMatrix * D2D1::Matrix3x2F::Translation(element->GetX(), element->GetY());
    pRenderTarget->SetTransform(matrix);
    HRESULT hr = element->Render(pD3DInteropHelper, pRenderTarget);
    if (FAILED(hr)) {
      SPDLOG_ERROR(L"render a child element failed, index = {}, hr = {:x}", i, hr);
    }
    pRenderTarget->SetTransform(origMatrix);
  }
  return S_OK;
}

void ui::UIRadioButtonGroup::NotifyFocusObtained(const std::shared_ptr<UIBase> &uiBase) {
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

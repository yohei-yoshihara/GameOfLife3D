#include "Stdafx.h"
#include "Common.h"
#include "graphics/color/SolidColor.h"
#include "graphics/color/LinearGradientColor.h"
#include "graphics/color/RadialGradientColor.h"
#include "graphics/brush/SolidColorBrush.h"
#include "graphics/brush/LinearGradientBrush.h"
#include "graphics/brush/RadialGradientBrush.h"
#include "graphics/brush/BrushSet.h"

// #define DEBUG_BRUSHSET

void graphics::brush::BrushSet::SetColorSet(const graphics::color::ColorSet &colorSet) {
  Clear();

  for (size_t i = 0; i < colorSet.GetNumberOfColors(); ++i) {
    auto color = colorSet.GetColor(i);
    switch (color->GetColorType()) {
    case graphics::color::IColor::COLOR_TYPE_SOLID_COLOR: {
      auto solidColor = std::dynamic_pointer_cast<graphics::color::SolidColor>(color);
      auto solidBrush = std::make_shared<graphics::brush::SolidColorBrush>();
      solidBrush->SetColor(solidColor->GetCurrentColor());
      if (solidColor->GetLabel().length() > 0) {
        solidBrush->SetLabel(solidColor->GetLabel());
        m_labelToBrashMap.insert({solidColor->GetLabel(), solidBrush});
      }
      m_brushes.push_back(solidBrush);
    } break;
    case graphics::color::IColor::COLOR_TYPE_LINEAR_GRADIENT_COLOR: {
      auto linearGradientColor = std::dynamic_pointer_cast<graphics::color::LinearGradientColor>(color);
      auto linearBrush = std::make_shared<graphics::brush::LinearGradientBrush>();
      linearBrush->SetGradientDirection(linearGradientColor->GetGradientDirection());
      linearBrush->SetGradientStops(linearGradientColor->GetCurrentColor());
      if (linearGradientColor->GetLabel().length() > 0) {
        linearBrush->SetLabel(linearGradientColor->GetLabel());
        m_labelToBrashMap.insert({linearGradientColor->GetLabel(), linearBrush});
      }
      m_brushes.push_back(linearBrush);
    } break;
    case graphics::color::IColor::COLOR_TYPE_RADIAL_GRADIENT_COLOR: {
      auto radialGradientColor = std::dynamic_pointer_cast<graphics::color::RadialGradientColor>(color);
      auto radialBrush = std::make_shared<graphics::brush::RadialGradientBrush>();
      radialBrush->SetGradientOriginOffsetRate(radialGradientColor->GetGradientOffsetX(),
                                               radialGradientColor->GetGradientOffsetY());
      radialBrush->SetGradientStops(radialGradientColor->GetCurrentColor());
      if (radialGradientColor->GetLabel().length() > 0) {
        radialBrush->SetLabel(radialGradientColor->GetLabel());
        m_labelToBrashMap.insert({radialGradientColor->GetLabel(), radialBrush});
      }
      m_brushes.push_back(radialBrush);
    } break;
    }
  }
}

HRESULT graphics::brush::BrushSet::CreateDeviceDependentResource(ID2D1RenderTarget *pRenderTarget,
                                                                 const D2D1_RECT_F &rect, FLOAT opacity) {
#ifdef DEBUG_BRUSHSET
  LOG_ENTER(SEVERITY_LEVEL_DEBUG);
#endif
  HRESULT hr = S_OK;
  for (size_t i = 0; i < m_brushes.size(); ++i) {
    CHK_FATAL_HRESULT(m_brushes[i]->CreateDeviceDependentResource(pRenderTarget, rect, opacity));
#ifdef DEBUG_BRUSHSET
    SPDLOG_DEBUG( << L"raw brush[" << i << L"] = " << std::hex << m_brushes[i]->GetBrush() << std::dec;
#endif
  }
  m_deviceDependentResourceCreated = SUCCEEDED(hr);
#ifdef DEBUG_BRUSHSET
  LOG_LEAVE(SEVERITY_LEVEL_DEBUG);
#endif
  return hr;
}

void graphics::brush::BrushSet::DiscardDeviceDependentResource() {
  for (size_t i = 0; i < m_brushes.size(); ++i) {
    m_brushes[i]->DiscardDeviceDependentResource();
  }
  m_deviceDependentResourceCreated = false;
}

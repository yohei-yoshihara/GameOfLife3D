#include "StdAfx.h"
#include "Common.h"
#include "graphics/GraphicsCommon.h"
#include "graphics/brush/SolidColorBrush.h"

graphics::brush::SolidColorBrush::SolidColorBrush(void)
    : m_label()
    , m_color(D2D1::ColorF(D2D1::ColorF::Black))
    , m_brush(nullptr)
    , m_deviceDependentResourceCreated(false) {
}

graphics::brush::SolidColorBrush::SolidColorBrush(D2D1_COLOR_F color)
    : m_label()
    , m_color(color)
    , m_brush(nullptr)
    , m_deviceDependentResourceCreated(false) {
}

graphics::brush::SolidColorBrush::SolidColorBrush(const graphics::color::ColorValue &color)
    : m_label()
    , m_color(color.ToD2D1_COLOR_F())
    , m_brush(nullptr)
    , m_deviceDependentResourceCreated(false) {
}

graphics::brush::SolidColorBrush::~SolidColorBrush(void) {
}

HRESULT graphics::brush::SolidColorBrush::CreateDeviceDependentResource(ID2D1RenderTarget *pRenderTarget,
                                                                        const D2D1_RECT_F &rect, FLOAT opacity) {
  UNREFERENCED_PARAMETER(rect);
  m_brush = nullptr;
  CHK_FATAL_HRESULT(pRenderTarget->CreateSolidColorBrush(m_color, D2D1::BrushProperties(opacity), &m_brush));
  m_deviceDependentResourceCreated = (m_brush != nullptr);
  return S_OK;
}

void graphics::brush::SolidColorBrush::DiscardDeviceDependentResource() {
  m_brush = nullptr;
  m_deviceDependentResourceCreated = false;
}

void graphics::brush::SolidColorBrush::Clear() {
  DiscardDeviceDependentResource();
  m_color = D2D1::ColorF(0);
}

ID2D1Brush *graphics::brush::SolidColorBrush::GetBrush(const D2D1_RECT_F &rect, FLOAT opacity) {
  UNREFERENCED_PARAMETER(rect);
  if (m_brush != nullptr) {
    m_brush->SetOpacity(opacity);
    return m_brush;
  } else {
    throw std::runtime_error("brush has not been created");
  }
}

ID2D1Brush *graphics::brush::SolidColorBrush::GetBrush(FLOAT opacity) {
  if (m_brush != nullptr) {
    m_brush->SetOpacity(opacity);
    return m_brush;
  } else {
    throw std::runtime_error("brush has not been created");
  }
}

ID2D1Brush *graphics::brush::SolidColorBrush::GetBrush() {
  if (m_brush != nullptr) {
    return m_brush;
  } else {
    throw std::runtime_error("brush has not been created");
  }
}

#include "StdAfx.h"
#include "Common.h"
#include "LinearGradientBrush.h"

graphics::brush::LinearGradientBrush::LinearGradientBrush(void)
    : m_label()
    , m_gradientStops()
    , m_gradientDirection(GRADIENT_DIRECTION_TOP_BOTTOM)
    , m_brush()
    , m_deviceDependentResourceCreated(false) {
}

graphics::brush::LinearGradientBrush::~LinearGradientBrush(void) {
}

void graphics::brush::LinearGradientBrush::_CreateStartAndEndPoint(IN FLOAT width, IN FLOAT height,
                                                                   OUT D2D1_POINT_2F &startPoint,
                                                                   OUT D2D1_POINT_2F &endPoint) {
  switch (m_gradientDirection) {
  case GRADIENT_DIRECTION_TOP_BOTTOM:
    startPoint = D2D1::Point2F(0, 0);
    endPoint = D2D1::Point2F(0, height);
    break;
  case GRADIENT_DIRECTION_TOPLEFT_TO_BOTTOMRIGHT:
    startPoint = D2D1::Point2F(0, 0);
    endPoint = D2D1::Point2F(width, height);
    break;
  default:
    startPoint = D2D1::Point2F(0, 0);
    endPoint = D2D1::Point2F(0, height);
    break;
  }
}

HRESULT graphics::brush::LinearGradientBrush::CreateDeviceDependentResource(ID2D1RenderTarget *pRenderTarget,
                                                                            const D2D1_RECT_F &rect, FLOAT opacity) {
  HRESULT hr = S_OK;
  CComPtr<ID2D1GradientStopCollection> gradientStopCollection;
  CHK_FATAL_HRESULT(
      pRenderTarget->CreateGradientStopCollection(m_gradientStops.data(), static_cast<UINT>(m_gradientStops.size()),
                                                  D2D1_GAMMA_2_2, D2D1_EXTEND_MODE_CLAMP, &gradientStopCollection));
  D2D1_POINT_2F startPoint;
  D2D1_POINT_2F endPoint;
  _CreateStartAndEndPoint(rect.right - rect.left, rect.bottom - rect.top, startPoint, endPoint);
  m_brush = nullptr;
  CHK_FATAL_HRESULT(pRenderTarget->CreateLinearGradientBrush(
      D2D1::LinearGradientBrushProperties(startPoint, endPoint),
      D2D1::BrushProperties(opacity, D2D1::Matrix3x2F::Translation(rect.left, rect.top)), gradientStopCollection,
      &m_brush));
  m_deviceDependentResourceCreated = (m_brush != nullptr);
  return hr;
}

ID2D1Brush *graphics::brush::LinearGradientBrush::GetBrush(const D2D1_RECT_F &rect, FLOAT opacity) {
  if (m_brush != nullptr) {
    m_brush->SetTransform(D2D1::Matrix3x2F::Translation(rect.left, rect.top));
    D2D1_POINT_2F startPoint;
    D2D1_POINT_2F endPoint;
    _CreateStartAndEndPoint(rect.right - rect.left, rect.bottom - rect.top, startPoint, endPoint);
    m_brush->SetStartPoint(startPoint);
    m_brush->SetEndPoint(endPoint);
    m_brush->SetOpacity(opacity);
    return m_brush;
  } else {
    throw std::runtime_error("brush has not been created");
  }
}

ID2D1Brush *graphics::brush::LinearGradientBrush::GetBrush(FLOAT opacity) {
  if (m_brush != nullptr) {
    m_brush->SetOpacity(opacity);
    return m_brush;
  } else {
    throw std::runtime_error("brush has not been created");
  }
}

ID2D1Brush *graphics::brush::LinearGradientBrush::GetBrush() {
  if (m_brush != nullptr) {
    return m_brush;
  } else {
    throw std::runtime_error("brush has not been created");
  }
}

void graphics::brush::LinearGradientBrush::DiscardDeviceDependentResource() {
  m_brush = nullptr;
  m_deviceDependentResourceCreated = false;
}

void graphics::brush::LinearGradientBrush::Clear() {
  DiscardDeviceDependentResource();
  m_gradientStops.clear();
}

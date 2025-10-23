﻿#include "stdafx.h"
#include "Common.h"
#include "graphics/GraphicsCommon.h"
#include "graphics/D3DInteropHelper.h"
#include "graphics/brush/BrushSet.h"
#include "graphics/brush/BrushFactory.h"
#include "graphics/brush/SolidColorBrush.h"
#include "graphics/brush/LinearGradientBrush.h"
#include "graphics/brush/RadialGradientBrush.h"
#include "graphics/color/ColorSet.h"
#include "graphics/color/ColorPatternUtil.h"
#include "graphics/color/ColorValue.h"
#include "graphics/color/SolidColor.h"
#include "graphics/filter/GaussFilter.h"
#include "graphics/figure/TriangleFigure.h"

// #define DEBUG_TRIANGLE_FIGURE

graphics::figure::TriangleFigure::TriangleFigure(void)
    : m_propertyChanged(false)
    , m_direction(TRIANGLE_DIRECTION_LEFT)
    , m_x(0.0f)
    , m_y(0.0f)
    , m_width(0.0f)
    , m_height(0.0f)
    , m_color(graphics::color::ColorValue::COLOR_TYPE_HSBA, 0.0f, 0.0f, 0.5f, 1.0f)
    , m_pGeometry(nullptr)
    , m_shadow(false)
    , m_frames()
    , m_brushes(std::make_unique<graphics::brush::BrushSet>())
    , m_shadowColor(std::make_shared<graphics::color::SolidColor>(
          graphics::color::ColorValue(graphics::color::ColorValue::COLOR_TYPE_RGBA, 1.0f, 1.0f, 1.0f, 1.0f)))
    , m_decorators()
    , m_geometryFilter() {
#ifdef DEBUG_TRIANGLE_FIGURE
  LOG_ENTER(SEVERITY_LEVEL_DEBUG);
#endif
#ifdef DEBUG_TRIANGLE_FIGURE
  LOG_LEAVE(SEVERITY_LEVEL_DEBUG);
#endif
}

graphics::figure::TriangleFigure::~TriangleFigure(void) {
#ifdef DEBUG_TRIANGLE_FIGURE
  LOG_ENTER(SEVERITY_LEVEL_DEBUG);
  LOG_LEAVE(SEVERITY_LEVEL_DEBUG);
#endif
}

HRESULT graphics::figure::TriangleFigure::Initialize(graphics::D3DInteropHelper *pD3DInteropHelper) {
#ifdef DEBUG_TRIANGLE_FIGURE
  LOG_ENTER(SEVERITY_LEVEL_DEBUG);
#endif
  HRESULT hr = S_OK;

  if (m_brushes->GetNumberOfBrushes() == 0) {
    SetColor(m_color, graphics::color::COLOR_PATTERN_CONVEX);
  }

  // ジオメトリの初期化
  m_pGeometry = nullptr;
  CHK_FATAL_HRESULT(_CreateTriangleGeometry(pD3DInteropHelper->GetD2DFactory(),
                                            D2D1::RectF(0.0f, 0.0f, m_width, m_height), m_direction, &m_pGeometry));

  // フレームの初期化
  for (size_t i = 0; i < m_brushes->GetNumberOfBrushes(); ++i) {
    FLOAT offset = static_cast<FLOAT>(i);
    m_frames[i] = nullptr;
    CHK_FATAL_HRESULT(_CreateTriangleGeometry(pD3DInteropHelper->GetD2DFactory(),
                                              D2D1::RectF(offset, offset, m_width - offset, m_height - offset),
                                              m_direction, &m_frames[i]));
  }

  // デコレータの初期化
  for (auto it = m_decorators.begin(); it != m_decorators.end(); ++it) {
    CHK_FATAL_HRESULT((*it)->Initialize(pD3DInteropHelper, m_pGeometry));
  }

  // シャドウの初期化
  if (m_shadow && !HasGeometryFilter(L"#shadow")) {
    AddGeometryFilter(L"#shadow", L"GaussFilter", FILTER_DISPLAY_ORDER_BEFORE_FIGURE, m_shadowColor);
  }

  // ジオメトリフィルターの初期化
  for (auto it = m_geometryFilter.begin(); it != m_geometryFilter.end(); ++it) {
    graphics::figure::GeometryFilterWorkSet &geometryFilterWorkSet = *it;
    auto filter = pD3DInteropHelper->GetGeometryFilter(geometryFilterWorkSet.GetFilterName());
#ifdef DEBUG_TRIANGLE_FIGURE
    SPDLOG_DEBUG( << L"filterName = " << geometryFilterWorkSet.GetFilterName() << L", filter = " << std::hex
                              << filter.get() << std::dec;
#endif
    if (filter.get() != nullptr) {
      geometryFilterWorkSet.SetGeometryFilter(filter);
    }
  }

#ifdef DEBUG_TRIANGLE_FIGURE
  LOG_LEAVE(SEVERITY_LEVEL_DEBUG);
#endif
  return hr;
}

HRESULT graphics::figure::TriangleFigure::CreateDeviceDependentResources(graphics::D3DInteropHelper *pD3DInteropHelper,
                                                                         ID2D1RenderTarget *pRenderTarget) {
#ifdef DEBUG_TRIANGLE_FIGURE
  LOG_ENTER(SEVERITY_LEVEL_DEBUG);
#endif
  HRESULT hr = S_OK;
  m_brushes->CreateDeviceDependentResource(pRenderTarget, D2D1::RectF(0.0f, 0.0f, GetWidth(), GetHeight()), 1.0f);

  for (auto it = m_geometryFilter.begin(); it != m_geometryFilter.end(); ++it) {
    graphics::figure::GeometryFilterWorkSet &geometryFilterWorkSet = *it;
    // ジオメトリフィルター用のブラシの初期化
    auto brush = graphics::brush::BrushFactory::Create(pRenderTarget, geometryFilterWorkSet.GetColor(),
                                                       D2D1::RectF(0.0f, 0.0f, GetWidth(), GetHeight()), 1.0f);
    geometryFilterWorkSet.SetBrush(brush);
#ifdef DEBUG_TRIANGLE_FIGURE
    SPDLOG_DEBUG( << L"filterName = " << geometryFilterWorkSet.GetFilterName() << L", brush = " << std::hex
                              << brush.get() << std::dec;
#endif
    CComPtr<ID2D1Bitmap> bitmap = nullptr;
    D2D1_RECT_F bounds = {0};
    auto geometryFilter = geometryFilterWorkSet.GetGeometryFilter();
    // ジオメトリフィルターの適用
    CHK_FATAL_HRESULT(geometryFilter->ApplyFilter(pD3DInteropHelper, pRenderTarget, m_frames[0], GetWidth(),
                                                  GetHeight(), geometryFilterWorkSet.GetBrush()->GetBrush(), &bitmap,
                                                  &bounds));
    geometryFilterWorkSet.SetBitmap(bitmap);
#ifdef DEBUG_TRIANGLE_FIGURE
    SPDLOG_DEBUG( << L"bitmap = " << std::hex << bitmap << std::dec;
#endif
    geometryFilterWorkSet.SetEffectBounds(bounds);
#ifdef DEBUG_TRIANGLE_FIGURE
    SPDLOG_DEBUG( << L"bounds = " << bounds.left << L"," << bounds.top << L"," << bounds.right << L","
                              << bounds.bottom;
#endif
  }
  for (auto it = m_decorators.begin(); it != m_decorators.end(); ++it) {
    CHK_FATAL_HRESULT((*it)->CreateDeviceDependentResource(pD3DInteropHelper, pRenderTarget, m_pGeometry));
  }
#ifdef DEBUG_TRIANGLE_FIGURE
  LOG_LEAVE(SEVERITY_LEVEL_DEBUG);
#endif
  return hr;
}

HRESULT graphics::figure::TriangleFigure::Render(graphics::D3DInteropHelper *pD3DInteropHelper,
                                                 ID2D1RenderTarget *pRenderTarget, FLOAT opacity) {
#ifdef DEBUG_TRIANGLE_FIGURE
  LOG_ENTER(SEVERITY_LEVEL_DEBUG);
#endif
  if (m_propertyChanged) {
    CHK_FATAL_HRESULT(Initialize(pD3DInteropHelper));
    CHK_FATAL_HRESULT(CreateDeviceDependentResources(pD3DInteropHelper, pRenderTarget));
    m_propertyChanged = false;
  }

  pD3DInteropHelper->PushMatrix(pRenderTarget, D2D1::Matrix3x2F::Translation(GetX(), GetY()));
  for (auto it = m_geometryFilter.begin(); it != m_geometryFilter.end(); ++it) {
    graphics::figure::GeometryFilterWorkSet &geometryFilterWorkSet = *it;
    if (geometryFilterWorkSet.GetFilterDisplayOrder() == FILTER_DISPLAY_ORDER_BEFORE_FIGURE) {
      pRenderTarget->DrawBitmap(geometryFilterWorkSet.GetBitmap(), geometryFilterWorkSet.GetEffectBounds(), opacity);
    }
  }
  for (size_t i = 0; i < m_brushes->GetNumberOfBrushes(); ++i) {
    pRenderTarget->FillGeometry(m_frames[i], m_brushes->Get(i)->GetBrush(opacity));
  }
  // decorator
  for (auto it = m_decorators.begin(); it != m_decorators.end(); ++it) {
    (*it)->Render(pD3DInteropHelper, pRenderTarget, m_pGeometry, opacity);
  }
  for (auto it = m_geometryFilter.begin(); it != m_geometryFilter.end(); ++it) {
    graphics::figure::GeometryFilterWorkSet &geometryFilterWorkSet = *it;
    if (geometryFilterWorkSet.GetFilterDisplayOrder() == FILTER_DISPLAY_ORDER_AFTER_FIGURE) {
      pRenderTarget->DrawBitmap(geometryFilterWorkSet.GetBitmap(), geometryFilterWorkSet.GetEffectBounds(), opacity);
    }
  }
  pD3DInteropHelper->PopMatrix(pRenderTarget);
#ifdef DEBUG_TRIANGLE_FIGURE
  LOG_LEAVE(SEVERITY_LEVEL_DEBUG);
#endif
  return S_OK;
}

void graphics::figure::TriangleFigure::DiscardDeviceDependentResources() {
#ifdef DEBUG_TRIANGLE_FIGURE
  LOG_ENTER(SEVERITY_LEVEL_DEBUG);
#endif
  m_brushes->DiscardDeviceDependentResource();
  // decorator
  for (auto it = m_decorators.begin(); it != m_decorators.end(); ++it) {
    (*it)->DiscardDeviceDependentResource();
  }
  for (auto it = m_geometryFilter.begin(); it != m_geometryFilter.end(); ++it) {
    graphics::figure::GeometryFilterWorkSet &geometryFilterWorkSet = *it;
    geometryFilterWorkSet.GetBrush()->DiscardDeviceDependentResource();
    geometryFilterWorkSet.SetBitmap(nullptr);
  }
#ifdef DEBUG_TRIANGLE_FIGURE
  LOG_LEAVE(SEVERITY_LEVEL_DEBUG);
#endif
}

void graphics::figure::TriangleFigure::SetColor(const graphics::color::ColorValue &colorValue,
                                                graphics::color::COLOR_PATTERN colorPattern) {
#ifdef DEBUG_TRIANGLE_FIGURE
  LOG_ENTER(SEVERITY_LEVEL_DEBUG);
#endif
  auto colorSet = std::make_shared<graphics::color::ColorSet>();
  graphics::color::ColorPatternUtil::GeneratePattern(colorPattern, colorValue, colorSet);
  m_brushes->SetColorSet(colorSet);
  m_frames.clear();
  m_frames.resize(m_brushes->GetNumberOfBrushes());
  for (size_t i = 0; i < m_frames.size(); ++i) {
    m_frames[i] = nullptr;
  }
  m_propertyChanged = true;
#ifdef DEBUG_TRIANGLE_FIGURE
  LOG_LEAVE(SEVERITY_LEVEL_DEBUG);
#endif
}

HRESULT graphics::figure::TriangleFigure::_CreateTriangleGeometry(IN ID2D1Factory *factory, IN D2D1_RECT_F rect,
                                                                  IN TRIANGLE_DIRECTION triangleDirection,
                                                                  OUT ID2D1PathGeometry **geometry) {
#ifdef DEBUG_TRIANGLE_FIGURE
  LOG_ENTER(SEVERITY_LEVEL_DEBUG);
#endif
  HRESULT hr = S_OK;
  CHK_ERROR_HRESULT(hr, factory->CreatePathGeometry(geometry));
  if (SUCCEEDED(hr)) {
    CComPtr<ID2D1GeometrySink> sink = nullptr;
    CHK_ERROR_HRESULT(hr, (*geometry)->Open(&sink));
    if (SUCCEEDED(hr)) {
      sink->SetFillMode(D2D1_FILL_MODE_WINDING);
      switch (triangleDirection) {
      case TRIANGLE_DIRECTION_LEFT:
        sink->BeginFigure(D2D1::Point2F(rect.left, (rect.top + rect.bottom) / 2.0f), D2D1_FIGURE_BEGIN_FILLED);
        sink->AddLine(D2D1::Point2F(rect.right, rect.bottom));
        sink->AddLine(D2D1::Point2F(rect.right, rect.top));
        sink->EndFigure(D2D1_FIGURE_END_CLOSED);
        break;
      case TRIANGLE_DIRECTION_TOP:
        sink->BeginFigure(D2D1::Point2F((rect.left + rect.right) / 2.0f, rect.top), D2D1_FIGURE_BEGIN_FILLED);
        sink->AddLine(D2D1::Point2F(rect.left, rect.bottom));
        sink->AddLine(D2D1::Point2F(rect.right, rect.bottom));
        sink->EndFigure(D2D1_FIGURE_END_CLOSED);
        break;
      case TRIANGLE_DIRECTION_RIGHT:
        sink->BeginFigure(D2D1::Point2F(rect.right, (rect.top + rect.bottom) / 2.0f), D2D1_FIGURE_BEGIN_FILLED);
        sink->AddLine(D2D1::Point2F(rect.left, rect.bottom));
        sink->AddLine(D2D1::Point2F(rect.left, rect.top));
        sink->EndFigure(D2D1_FIGURE_END_CLOSED);
        break;
      case TRIANGLE_DIRECTION_BOTTOM:
        sink->BeginFigure(D2D1::Point2F((rect.left + rect.right) / 2.0f, rect.bottom), D2D1_FIGURE_BEGIN_FILLED);
        sink->AddLine(D2D1::Point2F(rect.left, rect.top));
        sink->AddLine(D2D1::Point2F(rect.right, rect.top));
        sink->EndFigure(D2D1_FIGURE_END_CLOSED);
        break;
      }
    }
    CHK_ERROR_HRESULT(hr, sink->Close());
  }
#ifdef DEBUG_TRIANGLE_FIGURE
  LOG_LEAVE(SEVERITY_LEVEL_DEBUG);
#endif
  return hr;
}

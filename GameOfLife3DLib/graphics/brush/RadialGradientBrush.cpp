#include "stdafx.h"
#include "Common.h"
#include "graphics/color/GradientStopCollection.h"
#include "graphics/brush/RadialGradientBrush.h"

graphics::brush::RadialGradientBrush::RadialGradientBrush(void) :
    m_label(),
    m_gradientStops(),
    m_gradientOriginOffsetRate(D2D1::SizeF(0.0f, 0.0f)),
    m_brush(nullptr),
    m_deviceDependentResourceCreated(false)
{
}

graphics::brush::RadialGradientBrush::~RadialGradientBrush(void)
{
}

void graphics::brush::RadialGradientBrush::_CalculateParameters(
    const D2D1_RECT_F &rect,
    OUT D2D1_POINT_2F &center,
    OUT D2D1_POINT_2F &offset,
    OUT FLOAT &radiusX,
    OUT FLOAT &radiusY )
{
    LOG_ENTER(SEVERITY_LEVEL_DEBUG)
            << L"rect=" << rect.left << L"," << rect.top << L"," << rect.right << L"," << rect.bottom;
    radiusX = (rect.right - rect.left) / 2.0f;
    radiusY = (rect.bottom - rect.top) / 2.0f;
    center = D2D1::Point2F(
                 rect.left + radiusX,
                 rect.top + radiusY);
    offset = D2D1::Point2F(radiusX * m_gradientOriginOffsetRate.width,
                           radiusY * m_gradientOriginOffsetRate.height);
    LOG_LEAVE(SEVERITY_LEVEL_DEBUG) << L"center = " << center.x << L"," << center.y
                                    << L", offset = " << offset.x << L"," << offset.y
                                    << L", radius = " << radiusX << L"," << radiusY;
}

HRESULT graphics::brush::RadialGradientBrush::CreateDeviceDependentResource(
    ID2D1RenderTarget *pRenderTarget,
    const D2D1_RECT_F &rect,
    FLOAT opacity)
{
    HRESULT hr = S_OK;
    CComPtr<ID2D1GradientStopCollection> gradientStopCollection;
    CHK_FATAL_HRESULT(pRenderTarget->CreateGradientStopCollection(
                          m_gradientStops.data(),
                          static_cast<UINT>(m_gradientStops.size()),
                          D2D1_GAMMA_2_2,
                          D2D1_EXTEND_MODE_CLAMP,
                          &gradientStopCollection));
    D2D1_POINT_2F center;
    D2D1_POINT_2F offset;
    FLOAT radiusX, radiusY;
    _CalculateParameters(rect, center, offset, radiusX, radiusY);

    m_brush = nullptr;
    CHK_FATAL_HRESULT(
        pRenderTarget->CreateRadialGradientBrush(
            D2D1::RadialGradientBrushProperties(
                D2D1::Point2F(0, 0),
                offset,
                radiusX,
                radiusY
            ),
            D2D1::BrushProperties(opacity,
                                  D2D1::Matrix3x2F::Translation(center.x, center.y)),
            gradientStopCollection,
            &m_brush
        ));
    m_deviceDependentResourceCreated = (m_brush != nullptr);
    return hr;
}

ID2D1Brush* graphics::brush::RadialGradientBrush::GetBrush(
    const D2D1_RECT_F &rect,
    FLOAT opacity)
{
    if (m_brush != nullptr) {
        D2D1_POINT_2F center;
        D2D1_POINT_2F offset;
        FLOAT radiusX, radiusY;
        _CalculateParameters(rect, center, offset, radiusX, radiusY);
        m_brush->SetCenter(D2D1::Point2F(0, 0));
        m_brush->SetGradientOriginOffset(offset);
        m_brush->SetRadiusX(radiusX);
        m_brush->SetRadiusY(radiusY);
        m_brush->SetTransform(D2D1::Matrix3x2F::Translation(center.x, center.y));
        m_brush->SetOpacity(opacity);
        return m_brush;
    } else {
        throw std::runtime_error("brush has not been created");
    }
}

ID2D1Brush* graphics::brush::RadialGradientBrush::GetBrush(
    FLOAT opacity)
{
    if (m_brush != nullptr) {
        m_brush->SetOpacity(opacity);
        return m_brush;
    } else {
        throw std::runtime_error("brush has not been created");
    }
}

ID2D1Brush* graphics::brush::RadialGradientBrush::GetBrush()
{
    if (m_brush != nullptr) {
        return m_brush;
    } else {
        throw std::runtime_error("brush has not been created");
    }
}

void graphics::brush::RadialGradientBrush::DiscardDeviceDependentResource()
{
    m_brush = nullptr;
    m_deviceDependentResourceCreated = false;
}

void graphics::brush::RadialGradientBrush::Clear()
{
    DiscardDeviceDependentResource();
    m_gradientStops.clear();
}

void graphics::brush::RadialGradientBrush::SetGradientStops(
    const graphics::color::GradientStopCollection& collection )
{
    collection.ToD2D1_GRADIENT_STOP(m_gradientStops);
}

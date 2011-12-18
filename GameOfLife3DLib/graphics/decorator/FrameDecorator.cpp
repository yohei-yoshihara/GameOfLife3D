#include "StdAfx.h"
#include "Common.h"
#include "graphics/D3DInteropHelper.h"
#include "graphics/color/ColorSet.h"
#include "graphics/brush/BrushSet.h"
#include "graphics/color/GradientStop.h"
#include "graphics/color/GradientStopCollection.h"
#include "graphics/decorator/Frame.h"
#include "graphics/decorator/FrameDecorator.h"

// #define DEBUG_FRAMEDECORATOR

graphics::decorator::FrameDecorator::FrameDecorator(void) :
    m_frames(),
    m_geometries(),
    m_colorSet(),
    m_brushSet(new graphics::brush::BrushSet()),
    m_deviceDependentResourceCreated(false)
{
}

graphics::decorator::FrameDecorator::FrameDecorator(const FrameDecorator& other) :
    m_frames(other.m_frames),
    m_geometries(other.m_geometries),
    m_colorSet(other.m_colorSet),
    m_brushSet(),
    m_deviceDependentResourceCreated(false)
{
}

graphics::decorator::FrameDecorator::FrameDecorator(FrameDecorator && other) :
    m_frames(std::move(other.m_frames)),
    m_geometries(std::move(other.m_geometries)),
    m_colorSet(std::move(other.m_colorSet)),
    m_brushSet(std::move(other.m_brushSet)),
    m_deviceDependentResourceCreated(std::move(other.m_deviceDependentResourceCreated))
{
}


graphics::decorator::FrameDecorator& graphics::decorator::FrameDecorator::operator=(
    const graphics::decorator::FrameDecorator& other)
{
    if (this != &other) {
        m_frames = other.m_frames;
        m_geometries = other.m_geometries;
        m_colorSet = other.m_colorSet;
        m_deviceDependentResourceCreated = false;
    }
    return *this;
}

graphics::decorator::FrameDecorator& graphics::decorator::FrameDecorator::operator=(
    graphics::decorator::FrameDecorator && other)
{
    if (this != &other) {
        m_frames = std::move(other.m_frames);
        m_geometries = std::move(other.m_geometries);
        m_colorSet = std::move(other.m_colorSet);
        m_brushSet = std::move(other.m_brushSet);
        m_deviceDependentResourceCreated = std::move(other.m_deviceDependentResourceCreated);
    }
    return *this;
}

HRESULT graphics::decorator::FrameDecorator::Initialize(
    graphics::D3DInteropHelper *pD3DInteropHelper,
    ID2D1Geometry *pGeometry )
{
    HRESULT hr = S_OK;
    ID2D1Factory *factory = pD3DInteropHelper->GetD2DFactory();

    m_brushSet->SetColorSet(*m_colorSet);

    m_geometries.clear();
    m_geometries.resize(m_frames.size());

    D2D1_RECT_F origBounds;
    pGeometry->GetBounds(nullptr, &origBounds);
    FLOAT origWidth = origBounds.right - origBounds.left;
    FLOAT origHeight = origBounds.bottom - origBounds.top;
    D2D1_POINT_2F centerPoint = D2D1::Point2F(origWidth / 2.0f, origHeight / 2.0f);

    FLOAT offset = 0.0f;
    for (size_t i = 0; i < m_frames.size(); ++i) {
        // outer frame
        m_geometries[i] = nullptr;
        FLOAT thickOffset = static_cast<long>(m_frames[i].GetThick() / 2.0f) * 2.0f;
        D2D1_SIZE_F scale = D2D1::SizeF(
                                (origWidth - thickOffset - offset * 2.0f) / origWidth,
                                (origHeight - thickOffset - offset * 2.0f) / origHeight);
        D2D1_MATRIX_3X2_F matrix = D2D1::Matrix3x2F::Scale(
                                       scale,
                                       centerPoint);
        CHK_FATAL_HRESULT(factory->CreateTransformedGeometry(pGeometry,
                          &matrix,
                          &m_geometries[i]));

        offset += m_frames[i].GetThick();
    }

    return hr;
}

HRESULT graphics::decorator::FrameDecorator::CreateDeviceDependentResource(
    graphics::D3DInteropHelper *pD3DInteropHelper,
    ID2D1RenderTarget *pRenderTarget,
    ID2D1Geometry *pGeometry)
{
#ifdef DEBUG_FRAMEDECORATOR
    LOG_ENTER(SEVERITY_LEVEL_DEBUG);
#endif
    UNREFERENCED_PARAMETER(pD3DInteropHelper);
    HRESULT hr = S_OK;
    D2D1_RECT_F bounds = {0};
    CHK_FATAL_HRESULT(pGeometry->GetBounds(nullptr, &bounds));
    CHK_FATAL_HRESULT(m_brushSet->CreateDeviceDependentResource(pRenderTarget, bounds, 1.0f));
    m_deviceDependentResourceCreated = SUCCEEDED(hr);

#ifdef DEBUG_FRAMEDECORATOR
    for (size_t i = 0; i < m_brushSet->GetNumberOfBrushes(); ++i) {
        LOG(SEVERITY_LEVEL_DEBUG)
                << L"raw brush[" << i << "] = " << std::hex << m_brushSet->Get(i)->GetBrush() << std::dec;
    }
    LOG_LEAVE(SEVERITY_LEVEL_DEBUG);
#endif
    return hr;
}

HRESULT graphics::decorator::FrameDecorator::Render(
    graphics::D3DInteropHelper *pD3DInteropHelper,
    ID2D1RenderTarget *pRenderTarget,
    ID2D1Geometry *pGeometry,
    FLOAT opacity)
{
#ifdef DEBUG_FRAMEDECORATOR
    LOG_ENTER(SEVERITY_LEVEL_DEBUG);
    for (size_t i = 0; i < m_brushSet->GetNumberOfBrushes(); ++i) {
        LOG(SEVERITY_LEVEL_DEBUG)
                << L"raw brush[" << i << "] = " << std::hex << m_brushSet->Get(i)->GetBrush() << std::dec;
    }
#endif

    UNREFERENCED_PARAMETER(pD3DInteropHelper);
    HRESULT hr = S_OK;
    if (!IsDeviceDependentResourceCreated()) {
        CHK_FATAL_HRESULT(CreateDeviceDependentResource(pD3DInteropHelper, pRenderTarget, pGeometry));
    }
    for (size_t i = 0; i < m_frames.size(); ++i) {
        CComPtr<ID2D1TransformedGeometry> geometry = m_geometries[i];
        D2D1_RECT_F bounds;
        geometry->GetBounds(nullptr, &bounds);
        ID2D1Brush *brush = nullptr;
        switch (m_frames[i].GetBrushReferenceType()) {
        case graphics::decorator::Frame::BRUSH_REFERENCE_TYPE_BY_INDEX: {
            size_t index = m_frames[i].GetBrushIndex();
#ifdef DEBUG_FRAMEDECORATOR
            LOG(SEVERITY_LEVEL_DEBUG) << L"brush index = " << index
                                      << L", brush = " << std::hex << m_brushSet->Get(index)
                                      << L", raw brush = " << m_brushSet->Get(index)->GetBrush() << std::dec;
#endif
            brush = m_brushSet->Get(index)->GetBrush(bounds, opacity);
        }
        break;
        case graphics::decorator::Frame::BRUSH_REFERENCE_TYPE_BY_LABEL:
            brush = m_brushSet->Get(m_frames[i].GetBrushLabel())->GetBrush(bounds, opacity);
            break;
        }
        if (brush != nullptr) {
            pRenderTarget->DrawGeometry(m_geometries[i], brush, m_frames[i].GetThick());
        }
    }
#ifdef DEBUG_FRAMEDECORATOR
    LOG_LEAVE(SEVERITY_LEVEL_DEBUG);
#endif
    return hr;
}

void graphics::decorator::FrameDecorator::DiscardDeviceDependentResource()
{
    m_brushSet->DiscardDeviceDependentResource();
    m_deviceDependentResourceCreated = false;
}

std::wostream& graphics::decorator::operator<<(
    std::wostream& os,
    const graphics::decorator::FrameDecorator &frameDecoration)
{
    os << typeid(frameDecoration).name() << L"[frames={";
    for (size_t i = 0; i < frameDecoration.GetNumberOfFrames(); ++i) {
        os << frameDecoration.GetFrame(i);
        if (i != (frameDecoration.GetNumberOfFrames() - 1)) {
            os << L",";
        }
    }
    os << L"}]";
    return os;
}

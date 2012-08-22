#ifndef GRAPHICS_DECORATION_COLORSET_H_
#define GRAPHICS_DECORATION_COLORSET_H_

#include "graphics/GraphicsCommon.h"
#include "graphics/decorator/IDecorator.h"
#include "graphics/decorator/Frame.h"

namespace graphics
{
namespace decorator
{
class FrameDecorator : public graphics::decorator::IDecorator
{
private:
    std::vector<graphics::decorator::Frame> m_frames;
    std::vector<CComPtr<ID2D1TransformedGeometry>> m_geometries;
    std::shared_ptr<graphics::color::ColorSet> m_colorSet;
    std::unique_ptr<graphics::brush::BrushSet> m_brushSet;
    bool m_deviceDependentResourceCreated;

public:
    FrameDecorator(void);
    FrameDecorator(const FrameDecorator& other);
    FrameDecorator(FrameDecorator && other);
    ~FrameDecorator(void)
    {}
    FrameDecorator& operator=(const FrameDecorator& other);
    FrameDecorator& operator=(FrameDecorator && other);
    void AddFrame(const graphics::decorator::Frame &frame) {
        m_frames.push_back(frame);
        if (frame.GetBrushReferenceType() == Frame::BRUSH_REFERENCE_TYPE_AUTO_INDEX) {
            m_frames.back().SetBrushReferenceType(Frame::BRUSH_REFERENCE_TYPE_BY_INDEX);
            m_frames.back().SetBrushIndex(m_frames.size());
        }
    }
    void RemoveFrame(const graphics::decorator::Frame &frame) {
        m_frames.erase(std::remove(m_frames.begin(), m_frames.end(), frame), m_frames.end());
    }
    size_t GetNumberOfFrames() const {
        return m_frames.size();
    }
    const graphics::decorator::Frame& GetFrame(size_t index) const {
        return m_frames.at(index);
    }
    void SetColorSet(const std::shared_ptr<graphics::color::ColorSet> &colorSet) {
        m_colorSet = colorSet;
    }
    // interface graphics::decorator::IDecorator
    virtual HRESULT Initialize(
        graphics::D3DInteropHelper *pD3DInteropHelper,
        ID2D1Geometry *pGeometry);
    virtual HRESULT CreateDeviceDependentResource(
        graphics::D3DInteropHelper *pD3DInteropHelper,
        ID2D1RenderTarget *pRenderTarget,
        ID2D1Geometry *pGeometry);
    virtual HRESULT Render(
        graphics::D3DInteropHelper *pD3DInteropHelper,
        ID2D1RenderTarget *pRenderTarget,
        ID2D1Geometry *pGeometry,
        FLOAT opacity = 1.0f);
    virtual void DiscardDeviceDependentResource();
    virtual bool IsDeviceDependentResourceCreated() const {
        return m_deviceDependentResourceCreated;
    }
    friend std::wostream& operator<<(
        std::wostream& os,
        const graphics::decorator::FrameDecorator &frameDecoration);
};
}
}

#endif // GRAPHICS_DECORATION_COLORSET_H_

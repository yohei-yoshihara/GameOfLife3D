#ifndef GRAPHICS_BRUSH_LINEARGRADIENTBRUSH_H_
#define GRAPHICS_BRUSH_LINEARGRADIENTBRUSH_H_

#include "graphics/GraphicsCommon.h"
#include "graphics/color/GradientStopCollection.h"
#include "graphics/brush/IBrush.h"

namespace graphics
{
namespace brush
{
class LinearGradientBrush :
    public graphics::brush::IBrush
{
private:
    std::wstring m_label;
    std::vector<D2D1_GRADIENT_STOP> m_gradientStops;
    GRADIENT_DIRECTION m_gradientDirection;
    CComPtr<ID2D1LinearGradientBrush> m_brush;
    bool m_deviceDependentResourceCreated;

protected:
    void _CreateStartAndEndPoint(
        IN FLOAT width,
        IN FLOAT height,
        OUT D2D1_POINT_2F &startPoint,
        OUT D2D1_POINT_2F &endPoint);

public:
    LinearGradientBrush(void);
    virtual ~LinearGradientBrush(void);
    void SetLabel(const std::wstring &label) {
        m_label = label;
    }
    virtual const std::wstring& GetLabel() const {
        return m_label;
    }
    size_t GetNumberOfGradientStops() const {
        return m_gradientStops.size();
    }
    void AddGradientStop(D2D1_GRADIENT_STOP gradientStop) {
        m_gradientStops.push_back(gradientStop);
    }
    void SetGradientStops(const std::vector<D2D1_GRADIENT_STOP>& gradientStops) {
        m_gradientStops = gradientStops;
    }
    void SetGradientDirection(GRADIENT_DIRECTION gradientDirection) {
        m_gradientDirection = gradientDirection;
    }
    void SetGradientStops(const graphics::color::GradientStopCollection& collection) {
        collection.ToD2D1_GRADIENT_STOP(m_gradientStops);
        m_gradientDirection = collection.GetGradientDirection();
    }
    virtual HRESULT CreateDeviceDependentResource(
        ID2D1RenderTarget *pRenderTarget,
        const D2D1_RECT_F &rect,
        FLOAT opacity);
    virtual ID2D1Brush* GetBrush(
        const D2D1_RECT_F &rect,
        FLOAT opacity);
    virtual ID2D1Brush* GetBrush(
        FLOAT opacity);
    virtual ID2D1Brush* GetBrush();
    virtual void DiscardDeviceDependentResource();
    virtual bool IsDeviceDependentResourceCreated() const {
        return m_deviceDependentResourceCreated;
    }
    virtual void Clear();
};
}
}

#endif // GRAPHICS_BRUSH_LINEARGRADIENTBRUSH_H_
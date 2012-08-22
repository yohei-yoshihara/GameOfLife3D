#ifndef GRAPHICS_FIGURE_GEOMETRYFILTER_H_
#define GRAPHICS_FIGURE_GEOMETRYFILTER_H_

#include "graphics/color/ColorValue.h"
#include "graphics/figure/IFigure.h"
#include "graphics/brush/IBrush.h"

namespace graphics
{
namespace figure
{
class GeometryFilterWorkSet
{
private:
    std::wstring m_workSetName;
    std::wstring m_filterName;
    graphics::figure::FILTER_DISPLAY_ORDER m_filterDisplayOrder;
    std::shared_ptr<graphics::color::IColor> m_color;

    std::shared_ptr<graphics::brush::IBrush> m_brush;
    std::shared_ptr<graphics::filter::IGeometryFilter> m_geometryFilter;

    CComPtr<ID2D1Bitmap> m_bitmap;
    D2D1_RECT_F m_effectBounds;

public:
    GeometryFilterWorkSet();
    GeometryFilterWorkSet(const std::wstring &workSetName, const std::wstring &filterName,
                          graphics::figure::FILTER_DISPLAY_ORDER filterDisplayOrder,
                          std::shared_ptr<graphics::color::IColor> color);
    ~GeometryFilterWorkSet()
    {}
    void SetWorkSetName(const std::wstring &workSetName) {
        m_workSetName = workSetName;
    }
    const std::wstring& GetWorkSetName() const {
        return m_workSetName;
    }
    void SetFilterName(const std::wstring &filterName) {
        m_filterName = filterName;
    }
    const std::wstring& GetFilterName() const {
        return m_filterName;
    }
    void SetFilterDisplayOrder(graphics::figure::FILTER_DISPLAY_ORDER filterDisplayOrder) {
        m_filterDisplayOrder = filterDisplayOrder;
    }
    graphics::figure::FILTER_DISPLAY_ORDER GetFilterDisplayOrder() const {
        return m_filterDisplayOrder;
    }
    void SetColor(const std::shared_ptr<graphics::color::IColor> &color) {
        m_color = color;
    }
    const std::shared_ptr<graphics::color::IColor> GetColor() const {
        return m_color;
    }
    void SetGeometryFilter(const std::shared_ptr<graphics::filter::IGeometryFilter> &geometryFilter) {
        m_geometryFilter = geometryFilter;
    }
    std::shared_ptr<graphics::filter::IGeometryFilter> GetGeometryFilter() const {
        return m_geometryFilter;
    }
    void SetBrush(const std::shared_ptr<graphics::brush::IBrush> &brush) {
        m_brush = brush;
    }
    std::shared_ptr<graphics::brush::IBrush> GetBrush() const {
        return m_brush;
    }
    void SetBitmap(CComPtr<ID2D1Bitmap> bitmap) {
        m_bitmap = bitmap;
    }
    CComPtr<ID2D1Bitmap> GetBitmap() const {
        return m_bitmap;
    }
    void SetEffectBounds(D2D1_RECT_F effectBounds) {
        m_effectBounds = effectBounds;
    }
    D2D1_RECT_F GetEffectBounds() const {
        return m_effectBounds;
    }
};
}
}

#endif // GRAPHICS_FIGURE_GEOMETRYFILTER_H_

#include "stdafx.h"
#include "graphics/color/IColor.h"
#include "graphics/figure/IFigure.h"
#include "graphics/figure/GeometryFilterWorkSet.h"

graphics::figure::GeometryFilterWorkSet::GeometryFilterWorkSet() :
    m_workSetName(),
    m_filterName(),
    m_filterDisplayOrder(),
    m_color(),
    m_brush(),
    m_geometryFilter(),
    m_bitmap(nullptr),
    m_effectBounds()
{
}

graphics::figure::GeometryFilterWorkSet::GeometryFilterWorkSet(
    const std::wstring &workSetName,
    const std::wstring &filterName,
    graphics::figure::FILTER_DISPLAY_ORDER filterDisplayOrder,
    std::shared_ptr<graphics::color::IColor> color) :
    m_workSetName(workSetName),
    m_filterName(filterName),
    m_filterDisplayOrder(filterDisplayOrder),
    m_color(color),
    m_brush(),
    m_geometryFilter(),
    m_bitmap(nullptr),
    m_effectBounds()
{
}

#include "stdafx.h"
#include "Common.h"
#include "graphics/color/SolidColor.h"
#include "graphics/color/LinearGradientColor.h"
#include "graphics/color/RadialGradientColor.h"
#include "graphics/brush/SolidColorBrush.h"
#include "graphics/brush/LinearGradientBrush.h"
#include "graphics/brush/RadialGradientBrush.h"
#include "graphics/brush/BrushFactory.h"

std::shared_ptr<graphics::brush::IBrush> graphics::brush::BrushFactory::Create(
    ID2D1RenderTarget *pRenderTarget,
    std::shared_ptr<graphics::color::IColor> color,
    const D2D1_RECT_F &rect,
    FLOAT opacity)
{
    switch (color->GetColorType()) {
    case graphics::color::IColor::COLOR_TYPE_SOLID_COLOR: {
        auto solidColor =
            std::dynamic_pointer_cast<graphics::color::SolidColor>(color);
        auto solidColorBrush =
            std::make_shared<graphics::brush::SolidColorBrush>(solidColor->GetCurrentColor());
        CHK_FATAL_HRESULT(solidColorBrush->CreateDeviceDependentResource(pRenderTarget, rect, opacity));
        if (solidColor->GetLabel().length() > 0) {
            solidColorBrush->SetLabel(solidColor->GetLabel());
        }
        return solidColorBrush;
    }
    break;
    case graphics::color::IColor::COLOR_TYPE_LINEAR_GRADIENT_COLOR: {
        auto linearGradientColor =
            std::dynamic_pointer_cast<graphics::color::LinearGradientColor>(color);
        auto linearGradientBrush =
            std::make_shared<graphics::brush::LinearGradientBrush>();
        linearGradientBrush->SetGradientDirection(linearGradientColor->GetGradientDirection());
        linearGradientBrush->SetGradientStops(linearGradientColor->GetCurrentColor());
        if (linearGradientColor->GetLabel().length() > 0) {
            linearGradientBrush->SetLabel(linearGradientColor->GetLabel());
        }
        return linearGradientBrush;
    }
    break;
    case graphics::color::IColor::COLOR_TYPE_RADIAL_GRADIENT_COLOR: {
        auto radialGradientColor =
            std::dynamic_pointer_cast<graphics::color::RadialGradientColor>(color);
        auto radialGradientBrush =
            std::make_shared<graphics::brush::RadialGradientBrush>();
        radialGradientBrush->SetGradientOriginOffsetRate(
            radialGradientColor->GetGradientOffsetX(),
            radialGradientColor->GetGradientOffsetY());
        radialGradientBrush->SetGradientStops(radialGradientColor->GetCurrentColor());
        if (radialGradientColor->GetLabel().length() > 0) {
            radialGradientBrush->SetLabel(radialGradientColor->GetLabel());
        }
        return radialGradientBrush;
    }
    break;
    }
    throw std::runtime_error("unknown color type");
}

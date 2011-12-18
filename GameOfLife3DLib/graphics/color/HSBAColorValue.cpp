#include "StdAfx.h"
#include "graphics/color/ColorUtil.h"
#include "graphics/color/ColorValue.h"
#include "graphics/color/RGBAColorValue.h"
#include "graphics/color/HSBAColorValue.h"

graphics::color::HSBAColorValue::HSBAColorValue( const RGBAColorValue &rgbaColorValue )
{
    graphics::color::ColorUtil::RgbToHsv(
        rgbaColorValue.GetRed(),
        rgbaColorValue.GetGreen(),
        rgbaColorValue.GetBlue(),
        m_hue,
        m_saturation,
        m_brightness);
    m_alpha = rgbaColorValue.GetAlpha();
}

graphics::color::HSBAColorValue::HSBAColorValue( const ColorValue &colorValue ) :
    m_hue(colorValue.GetHue()),
    m_saturation(colorValue.GetSaturation()),
    m_brightness(colorValue.GetBrightness()),
    m_alpha(colorValue.GetAlpha())
{
}

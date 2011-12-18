#include "StdAfx.h"
#include "graphics/color/ColorUtil.h"
#include "graphics/color/ColorValue.h"
#include "graphics/color/HSBAColorValue.h"
#include "graphics/color/RGBAColorValue.h"

graphics::color::RGBAColorValue::RGBAColorValue( const HSBAColorValue &hsbaColorValue )
{
    graphics::color::ColorUtil::HsvToRgb(
        hsbaColorValue.GetHue(),
        hsbaColorValue.GetSaturation(),
        hsbaColorValue.GetBrightness(),
        m_red,
        m_green,
        m_blue
    );
    m_alpha = hsbaColorValue.GetAlpha();
}

graphics::color::RGBAColorValue::RGBAColorValue( const ColorValue &colorValue ) :
    m_red(colorValue.GetRed()),
    m_green(colorValue.GetGreen()),
    m_blue(colorValue.GetBlue()),
    m_alpha(colorValue.GetAlpha())
{
}

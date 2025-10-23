#include "stdafx.h"
#include "graphics/color/IColor.h"
#include "graphics/color/RadialGradientColor.h"

graphics::color::RadialGradientColor::RadialGradientColor(void)
    : m_label()
    , m_originalStopCollection()
    , m_currentStopCollection()
    , m_gradientOffsetX(0.0f)
    , m_gradientOffsetY(0.0f) {
}

graphics::color::RadialGradientColor::~RadialGradientColor(void) {
}

void graphics::color::RadialGradientColor::Brighter(FLOAT rate) {
  UNREFERENCED_PARAMETER(rate);
}

void graphics::color::RadialGradientColor::Darker(FLOAT rate) {
  UNREFERENCED_PARAMETER(rate);
}

void graphics::color::RadialGradientColor::SetAlpha(FLOAT alpha) {
  UNREFERENCED_PARAMETER(alpha);
}

void graphics::color::RadialGradientColor::Reset() {
}

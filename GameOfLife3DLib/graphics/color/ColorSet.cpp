#include "Stdafx.h"
#include "graphics/color/ColorSet.h"
#include "graphics/color/IColor.h"
#include "graphics/color/SolidColor.h"
#include "graphics/color/LinearGradientColor.h"
#include "graphics/color/RadialGradientColor.h"
#include "util/StringUtil.h"

using namespace graphics::color;

void graphics::color::ColorSet::AddColor(const std::shared_ptr<graphics::color::IColor> &color) {
  m_colors.push_back(color);
  if (color->GetLabel().length() > 0) {
    m_labelToColorMap.insert({color->GetLabel(), color});
  }
}

void graphics::color::ColorSet::Clear() {
  m_colors.clear();
  m_labelToColorMap.clear();
}

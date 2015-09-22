#ifndef GRAPHICS_COLOR_COLORPATTERNUTIL_H_
#define GRAPHICS_COLOR_COLORPATTERNUTIL_H_

#include "graphics/GraphicsCommon.h"

namespace graphics {
  namespace color {
    class ColorPatternUtil {
    private:
      ColorPatternUtil() {
      }
      ~ColorPatternUtil() {
      }

    public:
      static void GeneratePattern(IN COLOR_PATTERN colorPattern, IN const graphics::color::ColorValue &color,
                                  OUT std::vector<std::shared_ptr<graphics::brush::LinearGradientBrush>> &brushes);
      static void GeneratePattern(IN COLOR_PATTERN colorPattern, IN const graphics::color::ColorValue &color,
                                  OUT std::shared_ptr<graphics::color::ColorSet> colorSet);
    };
  }
}
#endif // GRAPHICS_COLOR_COLORPATTERNUTIL_H_
#ifndef GRAPHICS_BRUSH_BRUSHFACTORY_H_
#define GRAPHICS_BRUSH_BRUSHFACTORY_H_

#include "graphics/color/IColor.h"
#include "graphics/brush/IBrush.h"

namespace graphics {
  namespace brush {
    class BrushFactory {
    public:
      static std::shared_ptr<graphics::brush::IBrush> Create(ID2D1RenderTarget *pRenderTarget,
                                                             std::shared_ptr<graphics::color::IColor> color,
                                                             const D2D1_RECT_F &rect, FLOAT opacity);
    };
  }
}

#endif // GRAPHICS_BRUSH_BRUSHFACTORY_H_
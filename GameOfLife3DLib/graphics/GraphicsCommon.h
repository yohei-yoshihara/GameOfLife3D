#ifndef GRAPHICS_GRAPHICSCOMMON_H_
#define GRAPHICS_GRAPHICSCOMMON_H_

namespace graphics {
  enum GRADIENT_DIRECTION {
    GRADIENT_DIRECTION_TOP_BOTTOM,
    GRADIENT_DIRECTION_TOPLEFT_TO_BOTTOMRIGHT,
  };

  class Bitmap;
  class D3DInteropHelper;
  class FontAttribute;

  namespace brush {
    class BrushSet;
    class IBrush;
    class LinearGradientBrush;
    class RadialGradientBrush;
    class SolidColorBrush;
  }

  namespace color {
    enum COLOR_PATTERN {
      COLOR_PATTERN_FLAT,
      COLOR_PATTERN_GRADIENT,
      COLOR_PATTERN_CONVEX,
      COLOR_PATTERN_CONCAVE,
      COLOR_PATTERN_CONVEX_FRAME,
      COLOR_PATTERN_CONCAVE_FRAME,
      COLOR_PATTERN_FRAME,
      COLOR_PATTERN_CONVEX_METAL,
      COLOR_PATTERN_CONCAVE_METAL,
    };
    class ColorSet;
    class ColorValue;
    class GradientStop;
    class GradientStopCollection;
    class HSBAColorValue;
    class IColor;
    class LinearGradientColor;
    class RadialGradientColor;
    class RGBAColorValue;
    class SolidColor;
  }

  namespace decorator {
    class Frame;
    class FrameDecorator;
    class IDecorator;
  }

  namespace figure {
    class EllipseFigure;
    class IFigure;
    class RectangleFigure;
    class TriangleFigure;
  }

  namespace filter {
    class IGeometryFilter;
    class ITextureFilter;
    class GaussFilter;
    class InnerGaussFilter;
  }
}

#endif // GRAPHICS_GRAPHICSCOMMON_H_
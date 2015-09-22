#ifndef GRAPHICS_INNERGAUSSFILTER_H_
#define GRAPHICS_INNERGAUSSFILTER_H_

#include "graphics/GraphicsCommon.h"
#include "graphics/filter/IGeometryFilter.h"

namespace graphics {
  namespace filter {
    class InnerGaussFilter : public graphics::filter::IGeometryFilter,
                             public std::enable_shared_from_this<InnerGaussFilter> {
    private:
      std::shared_ptr<graphics::filter::GaussFilter> m_gaussFilter;

    public:
      InnerGaussFilter(void);
      virtual ~InnerGaussFilter(void);
      virtual HRESULT Initialize();
      virtual HRESULT ApplyFilter(IN graphics::D3DInteropHelper *pD3DInteropHelper, IN ID2D1RenderTarget *pRenderTarget,
                                  IN ID2D1Geometry *pGeometry, IN FLOAT width, IN FLOAT height, IN ID2D1Brush *pBrush,
                                  OUT ID2D1Bitmap **ppBitmap, OUT D2D1_RECT_F *pBitmapRect);
      virtual void CalculateMargins(IN FLOAT width, IN FLOAT height, OUT FLOAT &leftMargin, OUT FLOAT &topMargin,
                                    OUT FLOAT &rightMargin, OUT FLOAT &bottomMargin);
    };
  }
}

#endif // GRAPHICS_INNERGAUSSFILTER_H_
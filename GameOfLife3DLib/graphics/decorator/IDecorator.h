#ifndef GRAPHICS_DECORATOR_IDECORATOR_H_
#define GRAPHICS_DECORATOR_IDECORATOR_H_

#include "graphics/GraphicsCommon.h"

namespace graphics {
  namespace decorator {
    class IDecorator {
    public:
      virtual HRESULT Initialize(graphics::D3DInteropHelper *pD3DInteropHelper, ID2D1Geometry *pGeometry) = 0;
      virtual HRESULT CreateDeviceDependentResource(graphics::D3DInteropHelper *pD3DInteropHelper,
                                                    ID2D1RenderTarget *pRenderTarget, ID2D1Geometry *pGeometry)
          = 0;
      virtual HRESULT Render(graphics::D3DInteropHelper *pD3DInteropHelper, ID2D1RenderTarget *pRenderTarget,
                             ID2D1Geometry *pGeometry, FLOAT opacity = 1.0f)
          = 0;
      virtual void DiscardDeviceDependentResource() = 0;
      virtual bool IsDeviceDependentResourceCreated() const = 0;
    };
  }
}

#endif // GRAPHICS_DECORATOR_IDECORATOR_H_
#ifndef GRAPHICS_GAUSSFILTER_H_
#define GRAPHICS_GAUSSFILTER_H_

#include "graphics/GraphicsCommon.h"
#include "graphics/filter/IGeometryFilter.h"
#include "graphics/filter/ITextureFilter.h"

namespace graphics {
  namespace filter {
#define NUMBER_OF_WEIGHTS 8
    class GaussFilter : public graphics::filter::IGeometryFilter,
                        public graphics::filter::ITextureFilter,
                        public std::enable_shared_from_this<GaussFilter> {
    private:
      std::array<float, NUMBER_OF_WEIGHTS> m_gaussWeight;
      std::array<float, NUMBER_OF_WEIGHTS> m_tapOffsetX;
      std::array<float, NUMBER_OF_WEIGHTS> m_tapOffsetY;

      ID3D10Effect *m_pGaussEffect;
      ID3D10EffectTechnique *m_pTechniqueNoRef;
      ID3D10EffectShaderResourceVariable *m_pDiffuseVariableNoRef;
      ID3D10Buffer *m_pVertexBuffer;
      ID3D10InputLayout *m_pVertexLayout;
      ID3D10Buffer *m_pFacesIndexBuffer;

    protected:
      void _UpdateWeight(float dispersion);
      void _UpdateTapOffset(int width, int height);
      HRESULT _SetGaussParameters(int width, int height);

    public:
      GaussFilter(void);
      virtual ~GaussFilter(void);
      virtual HRESULT Initialize();
      virtual HRESULT ApplyFilter(IN graphics::D3DInteropHelper *pD3DInteropHelper, IN ID3D10Texture2D *pSourceTexture,
                                  OUT ID3D10Texture2D **ppOutputTexture);
      virtual HRESULT ApplyFilter(IN graphics::D3DInteropHelper *pD3DInteropHelper, IN ID2D1RenderTarget *pRenderTarget,
                                  IN ID2D1Geometry *pGeometry, IN FLOAT width, IN FLOAT height, IN ID2D1Brush *pBrush,
                                  OUT ID2D1Bitmap **ppBitmap, OUT D2D1_RECT_F *pBitmapRect);
      virtual void CalculateMargins(IN FLOAT width, IN FLOAT height, OUT FLOAT &leftMargin, OUT FLOAT &topMargin,
                                    OUT FLOAT &rightMargin, OUT FLOAT &bottomMargin) {
        UNREFERENCED_PARAMETER(width);
        UNREFERENCED_PARAMETER(height);
        FLOAT margin = static_cast<FLOAT>(NUMBER_OF_WEIGHTS /*- 1*/);
        leftMargin = margin;
        topMargin = margin;
        rightMargin = margin;
        bottomMargin = margin;
      }
      HRESULT CreateVertexAndIndexBufferForFilter(graphics::D3DInteropHelper *pD3DInteropHelper);
      void SetVertexAndIndexBufferForFilter(graphics::D3DInteropHelper *pD3DInteropHelper);
    };
  }
}

#endif // GRAPHICS_GAUSSFILTER_H_
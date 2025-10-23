#ifndef GRAPHICS_SHAREDSURFACE_H_
#define GRAPHICS_SHAREDSURFACE_H_

namespace graphics {
  class SharedSurface {
  private:
    ID3D11Texture2D *m_pD3D11SharedTexture;
    IDXGIKeyedMutex *m_pD3D11KeyedMutex;
    HANDLE m_d3d11SharedHandle;
    IDXGIKeyedMutex *m_pD3D10KeyedMutex;
    IDXGISurface1 *m_pD3D10SharedSurface;
    ID2D1RenderTarget *m_pRenderTarget;
    ID2D1Bitmap *m_pBitmap;

  public:
    SharedSurface();
    virtual ~SharedSurface();
    HRESULT Initialize(UINT width, UINT height, ID3D11Device *pD3D11Device, ID3D10Device *pD3D10Device,
                       ID2D1Factory *pD2DFactory);
    ID3D11Texture2D *GetSharedTexture() const {
      return m_pD3D11SharedTexture;
    }
    ID2D1RenderTarget *GetD2DRenderTarget() const {
      return m_pRenderTarget;
    }
    ID2D1Bitmap *GetSharedBitmap() const {
      return m_pBitmap;
    }
    IDXGIKeyedMutex *GetKeyedMutexForD3D() const {
      return m_pD3D11KeyedMutex;
    }
    IDXGIKeyedMutex *GetKeyedMutexForD2D() const {
      return m_pD3D10KeyedMutex;
    }
  };
}

#endif

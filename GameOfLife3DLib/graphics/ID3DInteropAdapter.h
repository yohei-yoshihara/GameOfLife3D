#ifndef GRAPHICS_ID3DINTEROPADAPTER_H_
#define GRAPHICS_ID3DINTEROPADAPTER_H_
namespace graphics {
  class D3DInteropHelper;
  class ID3DInteropAdapter {
  public:
    // Direct 2D
    virtual HRESULT CreateDeviceIndependentResources(D3DInteropHelper *pD3DInteropHelper, ID2D1Factory *pD2DFactory,
                                                     IWICImagingFactory *pWICFactory, IDWriteFactory *pDWriteFactory)
        = 0;
    virtual HRESULT CreateD2DResources(D3DInteropHelper *pD3DInteropHelper, ID2D1Factory *pD2DFactory,
                                       ID2D1RenderTarget *pRenderTarget)
        = 0;
    virtual HRESULT Draw2D(D3DInteropHelper *pD3DInteropHelper, ID2D1RenderTarget *pRenderTarget) = 0;

    // Direct 3D
    virtual HRESULT CreateD3DResources(graphics::D3DInteropHelper *pD3DInteropHelper, ID3D11Device *pDevice,
                                       ID3D11DeviceContext *pImmediateContext)
        = 0;
    virtual HRESULT RecreateSizedResources(UINT nWidth, UINT nHeight, graphics::D3DInteropHelper *pD3DInteropHelper,
                                           ID3D11Device *pDevice, ID3D11DeviceContext *pImmediateContext)
        = 0;
    virtual HRESULT Draw3D(graphics::D3DInteropHelper *pD3DInteropHelper, ID3D11Device *pDevice,
                           ID3D11DeviceContext *pImmediateContext)
        = 0;

    virtual void DiscardDeviceResources() = 0;
  };
}

#endif // GRAPHICS_ID3DINTEROPADAPTER_H_
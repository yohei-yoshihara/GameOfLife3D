#ifndef DUMMYADAPTER_H_
#define DUMMYADAPTER_H_

class DummyAdapter : public graphics::ID3DInteropAdapter
{
public:
    virtual HRESULT RecreateSizedResources(
        UINT nWidth,
        UINT nHeight,
        graphics::D3DInteropHelper *pD3DInteropHelper,
        ID3D11Device *pDevice,
        ID3D11DeviceContext *pImmediateContext ) {
        return S_OK;
    }

    virtual HRESULT Draw2D(
        graphics::D3DInteropHelper *pD3DInteropHelper,
        ID2D1RenderTarget *pRenderTarget ) {
        return S_OK;
    }

    virtual HRESULT Draw3D(
        graphics::D3DInteropHelper *pD3DInteropHelper,
        ID3D11Device *pDevice,
        ID3D11DeviceContext *pImmediateContext ) {
        return S_OK;
    }

    virtual HRESULT PrepareDraw3D(
        graphics::D3DInteropHelper *pD3DInteropHelper,
        ID3D10Device *pDevice ) {
        return S_OK;
    }

    virtual void DiscardDeviceResources() {
    }

    virtual HRESULT AfterDraw3D(
        graphics::D3DInteropHelper *pD3DInteropHelper,
        ID2D1RenderTarget *pRenderTarget ) {
        return S_OK;
    }

    virtual HRESULT CreateDeviceIndependentResources(
        graphics::D3DInteropHelper *pD3DInteropHelper,
        ID2D1Factory *pD2DFactory,
        IWICImagingFactory *pWICFactory,
        IDWriteFactory *pDWriteFactory ) {
        return S_OK;
    }

    virtual HRESULT CreateD2DResources(
        graphics::D3DInteropHelper *pD3DInteropHelper,
        ID2D1Factory *pD2DFactory,
        ID2D1RenderTarget *pRenderTarget ) {
        return S_OK;
    }

    virtual HRESULT CreateD3DResources(
        graphics::D3DInteropHelper *pD3DInteropHelper,
        ID3D11Device *pDevice,
        ID3D11DeviceContext *pImmediateContext ) {
        return S_OK;
    }

    virtual HRESULT BeforeDraw3D(
        graphics::D3DInteropHelper *pD3DInteropHelper,
        ID2D1RenderTarget *pRenderTarget ) {
        return S_OK;
    }
};

#endif // DUMMYADAPTER_H_

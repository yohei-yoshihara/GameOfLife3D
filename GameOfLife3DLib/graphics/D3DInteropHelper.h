#ifndef GRAPHICS_D3DINTEROPHELPER_H_
#define GRAPHICS_D3DINTEROPHELPER_H_

#include "graphics/GraphicsCommon.h"
#include "graphics/filter/IGeometryFilter.h"
#include "animation/IAnimationWindow.h"

namespace graphics
{
struct D2DVertex {
    DirectX::XMFLOAT3 Pos;
    DirectX::XMFLOAT2 Tex;
};

class ID3DInteropAdapter;
class SharedSurface;

class D3DInteropHelper
{
private:
    bool m_windowlessMode;
    HWND m_hwnd;
    float m_backgroundColor[4];
    // CreateDeviceIndependentResources
    ID2D1Factory *m_pD2DFactory;
    IWICImagingFactory *m_pWICFactory;
    IDWriteFactory *m_pDWriteFactory;
    // DirectX11
    UINT m_adapterIndex;
    ID3D11Device *m_pD3D11Device;
    bool m_d3D11DeviceHardware;
    ID3D11DeviceContext *m_pD3D11ImmediateContext;
    IDXGISwapChain *m_pD3D11SwapChain;
    ID3D10Device1 *m_pD3D10Device;
    ID3D11RasterizerState *m_pD3D11RasterizerState;
    ID3D11RenderTargetView *m_pD3D11BackBufferRenderTargetView;
    ID3D11Texture2D *m_pD3D11DepthStencil;
    ID3D11DepthStencilView *m_pD3D11DepthStencilView;
    ID3D11Texture2D *m_pD3D11BackBuffer;
    ID3D11Texture2D *m_pD3D11SharedTexture;
    IDXGIKeyedMutex *m_pD3D11KeyedMutex;
    HANDLE m_d3d11SharedHandle;
    IDXGISurface1 *m_pD3D10SharedSurface;
    IDXGIKeyedMutex *m_pD3D10KeyedMutex;
    ID2D1RenderTarget *m_pD2DTexture2DRenderTargetD3D11;
    ID3D11BlendState *m_pD3D11BlendState;
    D3D11_VIEWPORT m_viewport;
    // DirectX11 ComputeShader
    bool m_sharedCSDevice;
    bool m_computeShaderSupport;
    ID3D11Device *m_pD3D11CSDevice;
    ID3D11DeviceContext *m_pD3D11CSImmediateContext;
    // Direct2D
    ID3D11Buffer *m_pD2DVertexBuffer;
    ID3D11Buffer *m_pD2DIndexBuffer;
    // User Application
    std::weak_ptr<ID3DInteropAdapter> m_pD3DInteropAdapter;
    // for MSAA
    UINT m_msaaSampleCount;
    UINT m_msaaQuality;
    // for Animation
    IUIAnimationManager *m_pAnimationManager;
    IUIAnimationTimer *m_pAnimationTimer;
    IUIAnimationTransitionLibrary *m_pTransitionLibrary;
    std::weak_ptr<animation::IAnimationWindow> m_animationWindow;
    // matrix queue
    std::stack<D2D1_MATRIX_3X2_F> m_matrixQueue;
    // geometry filter
    std::unordered_map<std::wstring, std::shared_ptr<graphics::filter::IGeometryFilter>>
            m_geometryFilterMap;
    // texture filter
    std::unordered_map<std::wstring, std::shared_ptr<graphics::filter::ITextureFilter>>
            m_textureFilterMap;

    // Blend
    ID3D11VertexShader *m_pVertexShaderForBlend;
    ID3D11InputLayout *m_pVertexLayoutForBlend;
    ID3D11PixelShader *m_pPixelShaderForBlend;
    ID3D11ShaderResourceView *m_pTextureRVForBlend;
    ID3D11SamplerState *m_pSamplerLinearForBlend;

protected:
    HRESULT _CreateD3DDevice11(
        IDXGIAdapter1 *pAdapter,
        D3D_DRIVER_TYPE driverType,
        UINT flags,
        OUT ID3D11Device **ppDevice,
        OUT ID3D11DeviceContext **ppImmediateContext);
    HRESULT _CreateD3DDevice10(
        IDXGIAdapter *pAdapter,
        D3D10_DRIVER_TYPE driverType,
        UINT flags,
        OUT ID3D10Device1 **ppDevice);
    HRESULT _CreateCSDevice(
        IDXGIAdapter1 *pAdapter,
        UINT flags,
        OUT ID3D11Device **ppDevice,
        OUT ID3D11DeviceContext **ppImmediateContext);
    HRESULT _CreateD3DDeviceResources11();
    HRESULT _RecreateSizedResources11(UINT nWidth, UINT nHeight);
    HRESULT _InitializeAnimation();
public:
    static const D3D11_INPUT_ELEMENT_DESC s_D2DInputLayoutForBlend[];
    static const graphics::D2DVertex s_D2DVertexArrayForBlend[];
    static const SHORT s_D2DIndexArrayForBlend[];
    static HRESULT LoadResourceBitmap(
        ID2D1RenderTarget *pRenderTarget,
        IWICImagingFactory *pIWICFactory,
        PCWSTR resourceName,
        PCWSTR resourceType,
        ID2D1Bitmap **ppBitmap) {
        return LoadResourceBitmap(pRenderTarget,
                                  pIWICFactory,
                                  resourceName,
                                  resourceType,
                                  0,
                                  0,
                                  ppBitmap);
    }
    static HRESULT LoadResourceBitmap(
        ID2D1RenderTarget *pRenderTarget,
        IWICImagingFactory *pIWICFactory,
        PCWSTR resourceName,
        PCWSTR resourceType,
        UINT destinationWidth,
        UINT destinationHeight,
        ID2D1Bitmap **ppBitmap);
    static HRESULT LoadBitmapFile(
        ID2D1RenderTarget *pRenderTarget,
        IWICImagingFactory *pIWICFactory,
        const wchar_t *fileName,
        ID2D1Bitmap **ppBitmap
    );
    static HRESULT CompileShaderFromResource(
        ID3D10Device *pDevice,
        PCWSTR pszResource,
        ID3D10Effect **ppShader);
    HRESULT CompileShaderFromResource(
        PCWSTR pszResource,
        LPCSTR szEntryPoint,
        LPCSTR szShaderModel,
        OUT ID3DBlob **ppBlob,
        OUT std::wstring *errorMessage = nullptr);
    HRESULT CompileShaderFromFile(
        WCHAR* szFileName,
        LPCSTR szEntryPoint,
        LPCSTR szShaderModel,
        OUT ID3DBlob** ppBlobOut,
        OUT std::wstring *errorMessage = nullptr);

    D3DInteropHelper(bool windowlessMode = false);
    virtual ~D3DInteropHelper(void);
    void SetHWnd(HWND hwnd) {
        m_hwnd = hwnd;
    }
    HWND GetHWnd() {
        return m_hwnd;
    }
    void SetD3DInteropAdapter(
        std::weak_ptr<ID3DInteropAdapter> pD3DInteropAdapter) {
        m_pD3DInteropAdapter = pD3DInteropAdapter;
    }
    void SetAnimationWindow(std::weak_ptr<animation::IAnimationWindow> animationWindow) {
        m_animationWindow = animationWindow;
    }
    ID3D11Device* GetD3D11Device() const {
        return m_pD3D11Device;
    }
    ID3D11DeviceContext* GetD3D11ImmediateContext() const {
        return m_pD3D11ImmediateContext;
    }
    bool IsComputeShaderSupported() const {
        return m_computeShaderSupport;
    }
    ID3D11Device* GetD3D11CSDevice() const {
        return m_pD3D11CSDevice;
    }
    ID3D11DeviceContext* GetD3D11CSImmediateContext() const {
        return m_pD3D11CSImmediateContext;
    }
    IDXGIKeyedMutex* GetD3D11KeyedMutex() const {
        return m_pD3D11KeyedMutex;
    }
    IDXGIKeyedMutex* GetD3D10KeyedMutex() const {
        return m_pD3D10KeyedMutex;
    }
    ID3D10Device1* GetD3D10Device() const {
        return m_pD3D10Device;
    }
    ID2D1Factory* GetD2DFactory() const {
        return m_pD2DFactory;
    }
    IWICImagingFactory* GetWICImagingFactory() const {
        return m_pWICFactory;
    }
    IDWriteFactory* GetDWriteFactory() const {
        return m_pDWriteFactory;
    }
    IUIAnimationManager* GetAnimationManager() const {
        return m_pAnimationManager;
    }
    IUIAnimationTimer* GetAnimationTimer() const {
        return m_pAnimationTimer;
    }
    IUIAnimationTransitionLibrary* GetAnimationTransitionLibrary() const {
        return m_pTransitionLibrary;
    }
    HRESULT CreateDeviceIndependentResources();
    HRESULT OnRender();
    void OnResize(UINT width, UINT height);
    void DiscardDeviceResources();
    HRESULT CreateSharedSurface(UINT width, UINT height, OUT std::shared_ptr<graphics::SharedSurface> &sharedSurface);
    HRESULT CreateTexture2D(UINT width, UINT height, ID3D11Texture2D **ppTexture2D);
    HRESULT _CreateVertexAndIndexBufferFor2D();
    void PushMatrix(ID2D1RenderTarget *pRenderTarget, const D2D1_MATRIX_3X2_F &matrix);
    void PopMatrix(ID2D1RenderTarget *pRenderTarget);

    // geometry filter
    void RegisterGeometryFilter(
        const std::wstring &geometryFilterName,
        std::shared_ptr<graphics::filter::IGeometryFilter> geometryFilter);
    std::shared_ptr<graphics::filter::IGeometryFilter> GetGeometryFilter(
        const std::wstring &geometryFilterName) const;

    // texture filter
    void RegisterTextureFilter(
        const std::wstring &textureFilterName,
        std::shared_ptr<graphics::filter::ITextureFilter> textureFilter);
    std::shared_ptr<graphics::filter::ITextureFilter> GetTextureFilter(
        const std::wstring &textureFilterName) const;

    HRESULT CreateDeviceResources(UINT adapterIndex);
    HRESULT ListAdapters(OUT std::vector<std::wstring> adapterDescriptions);
    HRESULT Blend(ID3D11Texture2D *tex);
    HRESULT DetectMSAA();

    // D3D10
    HRESULT LoadD3D10ResourceShader(
        ID3D10Device *pDevice,
        PCWSTR pszResource,
        ID3D10Effect **ppShader);
    HRESULT CreateD3D10Texture2D(
        UINT width,
        UINT height,
        OUT ID3D10Texture2D **ppTexture2D);
    HRESULT CreateD2DRenderTargetFromD3D10Texture2D(
        ID3D10Texture2D *pTexure2D,
        ID2D1RenderTarget **ppRenderTarget);
    HRESULT CreateD2DSharedBitmapFromD3D10Texture2D(
        ID2D1RenderTarget *pRenderTarget,
        ID3D10Texture2D *pTexure2D,
        ID2D1Bitmap **ppBitmap);
};

}

#endif // GRAPHICS_D3DINTEROPHELPER_H_
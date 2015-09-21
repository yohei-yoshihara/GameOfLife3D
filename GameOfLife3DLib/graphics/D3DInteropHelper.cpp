#include "stdafx.h"
#include "Common.h"
#include "util/Utils.h"
#include "animation/IAnimationWindow.h"
#include "animation/AnimationManagerEventHandler.h"
#include "graphics/SharedSurface.h"
#include "graphics/ID3DInteropAdapter.h"
#include "graphics/D3DInteropHelper.h"

using namespace DirectX;

//#define DEBUG_GRAPHICS_D3DINTEROPHELPER_

/*static*/
const D3D11_INPUT_ELEMENT_DESC graphics::D3DInteropHelper::s_D2DInputLayoutForBlend[] = {
    { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0},
    { "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0},
};

/*static*/
const graphics::D2DVertex graphics::D3DInteropHelper::s_D2DVertexArrayForBlend[] = {
    { XMFLOAT3(-1.0f, -1.0f, 0.0f), XMFLOAT2(0.0f, 1.0f) },
    { XMFLOAT3(1.0f, -1.0f, 0.0f), XMFLOAT2(1.0f, 1.0f) },
    { XMFLOAT3(1.0f, 1.0f, 0.0f), XMFLOAT2(1.0f, 0.0f) },
    { XMFLOAT3(-1.0f, 1.0f, 0.0f), XMFLOAT2(0.0f, 0.0f) }
};

/*static*/
const SHORT graphics::D3DInteropHelper::s_D2DIndexArrayForBlend[] = {
    3, 1, 0,
    2, 1, 3
};

graphics::D3DInteropHelper::D3DInteropHelper(bool windowlessMode) :
    m_windowlessMode(windowlessMode),
    m_hwnd(nullptr),
    // CreateDeviceIndependentResources
    m_pD2DFactory(nullptr),
    m_pWICFactory(nullptr),
    m_pDWriteFactory(nullptr),
    // DirectX11
    m_adapterIndex(0),
    m_pD3D11Device(nullptr),
    m_d3D11DeviceHardware(false),
    m_pD3D11ImmediateContext(nullptr),
    m_pD3D11SwapChain(nullptr),
    m_pD3D10Device(nullptr),
    m_pD3D11RasterizerState(nullptr),
    m_pD3D11BackBufferRenderTargetView(nullptr),
    m_pD3D11DepthStencil(nullptr),
    m_pD3D11DepthStencilView(nullptr),
    m_pD3D11BackBuffer(nullptr),
    m_pD3D11SharedTexture(nullptr),
    m_pD3D11KeyedMutex(nullptr),
    m_d3d11SharedHandle(nullptr),
    m_pD3D10SharedSurface(nullptr),
    m_pD3D10KeyedMutex(nullptr),
    m_pD2DTexture2DRenderTargetD3D11(nullptr),
    m_pD3D11BlendState(nullptr),
    m_viewport(),
    // DirectX11 ComputeShader
    m_sharedCSDevice(false),
    m_computeShaderSupport(false),
    m_pD3D11CSDevice(nullptr),
    m_pD3D11CSImmediateContext(nullptr),
    // Direct2D
    m_pD2DVertexBuffer(nullptr),
    m_pD2DIndexBuffer(nullptr),
    // User application
    m_pD3DInteropAdapter(),
    // for MSAA
    m_msaaSampleCount(1),
    m_msaaQuality(0),
    // for Animation
    m_pAnimationManager(nullptr),
    m_pAnimationTimer(nullptr),
    m_pTransitionLibrary(nullptr),
    m_animationWindow(),
    m_matrixQueue(),
    // geometry filter
    m_geometryFilterMap(),
    // texture filter
    m_textureFilterMap(),
    // Blend
    m_pVertexShaderForBlend(nullptr),
    m_pVertexLayoutForBlend(nullptr),
    m_pPixelShaderForBlend(nullptr),
    m_pTextureRVForBlend(nullptr),
    m_pSamplerLinearForBlend(nullptr)
{
    m_backgroundColor[0] = 0.2f;
    m_backgroundColor[1] = 0.2f;
    m_backgroundColor[2] = 0.2f;
    m_backgroundColor[3] = 1.0f;
}

graphics::D3DInteropHelper::~D3DInteropHelper(void)
{
#ifdef DEBUG_RESOURCE
    LOG_ENTER(SEVERITY_LEVEL_INFO) << L"[RESOURCE] enter";
#endif
    // DirectX11
    SafeRelease(&m_pD3D11RasterizerState);
    SafeRelease(&m_pD3D11BackBufferRenderTargetView);
    SafeRelease(&m_pD3D11DepthStencil);
    SafeRelease(&m_pD3D11DepthStencilView);
    SafeRelease(&m_pD3D11BackBuffer);
    SafeRelease(&m_pD3D11SharedTexture);
    SafeRelease(&m_pD3D11KeyedMutex);
    m_d3d11SharedHandle = nullptr;
    SafeRelease(&m_pD3D10SharedSurface);
    SafeRelease(&m_pD3D10KeyedMutex);
    SafeRelease(&m_pD2DTexture2DRenderTargetD3D11);
    SafeRelease(&m_pD3D11BlendState);

    SafeRelease(&m_pD3D11SwapChain);
    SafeRelease(&m_pD3D11ImmediateContext);
    SafeRelease(&m_pD3D10Device);
    if (!m_sharedCSDevice) {
        SafeRelease(&m_pD3D11CSImmediateContext);
        SafeRelease(&m_pD3D11CSDevice);
    }
    SafeRelease(&m_pD3D11Device);

    // CreateVertexAndIndexBuffer
    SafeRelease(&m_pD2DVertexBuffer);
    SafeRelease(&m_pD2DIndexBuffer);

    // Release factories
    SafeRelease(&m_pD2DFactory);
    SafeRelease(&m_pWICFactory);
    SafeRelease(&m_pDWriteFactory);

    SafeRelease(&m_pAnimationManager);
    SafeRelease(&m_pAnimationTimer);
    SafeRelease(&m_pTransitionLibrary);

#ifdef DEBUG_RESOURCE
    LOG_LEAVE(SEVERITY_LEVEL_INFO) << L"[RESOURCE] leave";
#endif
}

void graphics::D3DInteropHelper::DiscardDeviceResources()
{
    if (!m_pD3DInteropAdapter.expired()) {
        m_pD3DInteropAdapter.lock()->DiscardDeviceResources();
    }
    // DirectX11
    SafeRelease(&m_pD3D11RasterizerState);
    SafeRelease(&m_pD3D11BackBufferRenderTargetView);
    SafeRelease(&m_pD3D11DepthStencil);
    SafeRelease(&m_pD3D11DepthStencilView);
    SafeRelease(&m_pD3D11BackBuffer);
    SafeRelease(&m_pD3D11SharedTexture);
    SafeRelease(&m_pD3D11KeyedMutex);
    m_d3d11SharedHandle = nullptr;
    SafeRelease(&m_pD3D10SharedSurface);
    SafeRelease(&m_pD3D10KeyedMutex);
    SafeRelease(&m_pD2DTexture2DRenderTargetD3D11);
    SafeRelease(&m_pD3D11BlendState);

    SafeRelease(&m_pD2DVertexBuffer);
    SafeRelease(&m_pD2DIndexBuffer);
}

HRESULT graphics::D3DInteropHelper::CreateDeviceResources(UINT adapterIndex)
{
#ifdef DEBUG_GRAPHICS_D3DINTEROPHELPER_
    LOG_ENTER(SEVERITY_LEVEL_DEBUG);
#endif

    HRESULT hr = S_OK;

    if (m_hwnd == nullptr) {
        LOG(SEVERITY_LEVEL_WARN) << L"HWND has not been set.";
        return S_FALSE;
    }

    RECT rcClient;
    if (!m_windowlessMode) {
        GetClientRect(m_hwnd, &rcClient);
    } else {
        rcClient.right = 1024;
        rcClient.left = 0;
        rcClient.bottom = 768;
        rcClient.top = 0;
    }
    UINT nWidth = std::abs(rcClient.right - rcClient.left);
    UINT nHeight = std::abs(rcClient.bottom - rcClient.top);

    if (!m_pD3D11Device) {
        // *** Create DXGIFactory ***
        CComPtr<IDXGIFactory1> pDXGIFactory1 = nullptr;
        hr = CreateDXGIFactory1(__uuidof(IDXGIFactory1), (void**)&pDXGIFactory1);
        if (FAILED(hr)) {
            LOG(SEVERITY_LEVEL_ERROR) << L"Creating DXGIFactory1 was failed";
            return hr;
        }
        // *** Get DXGIAdapter1 ***
        CComPtr<IDXGIAdapter1> pDXGIAdapter1 = nullptr;
        hr = pDXGIFactory1->EnumAdapters1(adapterIndex, &pDXGIAdapter1);
        if (FAILED(hr)) {
            LOG(SEVERITY_LEVEL_ERROR) << L"EnumAdapters1 was failed";
            return hr;
        }
        // *** Create ID3D11Device ***
        LOG(SEVERITY_LEVEL_INFO) << L"Try to create ID3D11Device";
        D3D_DRIVER_TYPE d3dDriverTypeAttempt[] = {
//             D3D_DRIVER_TYPE_HARDWARE,
            D3D_DRIVER_TYPE_UNKNOWN,
            D3D_DRIVER_TYPE_WARP,
        };
        wchar_t* d3dDriverTypeAttemptLabel[] = {
//             L"D3D_DRIVER_TYPE_HARDWARE",
            L"D3D_DRIVER_TYPE_UNKNOWN",
            L"D3D_DRIVER_TYPE_WARP",
        };

#if defined(DEBUG) || defined(_DEBUG)
        UINT nDeviceFlags = D3D11_CREATE_DEVICE_BGRA_SUPPORT | D3D11_CREATE_DEVICE_DEBUG;
#else
        UINT nDeviceFlags = D3D11_CREATE_DEVICE_BGRA_SUPPORT;
#endif
        for (size_t i = 0; i < ARRAYSIZE(d3dDriverTypeAttempt); ++i) {
            hr = _CreateD3DDevice11(
                     pDXGIAdapter1,
                     d3dDriverTypeAttempt[i],
                     nDeviceFlags,
                     &m_pD3D11Device,
                     &m_pD3D11ImmediateContext);
            if (SUCCEEDED(hr)) {
                m_d3D11DeviceHardware = (i == 0);
                LOG(SEVERITY_LEVEL_INFO) << L"driver type :" << d3dDriverTypeAttemptLabel[i];
                break;
            }
        }
        if (FAILED(hr)) {
            LOG(SEVERITY_LEVEL_FATAL) << L"Creating D3D11 device was failed.";
            return hr;
        }
        if (SUCCEEDED(hr)) {
            m_pD3D11ImmediateContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
        }

        if (SUCCEEDED(hr)) {
            DetectMSAA();
        }

        // *** Create SwapChain ***
        if (!m_windowlessMode) {
            DXGI_SWAP_CHAIN_DESC swapDesc;
            ZeroMemory(&swapDesc, sizeof(swapDesc));
            swapDesc.BufferDesc.Width = nWidth;
            swapDesc.BufferDesc.Height = nHeight;
            swapDesc.BufferDesc.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
            swapDesc.BufferDesc.RefreshRate.Numerator = 60;
            swapDesc.BufferDesc.RefreshRate.Denominator = 1;
            swapDesc.SampleDesc.Count = m_msaaSampleCount;
            swapDesc.SampleDesc.Quality = m_msaaQuality;
            swapDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
            swapDesc.BufferCount = 1;
            swapDesc.OutputWindow = m_hwnd;
            swapDesc.Windowed = TRUE;
            hr = pDXGIFactory1->CreateSwapChain(m_pD3D11Device, &swapDesc, &m_pD3D11SwapChain);
            if (FAILED(hr)) {
                LOG(SEVERITY_LEVEL_FATAL) << L"Creating swap chain was failed.";
                return hr;
            }
        }

        // Create Compute Shader Device
        hr = _CreateCSDevice(pDXGIAdapter1, 0, &m_pD3D11CSDevice, &m_pD3D11CSImmediateContext);
        if (FAILED(hr)) {
            LOG(SEVERITY_LEVEL_WARN) << L"Creating Compute Shader device was failed.";
            hr = S_OK;
        }

        // Create ID3D10Device1
        LOG(SEVERITY_LEVEL_INFO) << L"Try to create ID3D10Device1";
        D3D10_DRIVER_TYPE d3d10DriverTypeAttempt[] = {
            D3D10_DRIVER_TYPE_HARDWARE,
            D3D10_DRIVER_TYPE_WARP,
        };
        wchar_t* d3d10DriverTypeAttemptLabel[] = {
            L"D3D10_DRIVER_TYPE_HARDWARE",
            L"D3D10_DRIVER_TYPE_WARP",
        };
#ifdef _DEBUG
        nDeviceFlags = D3D10_CREATE_DEVICE_BGRA_SUPPORT |
                       D3D10_RESOURCE_MISC_SHARED_KEYEDMUTEX |
                       D3D10_CREATE_DEVICE_DEBUG;
#else
        nDeviceFlags = D3D10_CREATE_DEVICE_BGRA_SUPPORT |
                       D3D10_RESOURCE_MISC_SHARED_KEYEDMUTEX;
#endif
        for (size_t i = 0; i < ARRAYSIZE(d3d10DriverTypeAttempt); ++i) {
            hr = _CreateD3DDevice10(
                     pDXGIAdapter1,
                     d3d10DriverTypeAttempt[i],
                     nDeviceFlags,
                     &m_pD3D10Device
                 );
            if (SUCCEEDED(hr)) {
                LOG(SEVERITY_LEVEL_INFO) << L"driver type :" << d3d10DriverTypeAttemptLabel[i];
                break;
            }
        }
        if (FAILED(hr)) {
            LOG(SEVERITY_LEVEL_FATAL) << L"Creating D3D10 device was failed.";
            return hr;
        }
        if (SUCCEEDED(hr)) {
            m_pD3D10Device->IASetPrimitiveTopology(D3D10_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
        }
        if (SUCCEEDED(hr)) {
            hr = _CreateD3DDeviceResources11();
        }
        if (SUCCEEDED(hr)) {
            if (!m_pD3DInteropAdapter.expired()) {
                m_pD3DInteropAdapter.lock()->CreateD3DResources(
                    this, m_pD3D11Device, m_pD3D11ImmediateContext);
            }
        }
        if (SUCCEEDED(hr)) {
            if (!m_windowlessMode) {
                hr = _RecreateSizedResources11(nWidth, nHeight);
            }
        }
        if (SUCCEEDED(hr)) {
            if (!m_pD3DInteropAdapter.expired()) {
                m_pD3DInteropAdapter.lock()->CreateD2DResources(
                    this, m_pD2DFactory, m_pD2DTexture2DRenderTargetD3D11);
            }
        }


    }
#ifdef DEBUG_GRAPHICS_D3DINTEROPHELPER_
    LOG_LEAVE_HRESULT(SEVERITY_LEVEL_DEBUG, hr);
#endif
    return hr;
}

HRESULT graphics::D3DInteropHelper::_CreateD3DDeviceResources11()
{
#ifdef DEBUG_GRAPHICS_D3DINTEROPHELPER_
    LOG_ENTER(SEVERITY_LEVEL_DEBUG);
#endif

    HRESULT hr = S_OK;

    // *** Rasterizer ***
    D3D11_RASTERIZER_DESC rsDesc;
    rsDesc.FillMode = D3D11_FILL_SOLID;
    rsDesc.CullMode = D3D11_CULL_BACK;
    rsDesc.FrontCounterClockwise = FALSE;
    rsDesc.DepthBias = 0;
    rsDesc.DepthBiasClamp = 0;
    rsDesc.SlopeScaledDepthBias = 0;
    rsDesc.DepthClipEnable = TRUE;
    rsDesc.ScissorEnable = FALSE;
    rsDesc.MultisampleEnable = (m_msaaSampleCount > 1);
    rsDesc.AntialiasedLineEnable = FALSE;
    hr = m_pD3D11Device->CreateRasterizerState(&rsDesc, &m_pD3D11RasterizerState);
    if (FAILED(hr)) {
        LOG(SEVERITY_LEVEL_ERROR) << L"CreateRasterizerState failed, " << hr;
    }

#ifdef DEBUG_GRAPHICS_D3DINTEROPHELPER_
    LOG_LEAVE_HRESULT(SEVERITY_LEVEL_DEBUG, hr);
#endif
    return hr;
}

HRESULT graphics::D3DInteropHelper::_RecreateSizedResources11( UINT nWidth, UINT nHeight )
{
#ifdef DEBUG_GRAPHICS_D3DINTEROPHELPER_
    LOG_ENTER(SEVERITY_LEVEL_DEBUG);
#endif
    assert(nWidth > 0);
    assert(nHeight > 0);

    HRESULT hr = S_OK;
    SafeRelease(&m_pD3D11DepthStencilView);
    SafeRelease(&m_pD3D11DepthStencil);
    SafeRelease(&m_pD3D11BackBufferRenderTargetView);
    SafeRelease(&m_pD2DTexture2DRenderTargetD3D11);
    SafeRelease(&m_pD3D10KeyedMutex);
    SafeRelease(&m_pD3D10SharedSurface);
    SafeRelease(&m_pD3D11KeyedMutex);
    SafeRelease(&m_pD3D11SharedTexture);
    SafeRelease(&m_pD3D11BackBuffer);

    // *** Output Merger settings ***
    ID3D11RenderTargetView *viewList[1] = {
        nullptr,
    };
    m_pD3D11ImmediateContext->OMSetRenderTargets(1, viewList, nullptr);

    hr = m_pD3D11SwapChain->ResizeBuffers(1, nWidth, nHeight, DXGI_FORMAT_B8G8R8A8_UNORM, 0);
    if (FAILED(hr)) {
        LOG_HRESULT(SEVERITY_LEVEL_ERROR, hr) << L"resize the swap chain failed.";
    }

    CComPtr<ID3D11Resource> pBackBufferResource = nullptr;
    if (SUCCEEDED(hr)) {
        hr = m_pD3D11SwapChain->GetBuffer(0, IID_PPV_ARGS(&pBackBufferResource));
        if (FAILED(hr)) {
            LOG_HRESULT(SEVERITY_LEVEL_ERROR, hr) << L"getting a buffer from the swap chain was failed.";
        }
    }

    if (SUCCEEDED(hr)) {
        D3D11_RENDER_TARGET_VIEW_DESC renderDesc;
        renderDesc.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
        renderDesc.ViewDimension =
            (m_msaaSampleCount <= 1 ? D3D11_RTV_DIMENSION_TEXTURE2D : D3D11_RTV_DIMENSION_TEXTURE2DMS);
        renderDesc.Texture2D.MipSlice = 0;
        hr = m_pD3D11Device->CreateRenderTargetView(
                 pBackBufferResource,
                 &renderDesc,
                 &m_pD3D11BackBufferRenderTargetView);
        if (FAILED(hr)) {
            LOG(SEVERITY_LEVEL_FATAL) << L"creating a render target view was failed. hr = " << hr;
        }
    }

    // *** Depth Stencil ***
    if (SUCCEEDED(hr)) {
        D3D11_TEXTURE2D_DESC texDesc;
        ZeroMemory(&texDesc, sizeof(texDesc));
        texDesc.ArraySize = 1;
        texDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
        texDesc.CPUAccessFlags = 0;
        texDesc.Format = DXGI_FORMAT_D16_UNORM;
        texDesc.Width = nWidth;
        texDesc.Height = nHeight;
        texDesc.MipLevels = 1;
        texDesc.MiscFlags = 0;
        texDesc.SampleDesc.Count = m_msaaSampleCount;
        texDesc.SampleDesc.Quality = m_msaaQuality;
        texDesc.Usage = D3D11_USAGE_DEFAULT;

        hr = m_pD3D11Device->CreateTexture2D(&texDesc, nullptr, &m_pD3D11DepthStencil);
        if (FAILED(hr)) {
            LOG(SEVERITY_LEVEL_FATAL) << L"creating a depth stencil texture was failed. hr = " << hr;
        }
    }

    if (SUCCEEDED(hr)) {
        D3D11_DEPTH_STENCIL_VIEW_DESC depthViewDesc;
        ZeroMemory(&depthViewDesc, sizeof(depthViewDesc));
        depthViewDesc.Format = DXGI_FORMAT_D16_UNORM;
        depthViewDesc.ViewDimension =
            (m_msaaSampleCount <= 1 ? D3D11_DSV_DIMENSION_TEXTURE2D : D3D11_DSV_DIMENSION_TEXTURE2DMS);
        depthViewDesc.Texture2D.MipSlice = 0;
        hr = m_pD3D11Device->CreateDepthStencilView(
                 m_pD3D11DepthStencil,
                 &depthViewDesc,
                 &m_pD3D11DepthStencilView);
        if (FAILED(hr)) {
            LOG(SEVERITY_LEVEL_FATAL) << L"creating depth stencil view was failed. hr = " << hr;
        }
    }

    // *** set viewport ***
    if (SUCCEEDED(hr)) {
        m_viewport.TopLeftX = 0;
        m_viewport.TopLeftY = 0;
        m_viewport.Width = static_cast<FLOAT>(nWidth);
        m_viewport.Height = static_cast<FLOAT>(nHeight);
        m_viewport.MinDepth = 0;
        m_viewport.MaxDepth = 1;
        m_pD3D11ImmediateContext->RSSetViewports(1, &m_viewport);
    }

    // *** prepare D2D ***
    if (SUCCEEDED(hr)) {
        hr = m_pD3D11SwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (LPVOID*)&m_pD3D11BackBuffer);
        if (FAILED(hr)) {
            LOG(SEVERITY_LEVEL_ERROR) << "Getting the back buffer from the swap chain was failed, hr = " << hr;
        }
    }

    if (SUCCEEDED(hr)) {
        D3D11_TEXTURE2D_DESC sharedTexDesc;
        ZeroMemory(&sharedTexDesc, sizeof(sharedTexDesc));
        sharedTexDesc.Width = nWidth;
        sharedTexDesc.Height = nHeight;
        sharedTexDesc.MipLevels = 1;
        sharedTexDesc.ArraySize = 1;
        sharedTexDesc.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
        sharedTexDesc.SampleDesc.Count = 1;
        sharedTexDesc.SampleDesc.Quality = 0;
        sharedTexDesc.Usage = D3D11_USAGE_DEFAULT;
        sharedTexDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_RENDER_TARGET;
        sharedTexDesc.MiscFlags = D3D11_RESOURCE_MISC_SHARED_KEYEDMUTEX;

        hr = m_pD3D11Device->CreateTexture2D(&sharedTexDesc, nullptr, &m_pD3D11SharedTexture);
        if (FAILED(hr)) {
            LOG_HRESULT(SEVERITY_LEVEL_ERROR, hr) << L"CreateTexture2D was failed.";
        }
    }

    if (SUCCEEDED(hr)) {
        hr = m_pD3D11SharedTexture->QueryInterface(__uuidof(IDXGIKeyedMutex), (void**)&m_pD3D11KeyedMutex);
        if (FAILED(hr)) {
            LOG_HRESULT(SEVERITY_LEVEL_ERROR, hr) << L"could not obtain KeyedMutex from shared texture.";
        }
    }

    if (SUCCEEDED(hr)) {
        CComPtr<IDXGIResource> resource = nullptr;
        hr = m_pD3D11SharedTexture->QueryInterface(__uuidof(IDXGIResource), (void**)&resource);
        if (FAILED(hr)) {
            LOG_HRESULT(SEVERITY_LEVEL_ERROR, hr) << L"could not obtain IDXGIResource from shared texture.";
        }
        if (SUCCEEDED(hr)) {
            hr = resource->GetSharedHandle(&m_d3d11SharedHandle);
            if (FAILED(hr)) {
                LOG_HRESULT(SEVERITY_LEVEL_ERROR, hr) << L"could not obtain shared handle from shared texture.";
            }
        }
    }

    if (SUCCEEDED(hr)) {
        hr = m_pD3D10Device->OpenSharedResource(
                 m_d3d11SharedHandle,
                 __uuidof(IDXGISurface1),
                 (void**)&m_pD3D10SharedSurface);
        if (FAILED(hr)) {
            LOG(SEVERITY_LEVEL_ERROR) << L"OpenSharedResource failed, hr = " << hr;
        }
    }

    if (SUCCEEDED(hr)) {
        hr = m_pD3D10SharedSurface->QueryInterface(__uuidof(IDXGIKeyedMutex), (void**)&m_pD3D10KeyedMutex);
        if (FAILED(hr)) {
            LOG(SEVERITY_LEVEL_ERROR) << L"could not obtain IDXGIKeyedMutex from shared surface, hr = " << hr;
        }
    }

    if (SUCCEEDED(hr)) {
        FLOAT dpiX;
        FLOAT dpiY;
        m_pD2DFactory->GetDesktopDpi(&dpiX, &dpiY);

        D2D1_RENDER_TARGET_PROPERTIES props =
            D2D1::RenderTargetProperties(
                D2D1_RENDER_TARGET_TYPE_DEFAULT,
                D2D1::PixelFormat(DXGI_FORMAT_UNKNOWN, D2D1_ALPHA_MODE_PREMULTIPLIED),
                dpiX,
                dpiY
            );
        hr = m_pD2DFactory->CreateDxgiSurfaceRenderTarget(
                 m_pD3D10SharedSurface,
                 &props,
                 &m_pD2DTexture2DRenderTargetD3D11);
        if (FAILED(hr)) {
            LOG(SEVERITY_LEVEL_FATAL)
                    << L"creating a DXGI surface render target for Direct2D was failed. hr = " << hr;
        }
    }

    if (SUCCEEDED(hr)) {
        if (!m_pD3DInteropAdapter.expired()) {
            m_pD3DInteropAdapter.lock()->RecreateSizedResources(
                nWidth,
                nHeight,
                this,
                m_pD3D11Device,
                m_pD3D11ImmediateContext);
        }
    }

    // *** set back buffer render target and depth stencil view
    if (SUCCEEDED(hr)) {
        // color_dst = color_src x [SrcBlend] (BlendOp) color_dst x [DestBlend]
        // alpha_dst = alpha_src x [SrcBlendAlpha] (BlendOpAlpha) alpha_dst x [DestBlendAlpha]
        D3D11_BLEND_DESC blendDesc;
        ZeroMemory(&blendDesc, sizeof(blendDesc));
        blendDesc.IndependentBlendEnable = FALSE;
        blendDesc.RenderTarget[0].BlendEnable = TRUE;
        blendDesc.RenderTarget[0].SrcBlend = D3D11_BLEND_SRC_ALPHA;
        blendDesc.RenderTarget[0].DestBlend = D3D11_BLEND_INV_SRC_ALPHA;
        blendDesc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
        blendDesc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_SRC_ALPHA;
        blendDesc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_INV_SRC_ALPHA;
        blendDesc.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
        blendDesc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;

        hr = m_pD3D11Device->CreateBlendState(&blendDesc, &m_pD3D11BlendState);
        if (FAILED(hr)) {
            LOG_HRESULT(SEVERITY_LEVEL_ERROR, hr) << L"CreateBlendState failed";
        }
    }

#ifdef DEBUG_GRAPHICS_D3DINTEROPHELPER_
    LOG_LEAVE_HRESULT(SEVERITY_LEVEL_DEBUG, hr);
#endif
    return hr;
}

HRESULT graphics::D3DInteropHelper::OnRender()
{
#ifdef DEBUG_GRAPHICS_D3DINTEROPHELPER_
    LOG_ENTER(SEVERITY_LEVEL_DEBUG);
#endif
    HRESULT hr;
    hr = CreateDeviceResources(m_adapterIndex);

    if (SUCCEEDED(hr)) {
        // clear the back buffer
//         float clearColor[4] = {0.2f, 0.2f, 0.2f, 1.0f};
//         float clearColor[4] = {0.0f, 0.0f, 0.0f, 1.0f};
        m_pD3D11ImmediateContext->ClearRenderTargetView(m_pD3D11BackBufferRenderTargetView, m_backgroundColor);
        // clear the depth buffer to 1.0 (max depth)
        m_pD3D11ImmediateContext->ClearDepthStencilView(m_pD3D11DepthStencilView, D3D11_CLEAR_DEPTH, 1.0f, 0);

        m_pD3D11ImmediateContext->RSSetState(m_pD3D11RasterizerState);
        m_pD3D11ImmediateContext->RSSetViewports(1, &m_viewport);

        FLOAT blendFactor[4] = {0.0f, 0.0f, 0.0f, 0.0f};
        m_pD3D11ImmediateContext->OMSetBlendState(nullptr, blendFactor, 0xffffffff);

        ID3D11RenderTargetView *viewList[1] = {
            m_pD3D11BackBufferRenderTargetView,
        };
        m_pD3D11ImmediateContext->OMSetRenderTargets(1, viewList, m_pD3D11DepthStencilView);

        // Draw 2D
        if (m_pD2DTexture2DRenderTargetD3D11 != nullptr && !m_pD3DInteropAdapter.expired()) {
            m_pD3D10KeyedMutex->AcquireSync(0, INFINITE);
            m_pD3DInteropAdapter.lock()->Draw2D(this, m_pD2DTexture2DRenderTargetD3D11);
            m_pD3D10KeyedMutex->ReleaseSync(1);
        }

        // Draw 3D
        if (SUCCEEDED(hr)) {
            m_pD3D11KeyedMutex->AcquireSync(1, INFINITE);
            if (!m_pD3DInteropAdapter.expired()) {
                m_pD3DInteropAdapter.lock()->Draw3D(this, m_pD3D11Device, m_pD3D11ImmediateContext);
            }

            // Blend 2D
            if (SUCCEEDED(hr)) {
                FLOAT blendFactor[4] = {0.0f, 0.0f, 0.0f, 0.0f};
                m_pD3D11ImmediateContext->OMSetBlendState(m_pD3D11BlendState, blendFactor, 0xffffffff);
            }
            Blend(m_pD3D11SharedTexture);
            m_pD3D11KeyedMutex->ReleaseSync(0);

            // Display
            hr = m_pD3D11SwapChain->Present(1, 0);
        }

        // clear render target views
        {
            ID3D11RenderTargetView *viewListD3D11[1] = {nullptr};
            m_pD3D11ImmediateContext->OMSetRenderTargets(1, viewListD3D11, nullptr);
            ID3D10RenderTargetView *viewListD3D10[1] = {nullptr};
            m_pD3D10Device->OMSetRenderTargets(1, viewListD3D10, nullptr);
        }
    }

    if (hr == DXGI_ERROR_DEVICE_RESET ||
            hr == DXGI_ERROR_DEVICE_REMOVED ||
            hr == D2DERR_RECREATE_TARGET) {
        hr = S_OK;

        DiscardDeviceResources();
    }
#ifdef DEBUG_GRAPHICS_D3DINTEROPHELPER_
    LOG_LEAVE_HRESULT(SEVERITY_LEVEL_DEBUG, hr);
#endif
    return hr;
}

void graphics::D3DInteropHelper::OnResize(UINT width, UINT height)
{
#ifdef DEBUG_GRAPHICS_D3DINTEROPHELPER_
    LOG_ENTER(SEVERITY_LEVEL_DEBUG);
#endif
    if (!m_pD3D11Device) {
        CreateDeviceResources(m_adapterIndex);
    } else if (width > 0 && height > 0) {
        _RecreateSizedResources11(width, height);
    }
#ifdef DEBUG_GRAPHICS_D3DINTEROPHELPER_
    LOG_LEAVE(SEVERITY_LEVEL_DEBUG);
#endif
}

HRESULT graphics::D3DInteropHelper::_CreateD3DDevice11(
    IDXGIAdapter1 *pAdapter,
    D3D_DRIVER_TYPE driverType,
    UINT flags,
    OUT ID3D11Device **ppDevice,
    OUT ID3D11DeviceContext **ppImmediateContext)
{
#ifdef DEBUG_GRAPHICS_D3DINTEROPHELPER_
    LOG_ENTER(SEVERITY_LEVEL_DEBUG);
#endif
    HRESULT hr = S_OK;

    static const D3D_FEATURE_LEVEL levelAttempts[] = {
        D3D_FEATURE_LEVEL_11_0,
        D3D_FEATURE_LEVEL_10_1,
        D3D_FEATURE_LEVEL_10_0,
        D3D_FEATURE_LEVEL_9_3,
    };
    static const wchar_t *levelLabels[] = {
        L"D3D_FEATURE_LEVEL_11_0",
        L"D3D_FEATURE_LEVEL_10_1",
        L"D3D_FEATURE_LEVEL_10_0",
        L"D3D_FEATURE_LEVEL_9_3",
    };

    ID3D11Device *pDevice = nullptr;
    D3D_FEATURE_LEVEL featureLevel;
    ID3D11DeviceContext *pImmediateContext = nullptr;
    hr = D3D11CreateDevice(
             pAdapter,
             driverType,
             nullptr,
             flags,
             levelAttempts,
             ARRAYSIZE(levelAttempts),
             D3D11_SDK_VERSION,
             &pDevice,
             &featureLevel,
             &pImmediateContext
         );


    if (SUCCEEDED(hr)) {
        *ppDevice = pDevice;
        pDevice = nullptr;
        *ppImmediateContext = pImmediateContext;
        pImmediateContext = nullptr;
        for (size_t i = 0; i < ARRAYSIZE(levelAttempts); ++i) {
            if (featureLevel == levelAttempts[i]) {
                LOG(SEVERITY_LEVEL_INFO) << L"found " << levelLabels[i] << L" device";
                break;
            }
        }
    }

#ifdef DEBUG_GRAPHICS_D3DINTEROPHELPER_
    LOG_LEAVE_HRESULT(SEVERITY_LEVEL_DEBUG, hr);
#endif
    return hr;
}

HRESULT graphics::D3DInteropHelper::_CreateD3DDevice10(
    IDXGIAdapter *pAdapter,
    D3D10_DRIVER_TYPE driverType,
    UINT flags,
    ID3D10Device1 **ppDevice
)
{
#ifdef DEBUG_GRAPHICS_D3DINTEROPHELPER_
    LOG_ENTER(SEVERITY_LEVEL_DEBUG);
#endif
    HRESULT hr = S_OK;

    static const D3D10_FEATURE_LEVEL1 levelAttempts[] = {
        D3D10_FEATURE_LEVEL_10_1,
        D3D10_FEATURE_LEVEL_10_0,
        D3D10_FEATURE_LEVEL_9_3,
    };
    static const wchar_t *levelLabels[] = {
        L"D3D10_FEATURE_LEVEL_10_1",
        L"D3D10_FEATURE_LEVEL_10_0",
        L"D3D10_FEATURE_LEVEL_9_3",
    };

    for (UINT level = 0; level < ARRAYSIZE(levelAttempts); level++) {
        ID3D10Device1 *pDevice = nullptr;
        hr = D3D10CreateDevice1(
                 pAdapter,
                 driverType,
                 nullptr,
                 flags,
                 levelAttempts[level],
                 D3D10_1_SDK_VERSION,
                 &pDevice
             );

        if (SUCCEEDED(hr)) {
            *ppDevice = pDevice;
            pDevice = nullptr;
            LOG(SEVERITY_LEVEL_INFO) << L"found " << levelLabels[level] << L" device";
            break;
        }
    }

#ifdef DEBUG_GRAPHICS_D3DINTEROPHELPER_
    LOG_LEAVE_HRESULT(SEVERITY_LEVEL_DEBUG, hr);
#endif
    return hr;
}

HRESULT graphics::D3DInteropHelper::CreateDeviceIndependentResources()
{
    HRESULT hr;

    D2D1_FACTORY_OPTIONS factoryOptions;
#ifdef _DEBUG
    factoryOptions.debugLevel = D2D1_DEBUG_LEVEL_INFORMATION;
#else
    factoryOptions.debugLevel = D2D1_DEBUG_LEVEL_NONE;
#endif
    hr = D2D1CreateFactory(D2D1_FACTORY_TYPE_MULTI_THREADED, factoryOptions, &m_pD2DFactory);
#ifdef DEBUG_RESOURCE
    LOG_RESOURCE(SEVERITY_LEVEL_DEBUG, m_pD2DFactory);
#endif
    if (SUCCEEDED(hr)) {
        // Create a WIC factory
        hr = CoCreateInstance(
                 CLSID_WICImagingFactory,
                 nullptr,
                 CLSCTX_INPROC_SERVER,
                 IID_IWICImagingFactory,
                 reinterpret_cast<void **>(&m_pWICFactory)
             );
#ifdef DEBUG_RESOURCE
        LOG_RESOURCE(SEVERITY_LEVEL_DEBUG, m_pWICFactory);
#endif
    }
    if (SUCCEEDED(hr)) {
        // Create DWrite factory
        hr = DWriteCreateFactory(
                 DWRITE_FACTORY_TYPE_SHARED,
                 __uuidof(m_pDWriteFactory),
                 reinterpret_cast<IUnknown **>(&m_pDWriteFactory)
             );
#ifdef DEBUG_RESOURCE
        LOG_RESOURCE(SEVERITY_LEVEL_DEBUG, m_pDWriteFactory);
#endif
    }
    if (SUCCEEDED(hr) && !m_pD3DInteropAdapter.expired()) {
        hr = m_pD3DInteropAdapter.lock()->CreateDeviceIndependentResources(
                 this, m_pD2DFactory, m_pWICFactory, m_pDWriteFactory);
    }

    hr = _InitializeAnimation();
    return hr;
}

HRESULT graphics::D3DInteropHelper::_CreateVertexAndIndexBufferFor2D()
{
    HRESULT hr = S_OK;

    if (SUCCEEDED(hr) && m_pD2DVertexBuffer == nullptr) {
        D3D11_BUFFER_DESC bd;
        bd.Usage = D3D11_USAGE_DEFAULT;
        bd.ByteWidth = sizeof(s_D2DVertexArrayForBlend);
        bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
        bd.CPUAccessFlags = 0;
        bd.MiscFlags = 0;
        D3D11_SUBRESOURCE_DATA initData;
        initData.pSysMem = s_D2DVertexArrayForBlend;

        hr = m_pD3D11Device->CreateBuffer(&bd, &initData, &m_pD2DVertexBuffer);
#ifdef DEBUG_RESOURCE
        LOG_RESOURCE(SEVERITY_LEVEL_DEBUG, m_pD2DVertexBuffer);
#endif
    }
    if (SUCCEEDED(hr) && m_pD2DIndexBuffer == nullptr) {
        D3D11_BUFFER_DESC bd;
        bd.Usage = D3D11_USAGE_DEFAULT;
        bd.ByteWidth = sizeof(s_D2DIndexArrayForBlend);
        bd.BindFlags = D3D11_BIND_INDEX_BUFFER;
        bd.CPUAccessFlags = 0;
        bd.MiscFlags = 0;
        D3D11_SUBRESOURCE_DATA initData;
        initData.pSysMem = s_D2DIndexArrayForBlend;

        hr = m_pD3D11Device->CreateBuffer(&bd, &initData, &m_pD2DIndexBuffer);
#ifdef DEBUG_RESOURCE
        LOG_RESOURCE(SEVERITY_LEVEL_DEBUG, m_pD2DIndexBuffer);
#endif
    }
    return hr;
}

HRESULT graphics::D3DInteropHelper::ListAdapters( OUT std::vector<std::wstring> adapterDescriptions )
{
    HRESULT hr = S_OK;

    CComPtr<IDXGIFactory1> pDXGIFactory = nullptr;
    hr = CreateDXGIFactory(__uuidof(IDXGIFactory1), (void**)&pDXGIFactory);
    if (FAILED(hr)) {
        LOG(SEVERITY_LEVEL_ERROR) << L"CreateDXGIFactory failed";
        return hr;
    }

    UINT i = 0;
    IDXGIAdapter1 *pAdapter = nullptr;
    while (pDXGIFactory->EnumAdapters1(i, &pAdapter) != DXGI_ERROR_NOT_FOUND) {
        DXGI_ADAPTER_DESC1 adapterDesc;
        ZeroMemory(&adapterDesc, sizeof(adapterDesc));
        hr = pAdapter->GetDesc1(&adapterDesc);
        if (FAILED(hr)) {
            LOG(SEVERITY_LEVEL_ERROR) << L"IDXGIAdapter1::GetDesc1 failed";
            return hr;
        }
        adapterDescriptions.push_back(std::wstring(adapterDesc.Description));
        ++i;
    }
    return S_OK;
}

HRESULT graphics::D3DInteropHelper::CreateSharedSurface(
    UINT width,
    UINT height,
    OUT std::shared_ptr<graphics::SharedSurface> &sharedSurface)
{
    HRESULT hr = S_OK;
    auto _sharedSurface = std::make_shared<graphics::SharedSurface>();
    hr = _sharedSurface->Initialize(width, height, m_pD3D11Device, m_pD3D10Device, m_pD2DFactory);
    if (SUCCEEDED(hr)) {
        sharedSurface = _sharedSurface;
    } else {
        LOG_HRESULT(SEVERITY_LEVEL_ERROR, hr) << L"could not create a shared surface.";
    }
    return hr;
}

HRESULT graphics::D3DInteropHelper::Blend(ID3D11Texture2D *pTexture)
{
#ifdef DEBUG_GRAPHICS_D3DINTEROPHELPER_
    LOG_ENTER(SEVERITY_LEVEL_DEBUG);
#endif
    HRESULT hr = S_OK;

    // *** Vertex Shader ***
    if (m_pVertexShaderForBlend == nullptr) {
        CComPtr<ID3DBlob> pVSBlob = nullptr;
        hr = CompileShaderFromResource(MAKEINTRESOURCE(IDR_SHADER_BLEND), "VS", "vs_4_0", &pVSBlob);
        if (FAILED(hr)) {
            LOG(SEVERITY_LEVEL_ERROR) << L"LoadResourceShader(VS) failed, hr = " << hr;
            return hr;
        }
        hr = m_pD3D11Device->CreateVertexShader(
                 pVSBlob->GetBufferPointer(),
                 pVSBlob->GetBufferSize(),
                 nullptr,
                 &m_pVertexShaderForBlend);
        if (FAILED( hr )) {
            LOG(SEVERITY_LEVEL_ERROR) << L"CreateVertexShader failed, hr = " << hr;
            return hr;
        }

        // *** Input Layout ***
        if (m_pVertexLayoutForBlend == nullptr) {
            hr = m_pD3D11Device->CreateInputLayout(
                     s_D2DInputLayoutForBlend,
                     ARRAYSIZE(s_D2DInputLayoutForBlend),
                     pVSBlob->GetBufferPointer(),
                     pVSBlob->GetBufferSize(),
                     &m_pVertexLayoutForBlend);
            if (FAILED( hr )) {
                LOG(SEVERITY_LEVEL_ERROR) << L"CreateInputLayout failed, hr = " << hr;
                return hr;
            }
        }
    }

    // *** Pixel Shader ***
    if (m_pPixelShaderForBlend == nullptr) {
        CComPtr<ID3DBlob> pPSBlob = nullptr;
        hr = CompileShaderFromResource(MAKEINTRESOURCE(IDR_SHADER_BLEND), "PS", "ps_4_0", &pPSBlob);
        if (FAILED(hr)) {
            LOG(SEVERITY_LEVEL_ERROR) << L"LoadResourceShader(PS) failed, hr = " << hr;
            return hr;
        }
        hr = m_pD3D11Device->CreatePixelShader(
                 pPSBlob->GetBufferPointer(),
                 pPSBlob->GetBufferSize(),
                 nullptr,
                 &m_pPixelShaderForBlend);
        if (FAILED( hr )) {
            LOG(SEVERITY_LEVEL_ERROR) << L"CreatePixelShader failed, hr = " << hr;
            return hr;
        }
    }

    // Create the sample state
    if (m_pSamplerLinearForBlend == nullptr) {
        D3D11_SAMPLER_DESC sampDesc;
        ZeroMemory( &sampDesc, sizeof(sampDesc) );
        sampDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
        sampDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
        sampDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
        sampDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
        sampDesc.ComparisonFunc = D3D11_COMPARISON_NEVER;
        sampDesc.MinLOD = 0;
        sampDesc.MaxLOD = D3D11_FLOAT32_MAX;
        hr = m_pD3D11Device->CreateSamplerState( &sampDesc, &m_pSamplerLinearForBlend );
        if ( FAILED( hr ) ) {
            LOG_HRESULT(SEVERITY_LEVEL_ERROR, hr) << L"CreateSamplerState failed.";
            return hr;
        }
    }

    // *** Texture ***
    D3D11_SHADER_RESOURCE_VIEW_DESC desc;
    ZeroMemory(&desc, sizeof(desc));
    desc.Format = DXGI_FORMAT_UNKNOWN;
    desc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
    desc.Texture2D.MipLevels = 1;
    desc.Texture2D.MostDetailedMip = 0;
    SafeRelease(&m_pTextureRVForBlend);
    hr = m_pD3D11Device->CreateShaderResourceView(pTexture, &desc, &m_pTextureRVForBlend);
    if (FAILED( hr )) {
        LOG_HRESULT(SEVERITY_LEVEL_ERROR, hr) << L"CreateShaderResourceView failed.";
        return hr;
    }

    D3D11_TEXTURE2D_DESC texDesc;
    ZeroMemory(&texDesc, sizeof(texDesc));
    pTexture->GetDesc(&texDesc);

    hr = _CreateVertexAndIndexBufferFor2D();
    // Set vertex buffer
    UINT stride = sizeof(graphics::D2DVertex);
    UINT offset = 0;
    ID3D11Buffer *pVertexBuffer = m_pD2DVertexBuffer;
    m_pD3D11ImmediateContext->IASetVertexBuffers(
        0, // StartSlot
        1, // NumBuffers
        &pVertexBuffer,
        &stride,
        &offset
    );
    m_pD3D11ImmediateContext->IASetIndexBuffer(m_pD2DIndexBuffer, DXGI_FORMAT_R16_UINT, 0);
    m_pD3D11ImmediateContext->IASetInputLayout(m_pVertexLayoutForBlend);
    m_pD3D11ImmediateContext->VSSetShader( m_pVertexShaderForBlend, nullptr, 0 );
    m_pD3D11ImmediateContext->PSSetShader( m_pPixelShaderForBlend, nullptr, 0 );
    m_pD3D11ImmediateContext->PSSetShaderResources( 0, 1, &m_pTextureRVForBlend );
    m_pD3D11ImmediateContext->PSSetSamplers( 0, 1, &m_pSamplerLinearForBlend );
    m_pD3D11ImmediateContext->DrawIndexed( ARRAYSIZE(s_D2DIndexArrayForBlend), 0, 0 );

#ifdef DEBUG_GRAPHICS_D3DINTEROPHELPER_
    LOG_LEAVE_HRESULT(SEVERITY_LEVEL_DEBUG, hr);
#endif
    return hr;
}




// **************************************************************************************************

HRESULT graphics::D3DInteropHelper::LoadResourceBitmap(
    ID2D1RenderTarget *pRenderTarget,
    IWICImagingFactory *pIWICFactory,
    PCWSTR resourceName,
    PCWSTR resourceType,
    UINT destinationWidth,
    UINT destinationHeight,
    ID2D1Bitmap **ppBitmap
)
{
    HRESULT hr = S_OK;
    IWICBitmapDecoder *pDecoder = nullptr;
    IWICBitmapFrameDecode *pSource = nullptr;
    IWICStream *pStream = nullptr;
    IWICFormatConverter *pConverter = nullptr;
    IWICBitmapScaler *pScaler = nullptr;

    HRSRC imageResHandle = nullptr;
    HGLOBAL imageResDataHandle = nullptr;
    void *pImageFile = nullptr;
    DWORD imageFileSize = 0;

    imageResHandle = FindResource(
                         HINST_THISCOMPONENT,
                         resourceName,
                         resourceType
                     );
    hr = imageResHandle ? S_OK : E_FAIL;
    if (SUCCEEDED(hr)) {
        imageResDataHandle = LoadResource(HINST_THISCOMPONENT, imageResHandle);

        hr = imageResDataHandle ? S_OK : E_FAIL;
    }
    if (SUCCEEDED(hr)) {
        pImageFile = LockResource(imageResDataHandle);

        hr = pImageFile ? S_OK : E_FAIL;
    }
    if (SUCCEEDED(hr)) {
        imageFileSize = SizeofResource(HINST_THISCOMPONENT, imageResHandle);

        hr = imageFileSize ? S_OK : E_FAIL;
    }
    if (SUCCEEDED(hr)) {
        hr = pIWICFactory->CreateStream(&pStream);
#ifdef DEBUG_RESOURCE
        LOG_RESOURCE(SEVERITY_LEVEL_DEBUG, pStream);
#endif
    }
    if (SUCCEEDED(hr)) {
        hr = pStream->InitializeFromMemory(
                 reinterpret_cast<BYTE*>(pImageFile),
                 imageFileSize
             );
    }
    if (SUCCEEDED(hr)) {
        hr = pIWICFactory->CreateDecoderFromStream(
                 pStream,
                 nullptr,
                 WICDecodeMetadataCacheOnLoad,
                 &pDecoder
             );
#ifdef DEBUG_RESOURCE
        LOG_RESOURCE(SEVERITY_LEVEL_DEBUG, pDecoder);
#endif
    }
    if (SUCCEEDED(hr)) {
        hr = pDecoder->GetFrame(0, &pSource);
    }
    if (SUCCEEDED(hr)) {
        hr = pIWICFactory->CreateFormatConverter(&pConverter);
#ifdef DEBUG_RESOURCE
        LOG_RESOURCE(SEVERITY_LEVEL_DEBUG, pConverter);
#endif
    }
    if (SUCCEEDED(hr)) {
        if (destinationWidth != 0 || destinationHeight != 0) {
            UINT originalWidth, originalHeight;
            hr = pSource->GetSize(&originalWidth, &originalHeight);
            if (SUCCEEDED(hr)) {
                if (destinationWidth == 0) {
                    FLOAT scalar = static_cast<FLOAT>(destinationHeight) / static_cast<FLOAT>(originalHeight);
                    destinationWidth = static_cast<UINT>(scalar * static_cast<FLOAT>(originalWidth));
                } else if (destinationHeight == 0) {
                    FLOAT scalar = static_cast<FLOAT>(destinationWidth) / static_cast<FLOAT>(originalWidth);
                    destinationHeight = static_cast<UINT>(scalar * static_cast<FLOAT>(originalHeight));
                }

                hr = pIWICFactory->CreateBitmapScaler(&pScaler);
#ifdef DEBUG_RESOURCE
                LOG_RESOURCE(SEVERITY_LEVEL_DEBUG, pScaler);
#endif
                if (SUCCEEDED(hr)) {
                    hr = pScaler->Initialize(
                             pSource,
                             destinationWidth,
                             destinationHeight,
                             WICBitmapInterpolationModeCubic
                         );
                    if (SUCCEEDED(hr)) {
                        hr = pConverter->Initialize(
                                 pScaler,
                                 GUID_WICPixelFormat32bppPBGRA,
                                 WICBitmapDitherTypeNone,
                                 nullptr,
                                 0.f,
                                 WICBitmapPaletteTypeMedianCut
                             );
                    }
                }
            }
        } else {
            hr = pConverter->Initialize(
                     pSource,
                     GUID_WICPixelFormat32bppPBGRA,
                     WICBitmapDitherTypeNone,
                     nullptr,
                     0.f,
                     WICBitmapPaletteTypeMedianCut
                 );
        }
    }
    if (SUCCEEDED(hr)) {
        hr = pRenderTarget->CreateBitmapFromWicBitmap(pConverter, nullptr, ppBitmap);
#ifdef DEBUG_RESOURCE
        LOG_RESOURCE(SEVERITY_LEVEL_DEBUG, ppBitmap);
#endif
    }

    SafeRelease(&pDecoder);
    SafeRelease(&pSource);
    SafeRelease(&pStream);
    SafeRelease(&pConverter);
    SafeRelease(&pScaler);

    return hr;
}

HRESULT graphics::D3DInteropHelper::LoadBitmapFile(
    ID2D1RenderTarget *pRenderTarget,
    IWICImagingFactory *pIWICFactory,
    const wchar_t* fileName,
    ID2D1Bitmap ** ppBitmap )
{
    HRESULT hr = S_OK;

    CComPtr<IWICBitmapDecoder> decoder = nullptr;
    hr = pIWICFactory->CreateDecoderFromFilename(
             fileName,
             nullptr,
             GENERIC_READ,
             WICDecodeMetadataCacheOnDemand,
             &decoder
         );
    CComPtr<IWICBitmapFrameDecode> frame = nullptr;
    if (SUCCEEDED(hr)) {
        hr = decoder->GetFrame(0, &frame);
    }
    CComPtr<IWICFormatConverter> formatConverter = nullptr;
    if (SUCCEEDED(hr)) {
        pIWICFactory->CreateFormatConverter(&formatConverter);
    }
    if (SUCCEEDED(hr)) {
        hr = formatConverter->Initialize(
                 frame, GUID_WICPixelFormat32bppPBGRA,
                 WICBitmapDitherTypeNone,
                 nullptr,
                 0.0f,
                 WICBitmapPaletteTypeCustom);
    }
    if (SUCCEEDED(hr)) {
        hr = pRenderTarget->CreateBitmapFromWicBitmap(formatConverter, nullptr, ppBitmap);
    }

    return S_OK;
}

HRESULT graphics::D3DInteropHelper::CompileShaderFromResource(
    ID3D10Device *pDevice,
    PCWSTR pszResource,
    ID3D10Effect **ppShader)
{
    HRESULT hr;

    HRSRC hResource = ::FindResource(HINST_THISCOMPONENT, pszResource, RT_RCDATA);
    hr = hResource ? S_OK : E_FAIL;

    if (SUCCEEDED(hr)) {
        HGLOBAL hResourceData = ::LoadResource(HINST_THISCOMPONENT, hResource);
        hr = hResourceData ? S_OK : E_FAIL;

        if (SUCCEEDED(hr)) {
            LPVOID pData = ::LockResource(hResourceData);
            hr = pData ? S_OK : E_FAIL;

            if (SUCCEEDED(hr)) {
                hr = ::D3D10CreateEffectFromMemory(
                         pData,
                         ::SizeofResource(HINST_THISCOMPONENT, hResource),
                         0,
                         pDevice,
                         nullptr,
                         ppShader
                     );
            }
        }
    }

    return hr;
}

HRESULT graphics::D3DInteropHelper::CompileShaderFromResource(
    PCWSTR pszResource,
    LPCSTR szEntryPoint,
    LPCSTR szShaderModel,
    OUT ID3DBlob **ppBlob,
    OUT std::wstring *errorMessage)
{
    HRESULT hr = S_OK;

    DWORD dwShaderFlags = D3DCOMPILE_ENABLE_STRICTNESS;
#if defined( DEBUG ) || defined( _DEBUG )
    dwShaderFlags |= D3DCOMPILE_DEBUG;
#endif

    ID3DBlob* pErrorBlob = nullptr;

    HMODULE hModule = GetModuleHandle(NULL);
    HRSRC hRes = FindResource(hModule, pszResource, RT_RCDATA);
    HGLOBAL hMem = LoadResource(hModule, hRes);
    LPVOID lpResource = LockResource(hMem);
    DWORD size = SizeofResource(hModule, hRes);
    std::vector<uint8_t> data(size);
    memcpy(data.data(), lpResource, size);
    FreeResource(hMem);
    /*
    hr = D3DX11CompileFromResource(
        HINST_THISCOMPONENT,
        pszResource,
        L"LoadResourceShader",
        nullptr,
        nullptr,
        szEntryPoint,
        szShaderModel,
        dwShaderFlags,
        0,
        nullptr,
        ppBlob,
        &pErrorBlob,
        nullptr);*/
    hr = D3DCompile(data.data(),
        size,
        "LoadResourceShader",
        nullptr,
        nullptr,
        szEntryPoint,
        szShaderModel,
        dwShaderFlags,
        0,
        ppBlob,
        &pErrorBlob);

    if (FAILED(hr)) {
        LOG(SEVERITY_LEVEL_ERROR) << L"D3DX11CompileFromResource failed, hr = " << hr;
        if (pErrorBlob != nullptr) {
            char *errMsg = reinterpret_cast<char*>(pErrorBlob->GetBufferPointer());
            OutputDebugStringA(errMsg);
            if (errorMessage != nullptr) {
                for (size_t i = 0; i < pErrorBlob->GetBufferSize(); ++i) {
                    errorMessage->push_back(static_cast<wchar_t>(errMsg[i]));
                }
            }
			std::string _errMsg(errMsg, pErrorBlob->GetBufferSize());
			LOG(SEVERITY_LEVEL_ERROR) << _errMsg;
        }
        if (pErrorBlob) {
            pErrorBlob->Release();
        }
        return hr;
    }
    if (pErrorBlob) {
        pErrorBlob->Release();
    }
    return hr;
}

HRESULT graphics::D3DInteropHelper::CompileShaderFromFile(
    WCHAR* szFileName,
    LPCSTR szEntryPoint,
    LPCSTR szShaderModel,
    OUT ID3DBlob** ppBlobOut,
    OUT std::wstring *errorMessage)
{
    HRESULT hr = S_OK;

    DWORD dwShaderFlags = D3DCOMPILE_ENABLE_STRICTNESS;
#if defined( DEBUG ) || defined( _DEBUG )
    dwShaderFlags |= D3DCOMPILE_DEBUG;
#endif

    ID3DBlob* pErrorBlob;
    /*hr = D3DX11CompileFromFile(
             szFileName,
             nullptr,
             nullptr,
             szEntryPoint,
             szShaderModel,
             dwShaderFlags,
             0,
             nullptr,
             ppBlobOut,
             &pErrorBlob,
             nullptr);*/

    hr = D3DCompileFromFile(szFileName,
        nullptr,
        nullptr,
        szEntryPoint,
        szShaderModel,
        dwShaderFlags,
        0,
        ppBlobOut,
        &pErrorBlob);

    if (FAILED(hr)) {
        LOG(SEVERITY_LEVEL_ERROR) << L"D3DX11CompileFromFile failed, hr = " << hr;
        if (pErrorBlob != nullptr) {
            char *errMsg = reinterpret_cast<char*>(pErrorBlob->GetBufferPointer());
            OutputDebugStringA(errMsg);
            if (errorMessage != nullptr) {
                for (size_t i = 0; i < pErrorBlob->GetBufferSize(); ++i) {
                    errorMessage->push_back(static_cast<wchar_t>(errMsg[i]));
                }
            }
        }
        if (pErrorBlob) {
            pErrorBlob->Release();
        }
        return hr;
    }
    if (pErrorBlob) {
        pErrorBlob->Release();
    }
    return S_OK;
}

HRESULT graphics::D3DInteropHelper::_InitializeAnimation()
{
    HRESULT hr = CoCreateInstance(
                     CLSID_UIAnimationManager,
                     nullptr,
                     CLSCTX_INPROC_SERVER,
                     IID_PPV_ARGS(&m_pAnimationManager));
    if (SUCCEEDED(hr)) {
        hr = CoCreateInstance(
                 CLSID_UIAnimationTimer,
                 nullptr,
                 CLSCTX_INPROC_SERVER,
                 IID_PPV_ARGS(&m_pAnimationTimer));
    }
    if (SUCCEEDED(hr)) {
        hr = CoCreateInstance(
                 CLSID_UIAnimationTransitionLibrary,
                 nullptr,
                 CLSCTX_INPROC_SERVER,
                 IID_PPV_ARGS(&m_pTransitionLibrary));
    }
    IUIAnimationTimerUpdateHandler *pTimerUpdateHandler = nullptr;
    if (SUCCEEDED(hr)) {
        hr = m_pAnimationManager->QueryInterface(IID_PPV_ARGS(&pTimerUpdateHandler));
    }
    if (SUCCEEDED(hr)) {
        hr = m_pAnimationTimer->SetTimerUpdateHandler(
                 pTimerUpdateHandler, UI_ANIMATION_IDLE_BEHAVIOR_CONTINUE);  // UI_ANIMATION_IDLE_BEHAVIOR_DISABLE
        pTimerUpdateHandler->Release();
    }
    IUIAnimationManagerEventHandler* pAnimationManagerEventHandler = nullptr;
    if (SUCCEEDED(hr)) {
        hr = animation::AnimationManagerEventHandler::CreateInstance(
                 m_animationWindow,
                 &pAnimationManagerEventHandler);
    }
    if (SUCCEEDED(hr)) {
        hr = m_pAnimationManager->SetManagerEventHandler(pAnimationManagerEventHandler);
        pAnimationManagerEventHandler->Release();
    }

    m_pAnimationTimer->Enable();
    return hr;
}

void graphics::D3DInteropHelper::PushMatrix(
    ID2D1RenderTarget *pRenderTarget,
    const D2D1_MATRIX_3X2_F &matrix )
{
    D2D1_MATRIX_3X2_F origMatrix;
    pRenderTarget->GetTransform(&origMatrix);
    m_matrixQueue.push(origMatrix);
    D2D1_MATRIX_3X2_F newMatrix = origMatrix * matrix;
    pRenderTarget->SetTransform(newMatrix);
}

void graphics::D3DInteropHelper::PopMatrix(ID2D1RenderTarget *pRenderTarget)
{
    D2D1_MATRIX_3X2_F &origMatrix = m_matrixQueue.top();
    pRenderTarget->SetTransform(origMatrix);
    m_matrixQueue.pop();
}

void graphics::D3DInteropHelper::RegisterGeometryFilter(
    const std::wstring &geometryFilterName,
    std::shared_ptr<graphics::filter::IGeometryFilter> geometryFilter )
{
    if (m_geometryFilterMap.count(geometryFilterName) == 0) {
        m_geometryFilterMap.insert(
            std::pair<std::wstring, std::shared_ptr<graphics::filter::IGeometryFilter>>(
                geometryFilterName,
                geometryFilter));
    }
}

std::shared_ptr<graphics::filter::IGeometryFilter> graphics::D3DInteropHelper::GetGeometryFilter(
    const std::wstring &geometryFilterName ) const
{
    return m_geometryFilterMap.at(geometryFilterName);
}

void graphics::D3DInteropHelper::RegisterTextureFilter(
    const std::wstring &textureFilterName,
    std::shared_ptr<graphics::filter::ITextureFilter> textureFilter )
{
    if (m_textureFilterMap.count(textureFilterName) == 0) {
        m_textureFilterMap.insert(
            std::pair<std::wstring, std::shared_ptr<graphics::filter::ITextureFilter>>(
                textureFilterName,
                textureFilter
            ));
    }
}

std::shared_ptr<graphics::filter::ITextureFilter> graphics::D3DInteropHelper::GetTextureFilter(
    const std::wstring &textureFilterName ) const
{
    return m_textureFilterMap.at(textureFilterName);
}

HRESULT graphics::D3DInteropHelper::DetectMSAA()
{
    static UINT sampleCounts[] = {4, 2};
    m_msaaSampleCount = 1;
    m_msaaQuality = 0;

    for (size_t i = 0; i < ARRAYSIZE(sampleCounts); ++i) {
        UINT numQualityLevels = 0;
        HRESULT hr = m_pD3D11Device->CheckMultisampleQualityLevels(
                         DXGI_FORMAT_B8G8R8A8_UNORM,
                         sampleCounts[i],
                         &numQualityLevels);
        if (FAILED(hr)) {
            break;
        }

        if (numQualityLevels > 0) {
            m_msaaSampleCount = sampleCounts[i];
            m_msaaQuality = static_cast<UINT>(D3D11_STANDARD_MULTISAMPLE_PATTERN);
            break;
        }
    }
    LOG(SEVERITY_LEVEL_INFO) << L"MSAA Sample Count = " << m_msaaSampleCount;
    LOG(SEVERITY_LEVEL_INFO) << L"MSAA Quality Level = " << m_msaaQuality;
    return S_OK;
}

HRESULT graphics::D3DInteropHelper::CreateTexture2D(UINT width, UINT height, ID3D11Texture2D **ppTexture2D)
{
    assert(width > 0);
    assert(height > 0);

    HRESULT hr;
    D3D11_TEXTURE2D_DESC texDescForSource;
    texDescForSource.ArraySize = 1;
    texDescForSource.BindFlags = D3D10_BIND_RENDER_TARGET | D3D10_BIND_SHADER_RESOURCE;
    texDescForSource.CPUAccessFlags = 0;
    texDescForSource.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
    texDescForSource.Width = width;
    texDescForSource.Height = height;
    texDescForSource.MipLevels = 1;
    texDescForSource.MiscFlags = 0;
    texDescForSource.SampleDesc.Count = 1;
    texDescForSource.SampleDesc.Quality = 0;
    texDescForSource.Usage = D3D11_USAGE_DEFAULT;
    hr = m_pD3D11Device->CreateTexture2D(&texDescForSource, nullptr, ppTexture2D);
#ifdef DEBUG_RESOURCE
    LOG_RESOURCE(SEVERITY_LEVEL_DEBUG, ppTexture2D);
#endif

    return hr;
}

HRESULT graphics::D3DInteropHelper::LoadD3D10ResourceShader(
    ID3D10Device *pDevice,
    PCWSTR pszResource,
    ID3D10Effect **ppShader )
{
    HRESULT hr;

    HRSRC hResource = ::FindResource(HINST_THISCOMPONENT, pszResource, RT_RCDATA);
    hr = hResource ? S_OK : E_FAIL;

    if (SUCCEEDED(hr)) {
        HGLOBAL hResourceData = ::LoadResource(HINST_THISCOMPONENT, hResource);
        hr = hResourceData ? S_OK : E_FAIL;

        if (SUCCEEDED(hr)) {
            LPVOID pData = ::LockResource(hResourceData);
            hr = pData ? S_OK : E_FAIL;

            if (SUCCEEDED(hr)) {
                hr = ::D3D10CreateEffectFromMemory(
                         pData,
                         ::SizeofResource(HINST_THISCOMPONENT, hResource),
                         0,
                         pDevice,
                         nullptr,
                         ppShader
                     );
            }
        }
    }

    return hr;
}

HRESULT graphics::D3DInteropHelper::CreateD3D10Texture2D(
    UINT width, UINT height, OUT ID3D10Texture2D **ppTexture2D )
{
    assert(width > 0);
    assert(height > 0);

    HRESULT hr;
    D3D10_TEXTURE2D_DESC texDescForSource;
    texDescForSource.ArraySize = 1;
    texDescForSource.BindFlags = D3D10_BIND_RENDER_TARGET | D3D10_BIND_SHADER_RESOURCE;
    texDescForSource.CPUAccessFlags = 0;
    texDescForSource.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
    texDescForSource.Width = width;
    texDescForSource.Height = height;
    texDescForSource.MipLevels = 1;
    texDescForSource.MiscFlags = 0;
    texDescForSource.SampleDesc.Count = 1;
    texDescForSource.SampleDesc.Quality = 0;
    texDescForSource.Usage = D3D10_USAGE_DEFAULT;
    hr = m_pD3D10Device->CreateTexture2D(&texDescForSource, nullptr, ppTexture2D);
#ifdef DEBUG_RESOURCE
    LOG_RESOURCE(SEVERITY_LEVEL_DEBUG, ppTexture2D);
#endif

#ifdef DEBUG
    if (FAILED(hr)) {
        LOG(SEVERITY_LEVEL_ERROR) << L"hr = " << std::hex << hr << std::dec;
        LOG(SEVERITY_LEVEL_ERROR) << L"width = " << width << L", height = " << height;
    }
#endif

    return hr;
}

HRESULT graphics::D3DInteropHelper::CreateD2DRenderTargetFromD3D10Texture2D(
    ID3D10Texture2D *pTexure2D,
    ID2D1RenderTarget **ppRenderTarget )
{
    HRESULT hr = S_OK;
    IDXGISurface    *pDxgiSurface1 = nullptr;
    hr = pTexure2D->QueryInterface(&pDxgiSurface1);

    D2D1_RENDER_TARGET_PROPERTIES props =
        D2D1::RenderTargetProperties(
            D2D1_RENDER_TARGET_TYPE_DEFAULT,
            D2D1::PixelFormat(DXGI_FORMAT_UNKNOWN, D2D1_ALPHA_MODE_PREMULTIPLIED),
            96,
            96);
    if (SUCCEEDED(hr)) {
        hr = m_pD2DFactory->CreateDxgiSurfaceRenderTarget(
                 pDxgiSurface1,
                 &props,
                 ppRenderTarget);
#ifdef DEBUG_RESOURCE
        LOG_RESOURCE(SEVERITY_LEVEL_DEBUG, ppRenderTarget);
#endif
    }
    SafeRelease(&pDxgiSurface1);
    return hr;
}

HRESULT graphics::D3DInteropHelper::CreateD2DSharedBitmapFromD3D10Texture2D(
    ID2D1RenderTarget *pRenderTarget,
    ID3D10Texture2D *pTexure2D,
    ID2D1Bitmap **ppBitmap )
{
    HRESULT hr = S_OK;
    IDXGISurface *pDxgiSurface = nullptr;
    hr = pTexure2D->QueryInterface(&pDxgiSurface);
    if (SUCCEEDED(hr)) {
        D2D1_BITMAP_PROPERTIES bitmapProp;
        bitmapProp.dpiX = 96;
        bitmapProp.dpiY = 96;
        bitmapProp.pixelFormat = D2D1::PixelFormat(DXGI_FORMAT_B8G8R8A8_UNORM, D2D1_ALPHA_MODE_PREMULTIPLIED);
        hr = pRenderTarget->CreateSharedBitmap(__uuidof(IDXGISurface), pDxgiSurface, &bitmapProp, ppBitmap);
#ifdef DEBUG_RESOURCE
        LOG_RESOURCE(SEVERITY_LEVEL_DEBUG, ppBitmap);
#endif
        SafeRelease(&pDxgiSurface);
    }
    return hr;
}

HRESULT graphics::D3DInteropHelper::_CreateCSDevice(
    IDXGIAdapter1 *pAdapter,
    UINT flags,
    OUT ID3D11Device **ppDevice,
    OUT ID3D11DeviceContext **ppImmediateContext)
{
    static const D3D_FEATURE_LEVEL flvl[] = {
        D3D_FEATURE_LEVEL_11_0,
        D3D_FEATURE_LEVEL_10_1,
        D3D_FEATURE_LEVEL_10_0
    };

    UNREFERENCED_PARAMETER(pAdapter);
    HRESULT hr = S_OK;

	D3D11_FEATURE_DATA_D3D10_X_HARDWARE_OPTIONS hwopts;
	m_pD3D11Device->CheckFeatureSupport(D3D11_FEATURE_D3D10_X_HARDWARE_OPTIONS, &hwopts, sizeof(hwopts));

    if ( m_pD3D11Device->GetFeatureLevel() >= D3D_FEATURE_LEVEL_11_0 && hwopts.ComputeShaders_Plus_RawAndStructuredBuffers_Via_Shader_4_x) {
        *ppDevice = m_pD3D11Device;
        *ppImmediateContext = m_pD3D11ImmediateContext;
        m_sharedCSDevice = true;
        m_computeShaderSupport = true;
        LOG(SEVERITY_LEVEL_INFO) << L"Hardware Compute Shader(DX11) is found.";
    } else {
        //D3D11_FEATURE_DATA_D3D10_X_HARDWARE_OPTIONS hwopts;
        //m_pD3D11Device->CheckFeatureSupport( D3D11_FEATURE_D3D10_X_HARDWARE_OPTIONS, &hwopts, sizeof(hwopts) );
        //if ( hwopts.ComputeShaders_Plus_RawAndStructuredBuffers_Via_Shader_4_x ) {
        //    *ppDevice = m_pD3D11Device;
        //    *ppImmediateContext = m_pD3D11ImmediateContext;
        //    m_sharedCSDevice = true;
        //    m_computeShaderSupport = true;
        //    LOG(SEVERITY_LEVEL_INFO) << L"Hardware Compute Shader(DX10) is found.";
        //} else {
            UINT uCreationFlags = flags | D3D11_CREATE_DEVICE_SINGLETHREADED;
#if defined(DEBUG) || defined(_DEBUG)
            uCreationFlags |= D3D11_CREATE_DEVICE_DEBUG;
#endif
            D3D_FEATURE_LEVEL flOut;
            hr = D3D11CreateDevice( nullptr,
                                    D3D_DRIVER_TYPE_REFERENCE,
                                    NULL,
                                    uCreationFlags,
                                    flvl,
                                    ARRAYSIZE(flvl),
                                    D3D11_SDK_VERSION,
                                    ppDevice,
                                    &flOut,
                                    ppImmediateContext );
            if (SUCCEEDED(hr) && flOut >= D3D_FEATURE_LEVEL_11_0) {
                m_sharedCSDevice = false;
                m_computeShaderSupport = true;
                LOG_HRESULT(SEVERITY_LEVEL_INFO, hr) << L"Software Compute Shader is found.";
            } else {
                LOG_HRESULT(SEVERITY_LEVEL_INFO, hr) << L"Compute Shader could not be found.";
                m_sharedCSDevice = false;
                m_computeShaderSupport = false;
                hr = E_FAIL;
            }
        //}
    }

    return hr;
}


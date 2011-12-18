#include "stdafx.h"
#include "graphics/SharedSurface.h"

graphics::SharedSurface::SharedSurface() :
    m_pD3D11SharedTexture(nullptr),
    m_pD3D11KeyedMutex(nullptr),
    m_d3d11SharedHandle(0),
    m_pD3D10KeyedMutex(nullptr),
    m_pD3D10SharedSurface(nullptr),
    m_pRenderTarget(nullptr),
    m_pBitmap(nullptr)
{
}

graphics::SharedSurface::~SharedSurface()
{
    SafeRelease(&m_pD3D11SharedTexture);
    SafeRelease(&m_pD3D11KeyedMutex);
    m_d3d11SharedHandle = 0;
    SafeRelease(&m_pD3D10KeyedMutex);
    SafeRelease(&m_pD3D10SharedSurface);
    SafeRelease(&m_pRenderTarget);
    SafeRelease(&m_pBitmap);
}

HRESULT graphics::SharedSurface::Initialize(
    UINT width,
    UINT height,
    ID3D11Device *pD3D11Device,
    ID3D10Device *pD3D10Device,
    ID2D1Factory *pD2DFactory)
{
    HRESULT hr = S_OK;
    if (SUCCEEDED(hr)) {
        D3D11_TEXTURE2D_DESC sharedTexDesc;
        ZeroMemory(&sharedTexDesc, sizeof(sharedTexDesc));
        sharedTexDesc.Width = width;
        sharedTexDesc.Height = height;
        sharedTexDesc.MipLevels = 1;
        sharedTexDesc.ArraySize = 1;
        sharedTexDesc.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
        sharedTexDesc.SampleDesc.Count = 1;
        sharedTexDesc.SampleDesc.Quality = 0;
        sharedTexDesc.Usage = D3D11_USAGE_DEFAULT;
        sharedTexDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_RENDER_TARGET;
        sharedTexDesc.MiscFlags = D3D11_RESOURCE_MISC_SHARED_KEYEDMUTEX;

        hr = pD3D11Device->CreateTexture2D(&sharedTexDesc, nullptr, &m_pD3D11SharedTexture);
        if (FAILED(hr)) {
            LOG(SEVERITY_LEVEL_ERROR) << L"CreateTexture2D was failed, hr = " << hr;
        }
    }

    if (SUCCEEDED(hr)) {
        m_pD3D11SharedTexture->QueryInterface(__uuidof(IDXGIKeyedMutex), (void**)&m_pD3D11KeyedMutex);
        if (FAILED(hr)) {
            LOG(SEVERITY_LEVEL_ERROR) << L"could not obtain KeyedMutex from shared texture, hr = " << hr;
        }
    }

    if (SUCCEEDED(hr)) {
        CComPtr<IDXGIResource> resource = nullptr;
        hr = m_pD3D11SharedTexture->QueryInterface(__uuidof(IDXGIResource), (void**)&resource);
        if (FAILED(hr)) {
            LOG(SEVERITY_LEVEL_ERROR) << L"could not obtain IDXGIResource from shared texture, hr = " << hr;
        }
        if (SUCCEEDED(hr)) {
            hr = resource->GetSharedHandle(&m_d3d11SharedHandle);
            if (FAILED(hr)) {
                LOG(SEVERITY_LEVEL_ERROR) << L"could not obtain shared handle from shared texture, hr = " << hr;
            }
        }
    }

    if (SUCCEEDED(hr)) {
        hr = pD3D10Device->OpenSharedResource(
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
        pD2DFactory->GetDesktopDpi(&dpiX, &dpiY);

        D2D1_RENDER_TARGET_PROPERTIES props =
            D2D1::RenderTargetProperties(
                D2D1_RENDER_TARGET_TYPE_DEFAULT,
                D2D1::PixelFormat(DXGI_FORMAT_UNKNOWN, D2D1_ALPHA_MODE_PREMULTIPLIED),
                dpiX,
                dpiY
            );
        hr = pD2DFactory->CreateDxgiSurfaceRenderTarget(m_pD3D10SharedSurface, &props, &m_pRenderTarget);
        if (FAILED(hr)) {
            LOG(SEVERITY_LEVEL_FATAL) << L"creating a DXGI surface render target for Direct2D was failed. hr = " << hr;
        }
    }

    if (SUCCEEDED(hr)) {
        D2D1_BITMAP_PROPERTIES bitmapProp;
        bitmapProp.dpiX = 96;
        bitmapProp.dpiY = 96;
        bitmapProp.pixelFormat = D2D1::PixelFormat(DXGI_FORMAT_B8G8R8A8_UNORM, D2D1_ALPHA_MODE_PREMULTIPLIED);
        hr = m_pRenderTarget->CreateSharedBitmap(__uuidof(IDXGISurface), m_pD3D10SharedSurface, &bitmapProp, &m_pBitmap);
    }

    return hr;
}

#include "StdAfx.h"
#include "Common.h"
#include "graphics/GraphicsCommon.h"
#include "graphics/D3DInteropHelper.h"
#include "graphics/filter/GaussFilter.h"
#include "graphics/filter/InnerGaussFilter.h"

graphics::filter::InnerGaussFilter::InnerGaussFilter(void) :
    m_gaussFilter(std::make_shared<graphics::filter::GaussFilter>())
{
}


graphics::filter::InnerGaussFilter::~InnerGaussFilter(void)
{
}

HRESULT graphics::filter::InnerGaussFilter::Initialize()
{
    return m_gaussFilter->Initialize();
}

HRESULT graphics::filter::InnerGaussFilter::ApplyFilter(
    IN graphics::D3DInteropHelper *pD3DInteropHelper,
    IN ID2D1RenderTarget *pRenderTarget,
    IN ID2D1Geometry *pGeometry,
    IN FLOAT width,
    IN FLOAT height,
    IN ID2D1Brush *pBrush,
    OUT ID2D1Bitmap **ppBitmap,
    OUT D2D1_RECT_F *pBitmapRect )
{
    static UINT offset = 5;
    HRESULT hr = S_OK;

    UINT textureWidth = static_cast<UINT>(width + offset * 2);
    UINT textureHeight = static_cast<UINT>(height + offset * 2);
    CComPtr<ID2D1RectangleGeometry> outerRectangle = nullptr;
    CHK_ERROR_HRESULT(hr,
                      pD3DInteropHelper->GetD2DFactory()->CreateRectangleGeometry(
                          D2D1::RectF(
                              0.0f,
                              0.0f,
                              static_cast<FLOAT>(textureWidth),
                              static_cast<FLOAT>(textureHeight)),
                          &outerRectangle));

    CComPtr<ID2D1PathGeometry> reversedGeometry = nullptr;
    CHK_ERROR_HRESULT(hr,
                      pD3DInteropHelper->GetD2DFactory()->CreatePathGeometry(&reversedGeometry));

    CComPtr<ID2D1GeometrySink> geometrySink = nullptr;
    CHK_ERROR_HRESULT(hr,
                      reversedGeometry->Open(&geometrySink));

    CHK_ERROR_HRESULT(hr,
                      outerRectangle->CombineWithGeometry(
                          pGeometry,
                          D2D1_COMBINE_MODE_EXCLUDE,
                          D2D1::Matrix3x2F::Translation(
                              static_cast<FLOAT>(offset),
                              static_cast<FLOAT>(offset)),
                          geometrySink));

    CHK_ERROR_HRESULT(hr,
                      geometrySink->Close());

    CComPtr<ID3D10Texture2D> sourceTexture = nullptr;
    CHK_ERROR_HRESULT(hr,
                      pD3DInteropHelper->CreateD3D10Texture2D(
                          textureWidth,
                          textureHeight,
                          &sourceTexture));

    CComPtr<ID2D1RenderTarget> renderTarget = nullptr;
    CHK_ERROR_HRESULT(hr,
                      pD3DInteropHelper->CreateD2DRenderTargetFromD3D10Texture2D(
                          sourceTexture,
                          &renderTarget));

    renderTarget->BeginDraw();
    renderTarget->FillGeometry(reversedGeometry, pBrush);
    CHK_ERROR_HRESULT(hr, renderTarget->EndDraw());

    CComPtr<ID3D10Texture2D> outputTexture = nullptr;
    CHK_ERROR_HRESULT(hr, m_gaussFilter->ApplyFilter(
                          pD3DInteropHelper,
                          sourceTexture,
                          &outputTexture));
    CComPtr<ID2D1Bitmap> outputTextureSB = nullptr;
    CHK_ERROR_HRESULT(hr,
                      pD3DInteropHelper->CreateD2DSharedBitmapFromD3D10Texture2D(
                          pRenderTarget,
                          outputTexture,
                          &outputTextureSB));

    CComPtr<ID2D1Bitmap> resizedBitmap = nullptr;
    CHK_ERROR_HRESULT(hr,
                      pRenderTarget->CreateBitmap(
                          D2D1::SizeU(
                              static_cast<UINT32>(width),
                              static_cast<UINT32>(height)),
                          D2D1::BitmapProperties(
                              D2D1::PixelFormat(
                                  DXGI_FORMAT_B8G8R8A8_UNORM,
                                  D2D1_ALPHA_MODE_PREMULTIPLIED),
                              96.0f,
                              96.0f),
                          &resizedBitmap));

    D2D1_POINT_2U destPoint = D2D1::Point2U(0, 0);
    D2D1_RECT_U srcRect = D2D1::RectU(
                              offset,
                              offset,
                              static_cast<UINT32>(width + offset),
                              static_cast<UINT32>(height + offset));
    CHK_ERROR_HRESULT(hr,
                      resizedBitmap->CopyFromBitmap(&destPoint, outputTextureSB, &srcRect));

    CComPtr<ID2D1BitmapBrush> resizedBitmapBrush = nullptr;
    CHK_ERROR_HRESULT(hr,
                      pRenderTarget->CreateBitmapBrush(
                          resizedBitmap,
                          D2D1::BitmapBrushProperties(D2D1_EXTEND_MODE_CLAMP),
                          D2D1::BrushProperties(), &resizedBitmapBrush));

    CComPtr<ID3D10Texture2D> bitmapTexture = nullptr;
    CHK_ERROR_HRESULT(hr,
                      pD3DInteropHelper->CreateD3D10Texture2D(
                          static_cast<UINT>(width), static_cast<UINT>(height), &bitmapTexture));

    CComPtr<ID2D1RenderTarget> bitmapTextureRT = nullptr;
    CHK_ERROR_HRESULT(hr,
                      pD3DInteropHelper->CreateD2DRenderTargetFromD3D10Texture2D(bitmapTexture, &bitmapTextureRT));

    bitmapTextureRT->BeginDraw();
    bitmapTextureRT->FillGeometry(pGeometry, resizedBitmapBrush);
    CHK_ERROR_HRESULT(hr, bitmapTextureRT->EndDraw());

    CHK_ERROR_HRESULT(hr,
                      pD3DInteropHelper->CreateD2DSharedBitmapFromD3D10Texture2D(pRenderTarget, bitmapTexture, ppBitmap));

    pBitmapRect->left = 0.0f;
    pBitmapRect->top = 0.0f;
    pBitmapRect->right = width;
    pBitmapRect->bottom = height;
    return hr;
}

void graphics::filter::InnerGaussFilter::CalculateMargins(
    IN FLOAT width,
    IN FLOAT height,
    OUT FLOAT &leftMargin,
    OUT FLOAT &topMargin,
    OUT FLOAT &rightMargin,
    OUT FLOAT &bottomMargin )
{
    UNREFERENCED_PARAMETER(width);
    UNREFERENCED_PARAMETER(height);
    leftMargin = 0.0f;
    topMargin = 0.0f;
    rightMargin = 0.0f;
    bottomMargin = 0.0f;
}


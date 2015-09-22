#include "StdAfx.h"
#include "Common.h"
#include "graphics/D3DInteropHelper.h"
#include "graphics/filter/GaussFilter.h"
#include "graphics/filter/FilterCommon.h"

graphics::filter::GaussFilter::GaussFilter(void)
    : m_gaussWeight()
    , m_tapOffsetX()
    , m_tapOffsetY()
    , m_pGaussEffect(nullptr)
    , m_pTechniqueNoRef(nullptr)
    , m_pDiffuseVariableNoRef(nullptr)
    , m_pVertexBuffer(nullptr)
    , m_pVertexLayout(nullptr)
    , m_pFacesIndexBuffer(nullptr) {
}

graphics::filter::GaussFilter::~GaussFilter(void) {
  SafeRelease(&m_pFacesIndexBuffer);
  SafeRelease(&m_pVertexLayout);
  SafeRelease(&m_pVertexBuffer);
  m_pDiffuseVariableNoRef = nullptr;
  m_pTechniqueNoRef = nullptr;
  SafeRelease(&m_pGaussEffect);
}

void graphics::filter::GaussFilter::_UpdateWeight(float dispersion) {
  float total = 0;
  for (int i = 0; i < NUMBER_OF_WEIGHTS; i++) {
    m_gaussWeight[i] = expf(-0.5f * (float)(i * i) / dispersion);
    if (i == 0) {
      total += m_gaussWeight[i];
    } else {
      total += /* 2.0f */ 3.0f * m_gaussWeight[i];
    }
  }
  for (int i = 0; i < NUMBER_OF_WEIGHTS; i++) {
    m_gaussWeight[i] /= total;
  }
}

void graphics::filter::GaussFilter::_UpdateTapOffset(int width, int height) {
  for (int i = 0; i < NUMBER_OF_WEIGHTS; i++) {
    m_tapOffsetX[i] = 1.0f / width * i * 1.0f; // 2.0f;
    m_tapOffsetY[i] = 1.0f / height * i * 1.0f; // 2.0f;
  }
}

HRESULT graphics::filter::GaussFilter::_SetGaussParameters(int width, int height) {
  HRESULT hr = S_OK;
  _UpdateWeight(5.0f * 5.0f);
  _UpdateTapOffset(width, height);

  ID3D10EffectScalarVariable *pVarWidth = m_pGaussEffect->GetVariableByName("WIDTH")->AsScalar();
  hr = pVarWidth != nullptr ? S_OK : E_FAIL;
  if (SUCCEEDED(hr)) {
    hr = pVarWidth->SetFloat((float)width);
  }

  ID3D10EffectScalarVariable *pVarHeight = m_pGaussEffect->GetVariableByName("HEIGHT")->AsScalar();
  hr = pVarHeight != nullptr ? S_OK : E_FAIL;
  if (SUCCEEDED(hr)) {
    hr = pVarHeight->SetFloat((float)height);
  }

  ID3D10EffectScalarVariable *pVarWeight = m_pGaussEffect->GetVariableByName("WEIGHT")->AsScalar();
  hr = pVarWeight != nullptr ? S_OK : E_FAIL;
  if (SUCCEEDED(hr)) {
    hr = pVarWeight->SetFloatArray(m_gaussWeight.data(), 0, NUMBER_OF_WEIGHTS);
  }

  ID3D10EffectScalarVariable *pVarTapOffsetX = m_pGaussEffect->GetVariableByName("TAPOFFSET_X")->AsScalar();
  pVarTapOffsetX->SetFloatArray(m_tapOffsetX.data(), 0, NUMBER_OF_WEIGHTS);
  ID3D10EffectScalarVariable *pVarTapOffsetY = m_pGaussEffect->GetVariableByName("TAPOFFSET_Y")->AsScalar();
  pVarTapOffsetY->SetFloatArray(m_tapOffsetY.data(), 0, NUMBER_OF_WEIGHTS);

  return S_OK;
}

HRESULT graphics::filter::GaussFilter::Initialize() {
  HRSRC hResource = ::FindResource(HINST_THISCOMPONENT, MAKEINTRESOURCE(IDR_GAUSS_SHADER), RT_RCDATA);
  return hResource ? S_OK : E_FAIL;
}

HRESULT graphics::filter::GaussFilter::ApplyFilter(IN graphics::D3DInteropHelper *pD3DInteropHelper,
                                                   IN ID3D10Texture2D *pSourceTexture,
                                                   OUT ID3D10Texture2D **ppOutputTexture) {
  HRESULT hr = S_OK;

  ID3D10Device *pDevice = pD3DInteropHelper->GetD3D10Device();

  //     pD3DInteropHelper->PushStates();

  D3D10_TEXTURE2D_DESC sourceTextureDesc;
  pSourceTexture->GetDesc(&sourceTextureDesc);

  ID3D10ShaderResourceView *pSourceShaderRV = nullptr;
  ID3D10Texture2D *pTemporaryTexture = nullptr;
  ID3D10RenderTargetView *pTemporaryTextureRV = nullptr;
  ID3D10ShaderResourceView *pTemporaryShaderRV = nullptr;
  ID3D10RenderTargetView *pOutputTextureRV = nullptr;

  if (SUCCEEDED(hr) && m_pGaussEffect == nullptr) {
    // Load pixel shader
    hr = pD3DInteropHelper->LoadD3D10ResourceShader(pDevice, MAKEINTRESOURCE(IDR_GAUSS_SHADER), &m_pGaussEffect);
    if (FAILED(hr)) {
      LOG(SEVERITY_LEVEL_FATAL) << L"Load 'gauss.fx' effect failed, hr = " << hr;
    }
    if (SUCCEEDED(hr)) {
      // Obtain the technique
      m_pTechniqueNoRef = m_pGaussEffect->GetTechniqueByName("Shader");
      hr = m_pTechniqueNoRef ? S_OK : E_FAIL;
    }
    if (SUCCEEDED(hr)) {
      m_pDiffuseVariableNoRef = m_pGaussEffect->GetVariableByName("texDiffuse")->AsShaderResource();
      hr = m_pDiffuseVariableNoRef ? S_OK : E_FAIL;
    }
  }

  if (SUCCEEDED(hr)) {
    hr = _SetGaussParameters(sourceTextureDesc.Width, sourceTextureDesc.Height);
  }
  if (SUCCEEDED(hr)) {
    hr = pD3DInteropHelper->CreateD3D10Texture2D(sourceTextureDesc.Width, sourceTextureDesc.Height, &pTemporaryTexture);
  }
  if (SUCCEEDED(hr)) {
    hr = pD3DInteropHelper->CreateD3D10Texture2D(sourceTextureDesc.Width, sourceTextureDesc.Height, ppOutputTexture);
  }

  // <<< PASS 0 >>>
  D3D10_SHADER_RESOURCE_VIEW_DESC srvDesc;
  srvDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
  srvDesc.ViewDimension = D3D10_SRV_DIMENSION_TEXTURE2D;
  srvDesc.Texture2D.MipLevels = 1;
  srvDesc.Texture2D.MostDetailedMip = 0;
  if (SUCCEEDED(hr)) {
    CHK_ERROR_HRESULT(hr, pDevice->CreateShaderResourceView(pSourceTexture, nullptr, &pSourceShaderRV));
#ifdef DEBUG_GPU_RESOURCE
    LOG_RESOURCE(SEVERITY_LEVEL_DEBUG, pSourceShaderRV);
#endif
  }

  D3D10_RENDER_TARGET_VIEW_DESC rtDesc;
  rtDesc.Format = sourceTextureDesc.Format;
  rtDesc.ViewDimension = D3D10_RTV_DIMENSION_TEXTURE2D;
  rtDesc.Texture2D.MipSlice = 0;

  if (SUCCEEDED(hr)) {
    CHK_ERROR_HRESULT(hr, pDevice->CreateRenderTargetView(pTemporaryTexture, &rtDesc, &pTemporaryTextureRV));
#ifdef DEBUG_GPU_RESOURCE
    LOG_RESOURCE(SEVERITY_LEVEL_DEBUG, pTemporaryTextureRV);
#endif
  }

  m_pDiffuseVariableNoRef->SetResource(pSourceShaderRV);

  ID3D10RenderTargetView *viewList[1] = {pTemporaryTextureRV};
  pDevice->OMSetRenderTargets(1, viewList, nullptr);

  D3D10_VIEWPORT viewport;
  viewport.Width = sourceTextureDesc.Width;
  viewport.Height = sourceTextureDesc.Height;
  viewport.TopLeftX = 0;
  viewport.TopLeftY = 0;
  viewport.MinDepth = 0;
  viewport.MaxDepth = 1;
  pDevice->RSSetViewports(1, &viewport);

  if (SUCCEEDED(hr)) {
    CHK_ERROR_HRESULT(hr, m_pTechniqueNoRef->GetPassByName("P0")->Apply(0));
  }
  SetVertexAndIndexBufferForFilter(pD3DInteropHelper);
  pDevice->DrawIndexed(6, 0, 0);
  pDevice->Flush();

  m_pDiffuseVariableNoRef->SetResource(nullptr);
  ID3D10ShaderResourceView *resViewList[1] = {nullptr};
  pDevice->PSSetShaderResources(0, 1, resViewList);
  viewList[0] = nullptr;
  pDevice->OMSetRenderTargets(1, viewList, nullptr);
  // <<< PASS 0 END >>>

  // <<< PASS 1 START >>>
  if (SUCCEEDED(hr)) {
    CHK_ERROR_HRESULT(hr, pDevice->CreateShaderResourceView(pTemporaryTexture, nullptr, &pTemporaryShaderRV));
#ifdef DEBUG_GPU_RESOURCE
    LOG_RESOURCE(SEVERITY_LEVEL_DEBUG, pTemporaryShaderRV);
#endif
  }

  if (SUCCEEDED(hr)) {
    CHK_ERROR_HRESULT(hr, pDevice->CreateRenderTargetView(*ppOutputTexture, &rtDesc, &pOutputTextureRV));
#ifdef DEBUG_GPU_RESOURCE
    LOG_RESOURCE(SEVERITY_LEVEL_DEBUG, pOutputTextureRV);
#endif
  }

  m_pDiffuseVariableNoRef->SetResource(pTemporaryShaderRV);

  viewList[0] = pOutputTextureRV;
  pDevice->OMSetRenderTargets(1, viewList, nullptr);

  if (SUCCEEDED(hr)) {
    CHK_ERROR_HRESULT(hr, m_pTechniqueNoRef->GetPassByName("P1")->Apply(0));
  }
  pDevice->DrawIndexed(6, 0, 0);
  pDevice->Flush();

  m_pDiffuseVariableNoRef->SetResource(nullptr);
  pDevice->PSSetShaderResources(0, 1, resViewList);
  viewList[0] = nullptr;
  pDevice->OMSetRenderTargets(1, viewList, nullptr);
  // <<< PASS 1 END >>>

  SafeRelease(&pOutputTextureRV);
  SafeRelease(&pTemporaryShaderRV);
  SafeRelease(&pTemporaryTextureRV);
  SafeRelease(&pTemporaryTexture);
  SafeRelease(&pSourceShaderRV);
  return hr;
}

HRESULT graphics::filter::GaussFilter::ApplyFilter(IN graphics::D3DInteropHelper *pD3DInteropHelper,
                                                   IN ID2D1RenderTarget *pRenderTarget, IN ID2D1Geometry *pGeometry,
                                                   IN FLOAT width, IN FLOAT height, IN ID2D1Brush *pBrush,
                                                   OUT ID2D1Bitmap **ppBitmap, OUT D2D1_RECT_F *pBitmapRect) {
  HRESULT hr = S_OK;
  CComPtr<ID3D10Texture2D> sourceTexture = nullptr;
  FLOAT leftMargin, topMargin, rightMargin, bottomMargin;
  CalculateMargins(width, height, leftMargin, topMargin, rightMargin, bottomMargin);
  UINT textureWidth = static_cast<UINT>(width + leftMargin + rightMargin);
  UINT textureHeight = static_cast<UINT>(height + topMargin + bottomMargin);
  CHK_FATAL_HRESULT(pD3DInteropHelper->CreateD3D10Texture2D(textureWidth, textureHeight, &sourceTexture));

  CComPtr<ID2D1RenderTarget> textureRenderTarget = nullptr;
  CHK_FATAL_HRESULT(pD3DInteropHelper->CreateD2DRenderTargetFromD3D10Texture2D(sourceTexture, &textureRenderTarget));

  textureRenderTarget->BeginDraw();
  textureRenderTarget->Clear();
  textureRenderTarget->SetTransform(D2D1::Matrix3x2F::Translation(leftMargin, topMargin));
  textureRenderTarget->FillGeometry(pGeometry, pBrush);
  CHK_FATAL_HRESULT(textureRenderTarget->EndDraw());

  CComPtr<ID3D10Texture2D> destinationTexture = nullptr;
  CHK_FATAL_HRESULT(ApplyFilter(pD3DInteropHelper, sourceTexture, &destinationTexture));

  CHK_FATAL_HRESULT(
      pD3DInteropHelper->CreateD2DSharedBitmapFromD3D10Texture2D(pRenderTarget, destinationTexture, ppBitmap));
  pBitmapRect->left = -leftMargin;
  pBitmapRect->top = -topMargin;
  pBitmapRect->right = width + rightMargin;
  pBitmapRect->bottom = height + bottomMargin;

  return hr;
}

HRESULT
graphics::filter::GaussFilter::CreateVertexAndIndexBufferForFilter(graphics::D3DInteropHelper *pD3DInteropHelper) {
  HRESULT hr;
  D3D10_BUFFER_DESC bd;
  bd.Usage = D3D10_USAGE_DEFAULT;
  bd.ByteWidth = graphics::filter::FilterCommon::VERTEX_ARRAY_BYTEWIDTH;
  bd.BindFlags = D3D10_BIND_VERTEX_BUFFER;
  bd.CPUAccessFlags = 0;
  bd.MiscFlags = 0;
  D3D10_SUBRESOURCE_DATA InitData;
  InitData.pSysMem = graphics::filter::FilterCommon::VERTEX_ARRAY;

  hr = pD3DInteropHelper->GetD3D10Device()->CreateBuffer(&bd, &InitData, &m_pVertexBuffer);
#ifdef DEBUG_RESOURCE
  LOG_RESOURCE(SEVERITY_LEVEL_DEBUG, m_pVertexBuffer);
#endif

  if (SUCCEEDED(hr)) {
    UINT numElements = graphics::filter::FilterCommon::INPUT_LAYOUT_ARRAYSIZE;

    D3D10_PASS_DESC PassDesc;
    m_pTechniqueNoRef->GetPassByIndex(0)->GetDesc(&PassDesc);
    if (FAILED(hr)) {
      LOG(SEVERITY_LEVEL_ERROR) << L"GetPassByIndex(0)->GetDesc() failed, " << hr;
    }

    hr = pD3DInteropHelper->GetD3D10Device()->CreateInputLayout(graphics::filter::FilterCommon::INPUT_LAYOUT,
                                                                numElements, PassDesc.pIAInputSignature,
                                                                PassDesc.IAInputSignatureSize, &m_pVertexLayout);
#ifdef DEBUG_RESOURCE
    LOG_RESOURCE(SEVERITY_LEVEL_DEBUG, m_pVertexLayout);
#endif
    if (FAILED(hr)) {
      LOG(SEVERITY_LEVEL_ERROR) << L"CreateInputLayout failed, " << hr;
    }
  }

  if (SUCCEEDED(hr)) {
    D3D10_BUFFER_DESC bd;
    bd.Usage = D3D10_USAGE_DEFAULT;
    bd.ByteWidth = graphics::filter::FilterCommon::INDEX_ARRAY_BYTEWIDTH;
    bd.BindFlags = D3D10_BIND_INDEX_BUFFER;
    bd.CPUAccessFlags = 0;
    bd.MiscFlags = 0;
    D3D10_SUBRESOURCE_DATA InitData;
    InitData.pSysMem = graphics::filter::FilterCommon::INDEX_ARRAY;

    hr = pD3DInteropHelper->GetD3D10Device()->CreateBuffer(&bd, &InitData, &m_pFacesIndexBuffer);
#ifdef DEBUG_RESOURCE
    LOG_RESOURCE(SEVERITY_LEVEL_DEBUG, m_pFacesIndexBuffer);
#endif
  }
  return hr;
}

void graphics::filter::GaussFilter::SetVertexAndIndexBufferForFilter(graphics::D3DInteropHelper *pD3DInteropHelper) {
  if (m_pVertexBuffer == nullptr || m_pFacesIndexBuffer == nullptr) {
    CreateVertexAndIndexBufferForFilter(pD3DInteropHelper);
  }

  // Set vertex buffer
  UINT stride = sizeof(graphics::D2DVertex);
  UINT offset = 0;
  ID3D10Buffer *pVertexBuffer = m_pVertexBuffer;

  pD3DInteropHelper->GetD3D10Device()->IASetVertexBuffers(0, // StartSlot
                                                          1, // NumBuffers
                                                          &pVertexBuffer, &stride, &offset);

  pD3DInteropHelper->GetD3D10Device()->IASetInputLayout(m_pVertexLayout);

  pD3DInteropHelper->GetD3D10Device()->IASetIndexBuffer(m_pFacesIndexBuffer, DXGI_FORMAT_R16_UINT, 0);
}

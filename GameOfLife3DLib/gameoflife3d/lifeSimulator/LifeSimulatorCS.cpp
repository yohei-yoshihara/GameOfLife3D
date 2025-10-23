#include "stdafx.h"
#include "Common.h"
#include "util/Utils.h"
#include "graphics/D3DInteropHelper.h"
#include "gameOfLife3D/lifeSimulator/LifeSimulatorCS.h"

using namespace DirectX;

#define DEBUG_LIFESIMULATOR

const XMFLOAT3 gameOfLife3D::lifeSimulator::LifeSimulatorCS::s_bornColors[NUMBER_OF_BORNCOLORS] = {
    XMFLOAT3(1.0f, 0.0f, 0.0f), XMFLOAT3(0.0f, 1.0f, 0.0f), XMFLOAT3(0.0f, 0.0f, 1.0f), XMFLOAT3(1.0f, 1.0f, 0.0f),
    XMFLOAT3(0.0f, 1.0f, 1.0f), XMFLOAT3(1.0f, 0.0f, 1.0f), XMFLOAT3(1.0f, 1.0f, 1.0f),
};

gameOfLife3D::lifeSimulator::LifeSimulatorCS::LifeSimulatorCS()
    : m_currentIndex(0)
    , m_nextIndex(1)
    , m_pDevice(nullptr)
    , m_pImmediateContext(nullptr)
    , m_pComputeShader(nullptr)
    , m_pCBParamsBuffer(nullptr) {
  for (size_t i = 0; i < NUMBER_OF_BUFFERS; ++i) {
    m_pDataBuffer[i] = nullptr;
    m_pDataBufferSRV[i] = nullptr;
    m_pDataBufferUAV[i] = nullptr;
  }
}

gameOfLife3D::lifeSimulator::LifeSimulatorCS::~LifeSimulatorCS() {
  for (size_t i = 0; i < NUMBER_OF_BUFFERS; ++i) {
    SafeRelease(&m_pDataBufferUAV[i]);
    SafeRelease(&m_pDataBufferSRV[i]);
    SafeRelease(&m_pDataBuffer[i]);
  }
  SafeRelease(&m_pCBParamsBuffer);
  SafeRelease(&m_pComputeShader);
}

HRESULT gameOfLife3D::lifeSimulator::LifeSimulatorCS::Initialize(graphics::D3DInteropHelper *pD3DInteropHelper) {
  m_pDevice = pD3DInteropHelper->GetD3D11CSDevice();
  m_pImmediateContext = pD3DInteropHelper->GetD3D11CSImmediateContext();

  CComPtr<ID3DBlob> pCSBlob = nullptr;
  LPCSTR pProfile = (m_pDevice->GetFeatureLevel() >= D3D_FEATURE_LEVEL_11_0) ? "cs_5_0" : "cs_4_0";
  CHK_FATAL_HRESULT(
      pD3DInteropHelper->CompileShaderFromResource(MAKEINTRESOURCE(IDR_SHADER_GAMEOFLIFE), "main", pProfile, &pCSBlob));
  CHK_FATAL_HRESULT(m_pDevice->CreateComputeShader(pCSBlob->GetBufferPointer(), pCSBlob->GetBufferSize(), nullptr,
                                                   &m_pComputeShader));

  for (UINT bufIndex = 0; bufIndex < NUMBER_OF_BUFFERS; ++bufIndex) {
    std::vector<gameOfLife3D::lifeSimulator::LifeData> lifeData(GetWidth() * GetHeight());
    for (UINT y = 0; y < GetHeight(); ++y) {
      for (UINT x = 0; x < GetWidth(); ++x) {
        UINT index = x + y * GetWidth();
        if (bufIndex == m_currentIndex) {
          lifeData.at(index) = GetInitialData()[index];
        } else {
          gameOfLife3D::lifeSimulator::LifeData data;
          ZeroMemory(&data, sizeof(data));
          lifeData.at(index) = data;
        }
      }
    }
#ifdef DEBUG_LIFESIMULATOR
    SPDLOG_DEBUG(L"Buffer Index = {}", bufIndex);
    Dump(lifeData.data());
#endif
    D3D11_BUFFER_DESC bufDesc;
    ZeroMemory(&bufDesc, sizeof(bufDesc));
    bufDesc.BindFlags = D3D11_BIND_UNORDERED_ACCESS | D3D11_BIND_SHADER_RESOURCE;
    bufDesc.ByteWidth = sizeof(gameOfLife3D::lifeSimulator::LifeData) * GetWidth() * GetHeight();
    bufDesc.MiscFlags = D3D11_RESOURCE_MISC_BUFFER_STRUCTURED;
    bufDesc.StructureByteStride = sizeof(gameOfLife3D::lifeSimulator::LifeData);

    D3D11_SUBRESOURCE_DATA initData;
    initData.pSysMem = lifeData.data();
    CHK_FATAL_HRESULT(m_pDevice->CreateBuffer(&bufDesc, &initData, &m_pDataBuffer[bufIndex]));

    D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc;
    ZeroMemory(&srvDesc, sizeof(srvDesc));
    srvDesc.ViewDimension = D3D11_SRV_DIMENSION_BUFFEREX;
    srvDesc.BufferEx.FirstElement = 0;
    srvDesc.Format = DXGI_FORMAT_UNKNOWN;
    srvDesc.BufferEx.NumElements = GetWidth() * GetHeight();
    CHK_FATAL_HRESULT(
        m_pDevice->CreateShaderResourceView(m_pDataBuffer[bufIndex], &srvDesc, &m_pDataBufferSRV[bufIndex]));

    D3D11_UNORDERED_ACCESS_VIEW_DESC uavDesc;
    ZeroMemory(&uavDesc, sizeof(uavDesc));
    uavDesc.ViewDimension = D3D11_UAV_DIMENSION_BUFFER;
    uavDesc.Buffer.FirstElement = 0;
    uavDesc.Format = DXGI_FORMAT_UNKNOWN;
    uavDesc.Buffer.NumElements = GetWidth() * GetHeight();
    CHK_FATAL_HRESULT(
        m_pDevice->CreateUnorderedAccessView(m_pDataBuffer[bufIndex], &uavDesc, &m_pDataBufferUAV[bufIndex]));
  }

  // CBParams
  D3D11_BUFFER_DESC bufDesc;
  ZeroMemory(&bufDesc, sizeof(bufDesc));
  bufDesc.Usage = D3D11_USAGE_DEFAULT;
  bufDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
  bufDesc.CPUAccessFlags = 0;
  bufDesc.ByteWidth = static_cast<UINT>(util::ToBlockSize(sizeof(CBParams), 16));
  CHK_FATAL_HRESULT(m_pDevice->CreateBuffer(&bufDesc, nullptr, &m_pCBParamsBuffer));

  CBParams cbParams;
  cbParams.width = GetWidth();
  cbParams.height = GetHeight();
  cbParams.r1 = GetR1();
  cbParams.r2 = GetR2();
  cbParams.r3 = GetR3();
  cbParams.r4 = GetR4();
  m_pImmediateContext->UpdateSubresource(m_pCBParamsBuffer, 0, nullptr, &cbParams, 0, 0);

  return S_OK;
}

HRESULT gameOfLife3D::lifeSimulator::LifeSimulatorCS::Compute() {
  m_pImmediateContext->CSSetShader(m_pComputeShader, nullptr, 0);
  m_pImmediateContext->CSSetShaderResources(0, 1, &m_pDataBufferSRV[m_currentIndex]);
  m_pImmediateContext->CSSetUnorderedAccessViews(0, 1, &m_pDataBufferUAV[m_nextIndex], nullptr);
  m_pImmediateContext->CSSetConstantBuffers(0, 1, &m_pCBParamsBuffer);
  m_pImmediateContext->Dispatch(GetWidth() * GetHeight(), 1, 1);

#ifdef DEBUG_LIFESIMULATOR
  ID3D11Buffer *debugCurrentBuf = CreateAndCopyToDebugBuf(m_pDataBuffer[m_currentIndex]);
  D3D11_MAPPED_SUBRESOURCE mappedResource;
  m_pImmediateContext->Map(debugCurrentBuf, 0, D3D11_MAP_READ, 0, &mappedResource);
  gameOfLife3D::lifeSimulator::LifeData *pData = (gameOfLife3D::lifeSimulator::LifeData *)mappedResource.pData;
  SPDLOG_DEBUG(L"Current Buffer ({})", m_currentIndex);
  Dump(pData);
  m_pImmediateContext->Unmap(debugCurrentBuf, 0);
  SafeRelease(&debugCurrentBuf);

  ID3D11Buffer *debugNextBuf = CreateAndCopyToDebugBuf(m_pDataBuffer[m_nextIndex]);
  m_pImmediateContext->Map(debugNextBuf, 0, D3D11_MAP_READ, 0, &mappedResource);
  pData = (gameOfLife3D::lifeSimulator::LifeData *)mappedResource.pData;
  SPDLOG_DEBUG(L"Next Buffer ({})", m_nextIndex);
  Dump(pData);
  m_pImmediateContext->Unmap(debugNextBuf, 0);
  SafeRelease(&debugNextBuf);
#endif

  m_currentIndex = (m_currentIndex + 1) % 2;
  m_nextIndex = (m_currentIndex + 1) % 2;
#ifdef DEBUG_LIFESIMULATOR
  SPDLOG_DEBUG(L"Change index: current = {}, next = {}", m_currentIndex, m_nextIndex);
#endif

  ID3D11ShaderResourceView *pSRV = nullptr;
  m_pImmediateContext->CSSetShaderResources(0, 1, &pSRV);
  ID3D11UnorderedAccessView *pUAV = nullptr;
  m_pImmediateContext->CSSetUnorderedAccessViews(0, 1, &pUAV, nullptr);

  return S_OK;
}

ID3D11Buffer *gameOfLife3D::lifeSimulator::LifeSimulatorCS::CreateAndCopyToDebugBuf(ID3D11Buffer *pBuffer) {
  ID3D11Buffer *debugbuf = nullptr;

  D3D11_BUFFER_DESC desc;
  ZeroMemory(&desc, sizeof(desc));
  pBuffer->GetDesc(&desc);
  desc.CPUAccessFlags = D3D11_CPU_ACCESS_READ;
  desc.Usage = D3D11_USAGE_STAGING;
  desc.BindFlags = 0;
  desc.MiscFlags = 0;
  if (SUCCEEDED(m_pDevice->CreateBuffer(&desc, nullptr, &debugbuf))) {
    m_pImmediateContext->CopyResource(debugbuf, pBuffer);
  }

  return debugbuf;
}

void gameOfLife3D::lifeSimulator::LifeSimulatorCS::Dump(gameOfLife3D::lifeSimulator::LifeData *data) {
  for (UINT y = 0; y < GetHeight(); ++y) {
    std::wstringstream sstr;
    for (UINT x = 0; x < GetWidth(); ++x) {
      UINT index = x + y * GetWidth();
      sstr << data[index].alive << L" ";
    }
    SPDLOG_DEBUG(sstr.str());
  }
}

void gameOfLife3D::lifeSimulator::LifeSimulatorCS::CopyCurrent(OUT gameOfLife3D::lifeSimulator::LifeData *pLifeData) {
  Copy(m_currentIndex, pLifeData);
}

void gameOfLife3D::lifeSimulator::LifeSimulatorCS::CopyNext(OUT gameOfLife3D::lifeSimulator::LifeData *pLifeData) {
  Copy(m_nextIndex, pLifeData);
}

void gameOfLife3D::lifeSimulator::LifeSimulatorCS::Copy(UINT index,
                                                        OUT gameOfLife3D::lifeSimulator::LifeData *pLifeData) {
  ID3D11Buffer *buf = CreateAndCopyToDebugBuf(m_pDataBuffer[index]);
  D3D11_MAPPED_SUBRESOURCE mappedResource;
  m_pImmediateContext->Map(buf, 0, D3D11_MAP_READ, 0, &mappedResource);
  gameOfLife3D::lifeSimulator::LifeData *pData = (gameOfLife3D::lifeSimulator::LifeData *)mappedResource.pData;
  for (UINT i = 0; i < GetMaxNumberOfLifeData(); ++i) {
    pLifeData[i] = pData[i];
  }
  m_pImmediateContext->Unmap(buf, 0);
  SafeRelease(&buf);
}

HRESULT gameOfLife3D::lifeSimulator::LifeSimulatorCS::LoadRuleFile(graphics::D3DInteropHelper *pD3DInteropHelper,
                                                                   const std::wstring &fileName,
                                                                   OUT std::wstring &errorMessage) {
  HRESULT hr = S_OK;

  ID3D11ComputeShader *pComputeShader = nullptr;
  CComPtr<ID3DBlob> pCSBlob = nullptr;
  LPCSTR pProfile = (m_pDevice->GetFeatureLevel() >= D3D_FEATURE_LEVEL_11_0) ? "cs_5_0" : "cs_4_0";
  CHK_ERROR_HRESULT(hr, pD3DInteropHelper->CompileShaderFromFile((WCHAR *)fileName.c_str(), "main", pProfile, &pCSBlob,
                                                                 &errorMessage));
  if (FAILED(hr)) {
    return hr;
  }

  CHK_ERROR_HRESULT(hr, m_pDevice->CreateComputeShader(pCSBlob->GetBufferPointer(), pCSBlob->GetBufferSize(), nullptr,
                                                       &pComputeShader));
  if (FAILED(hr)) {
    return hr;
  }

  SafeRelease(&m_pComputeShader);
  m_pComputeShader = pComputeShader;

  return S_OK;
}

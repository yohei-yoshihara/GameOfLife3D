#include "stdafx.h"
#include "Common.h"
#include "gameOfLife3D/lifeSimulator/AbstractLifeSimulator.h"
#include "graphics/D3DInteropHelper.h"
#include "gameOfLife3D/draw3d/VertexGenerator.h"
#include "gameOfLife3D/draw3d/Draw3DUtil.h"
#include "gameOfLife3D/draw3d/Draw3DWithCPU.h"

using namespace DirectX;

gameOfLife3D::draw3d::Draw3DWithCPU::Draw3DWithCPU()
    : m_pLifeSimulator()
    , m_maxNumberOfGenerations(24)
    , m_pVertexGenerator(std::make_shared<gameOfLife3D::draw3d::VertexGenerator>())
    , m_pVertexShader(nullptr)
    , m_pInputLayout(nullptr)
    , m_pPixelShader(nullptr) {
}

gameOfLife3D::draw3d::Draw3DWithCPU::~Draw3DWithCPU() {
  SafeRelease(&m_pVertexShader);
  SafeRelease(&m_pInputLayout);
  SafeRelease(&m_pPixelShader);
}

HRESULT gameOfLife3D::draw3d::Draw3DWithCPU::Initialize(
    graphics::D3DInteropHelper *pD3DInteropHelper,
    std::shared_ptr<gameOfLife3D::lifeSimulator::AbstractLifeSimulator> pLifeSimulator, UINT maxNumberOfGenerations,
    FLOAT marginRate) {
  m_pLifeSimulator = pLifeSimulator;
  m_maxNumberOfGenerations = maxNumberOfGenerations;

  XMFLOAT3 p0;
  XMFLOAT3 p1;
  Draw3DUtil::CalculateP0P1(pLifeSimulator->GetWidth(), pLifeSimulator->GetHeight(), maxNumberOfGenerations, p0, p1);

  m_pVertexGenerator = nullptr;
  m_pVertexGenerator = std::make_shared<gameOfLife3D::draw3d::VertexGenerator>();
  m_pVertexGenerator->SetWidth(pLifeSimulator->GetWidth());
  m_pVertexGenerator->SetHeight(pLifeSimulator->GetHeight());
  m_pVertexGenerator->SetP0(p0);
  m_pVertexGenerator->SetP1(p1);
  m_pVertexGenerator->SetMaxNumberOfGenerations(maxNumberOfGenerations);
  m_pVertexGenerator->SetMarginRate(marginRate);
  m_pVertexGenerator->Initialize(pD3DInteropHelper);
  return S_OK;
}

HRESULT gameOfLife3D::draw3d::Draw3DWithCPU::MoveNextGeneration(graphics::D3DInteropHelper *pD3DInteropHelper) {
  CHK_FATAL_HRESULT(m_pLifeSimulator->Compute());
  std::vector<gameOfLife3D::lifeSimulator::LifeData> lifeData(m_pLifeSimulator->GetMaxNumberOfLifeData());
  m_pLifeSimulator->CopyNext(lifeData.data());
  CHK_FATAL_HRESULT(m_pVertexGenerator->Add(lifeData.data()));
  CHK_FATAL_HRESULT(m_pVertexGenerator->Update(pD3DInteropHelper));
  return S_OK;
}

HRESULT gameOfLife3D::draw3d::Draw3DWithCPU::CreateD3DResources(graphics::D3DInteropHelper *pD3DInteropHelper,
                                                                ID3D11Device *pDevice,
                                                                ID3D11DeviceContext *pImmediateContext) {
  // Vertex Shader
  CComPtr<ID3DBlob> pVSBlob = nullptr;
  CHK_FATAL_HRESULT(
      pD3DInteropHelper->CompileShaderFromResource(MAKEINTRESOURCE(IDR_SHADER_SIMPLE_D3D), "VS", "vs_4_0", &pVSBlob));
  CHK_FATAL_HRESULT(
      pDevice->CreateVertexShader(pVSBlob->GetBufferPointer(), pVSBlob->GetBufferSize(), nullptr, &m_pVertexShader));

  // Input Layout
  D3D11_INPUT_ELEMENT_DESC layout[] = {
      {"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0},
      {"NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0},
      {"COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 24, D3D11_INPUT_PER_VERTEX_DATA, 0},
  };
  UINT numElements = ARRAYSIZE(layout);

  CHK_FATAL_HRESULT(pDevice->CreateInputLayout(layout, numElements, pVSBlob->GetBufferPointer(),
                                               pVSBlob->GetBufferSize(), &m_pInputLayout));
  pImmediateContext->IASetInputLayout(m_pInputLayout);

  // Pixel Shader
  CComPtr<ID3DBlob> pPSBlob = nullptr;
  CHK_FATAL_HRESULT(
      pD3DInteropHelper->CompileShaderFromResource(MAKEINTRESOURCE(IDR_SHADER_SIMPLE_D3D), "PS", "ps_4_0", &pPSBlob));
  CHK_FATAL_HRESULT(
      pDevice->CreatePixelShader(pPSBlob->GetBufferPointer(), pPSBlob->GetBufferSize(), nullptr, &m_pPixelShader));

  return S_OK;
}

HRESULT gameOfLife3D::draw3d::Draw3DWithCPU::RecreateSizedResources(UINT nWidth, UINT nHeight,
                                                                    graphics::D3DInteropHelper *pD3DInteropHelper,
                                                                    ID3D11Device *pDevice,
                                                                    ID3D11DeviceContext *pImmeidiateContext) {
  UNREFERENCED_PARAMETER(nWidth);
  UNREFERENCED_PARAMETER(nHeight);
  UNREFERENCED_PARAMETER(pD3DInteropHelper);
  UNREFERENCED_PARAMETER(pDevice);
  UNREFERENCED_PARAMETER(pImmeidiateContext);
  return S_OK;
}

HRESULT gameOfLife3D::draw3d::Draw3DWithCPU::Draw3D(graphics::D3DInteropHelper *pD3DInteropHelper,
                                                    ID3D11Device *pDevice, ID3D11DeviceContext *pImmediateContext) {
  UNREFERENCED_PARAMETER(pDevice);
  UINT stride = sizeof(gameOfLife3D::draw3d::VertexInfo);
  UINT offset = 0;
  pImmediateContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
  pImmediateContext->IASetInputLayout(m_pInputLayout);
  m_pVertexGenerator->Update(pD3DInteropHelper);
  ID3D11Buffer *pVertexBuffer = m_pVertexGenerator->GetVertexBuffer();
  pImmediateContext->IASetVertexBuffers(0, 1, &pVertexBuffer, &stride, &offset);
  ID3D11Buffer *pIndexBuffer = m_pVertexGenerator->GetIndexBuffer();
  pImmediateContext->IASetIndexBuffer(
      pIndexBuffer, sizeof(gameOfLife3D::draw3d::VERTEX_INDEX) == 2 ? DXGI_FORMAT_R16_UINT : DXGI_FORMAT_R32_UINT, 0);
  pImmediateContext->VSSetShader(m_pVertexShader, nullptr, 0);
  //     pImmediateContext->VSSetConstantBuffers( 0, 1, &m_pConstantBuffer );
  pImmediateContext->PSSetShader(m_pPixelShader, nullptr, 0);
  //     pImmediateContext->PSSetConstantBuffers( 0, 1, &m_pConstantBuffer );
  pImmediateContext->DrawIndexed(m_pVertexGenerator->GetNumberOfIndexes(), 0, 0);
  pImmediateContext->VSSetShader(nullptr, nullptr, 0);
  pImmediateContext->GSSetShader(nullptr, nullptr, 0);
  pImmediateContext->PSSetShader(nullptr, nullptr, 0);
  return S_OK;
}

void gameOfLife3D::draw3d::Draw3DWithCPU::DiscardDeviceResources() {
}

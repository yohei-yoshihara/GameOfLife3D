#include "stdafx.h"
#include "Common.h"
#include "gameOfLife3D/lifeSimulator/AbstractLifeSimulator.h"
#include "gameOfLife3D/draw3d/Draw3DUtil.h"
#include "gameOfLife3D/draw3d/Draw3DWithGS.h"
#include "graphics/D3DInteropHelper.h"

using namespace DirectX;

gameOfLife3D::draw3d::Draw3DWithGS::Draw3DWithGS() :
    m_pLifeSimulator(),
    m_maxNumberOfGenerations(24),
    m_pCBParams(nullptr),
    m_pVertexShader(nullptr),
    m_pGeometryShader(nullptr),
    m_pPixelShader(nullptr),
    m_pInputLayout(nullptr),
    m_pVertexBuffer(nullptr)
{
}

gameOfLife3D::draw3d::Draw3DWithGS::~Draw3DWithGS()
{
    SafeRelease(&m_pCBParams);
    SafeRelease(&m_pVertexShader);
    SafeRelease(&m_pGeometryShader);
    SafeRelease(&m_pPixelShader);
    SafeRelease(&m_pInputLayout);
    SafeRelease(&m_pVertexBuffer);
}

HRESULT gameOfLife3D::draw3d::Draw3DWithGS::Initialize(
    graphics::D3DInteropHelper *pD3DInteropHelper,
    std::shared_ptr<gameOfLife3D::lifeSimulator::AbstractLifeSimulator> pLifeSimulator,
    UINT maxNumberOfGenerations, FLOAT marginRate )
{
    UNREFERENCED_PARAMETER(pD3DInteropHelper);
    m_pLifeSimulator = pLifeSimulator;
    m_maxNumberOfGenerations = maxNumberOfGenerations;
    m_marginRate = marginRate;

    for (UINT i = 0; i < m_maxNumberOfGenerations; ++i) {
        std::shared_ptr<std::vector<VertexInfoForGS>> lifeData
                = std::make_shared<std::vector<VertexInfoForGS>>
                  (m_pLifeSimulator->GetMaxNumberOfLifeData());
        m_lifeDataArray.push_back(lifeData);
    }

    return S_OK;
}

HRESULT gameOfLife3D::draw3d::Draw3DWithGS::CreateD3DResources(
    graphics::D3DInteropHelper *pD3DInteropHelper,
    ID3D11Device *pDevice,
    ID3D11DeviceContext *pImmeidiateContext )
{
    UNREFERENCED_PARAMETER(pD3DInteropHelper);
    UNREFERENCED_PARAMETER(pImmeidiateContext);
    UINT width =  m_pLifeSimulator->GetWidth();
    UINT height = m_pLifeSimulator->GetHeight();
    UINT area = width * height;

    // Constant Buffer
    D3D11_BUFFER_DESC cbufDesc;
    ZeroMemory(&cbufDesc, sizeof(cbufDesc));
    cbufDesc.Usage = D3D11_USAGE_DEFAULT;
    cbufDesc.ByteWidth = sizeof(CBParams);
    cbufDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
    cbufDesc.CPUAccessFlags = 0;
    CHK_FATAL_HRESULT(pDevice->CreateBuffer(&cbufDesc, nullptr, &m_pCBParams));

    XMFLOAT3 p0;
    XMFLOAT3 p1;
    gameOfLife3D::draw3d::Draw3DUtil::CalculateP0P1(
        width, height, m_maxNumberOfGenerations,
        p0, p1);

    XMFLOAT3 cubeLength;
    XMFLOAT3 cubeMargin;
    gameOfLife3D::draw3d::Draw3DUtil::CalculateLengthAndMargin(
        width, height, m_maxNumberOfGenerations,
        p0, p1, m_marginRate, cubeLength, cubeMargin);

    CBParams cb;
    cb.width = width;
    cb.height = height;
    cb.width_x_height = cb.width * cb.height;
    cb.generations = m_maxNumberOfGenerations;
    cb.p0 = p0;
    cb.cubeLength = cubeLength;
    cb.halfCubeLength = XMFLOAT3(cubeLength.x / 2.0f, cubeLength.y / 2.0f, cubeLength.z / 2.0f);
    cb.cubeMargin = cubeMargin;
    pImmeidiateContext->UpdateSubresource(m_pCBParams, 0, nullptr, &cb, 0, 0);

    // Vertex Shader
    CComPtr<ID3DBlob> pVSBlob = nullptr;
    CHK_FATAL_HRESULT(
        pD3DInteropHelper->CompileShaderFromResource(
            MAKEINTRESOURCE(IDR_SHADER_SIMPLE_D3D_GS), "VS", "vs_4_0", &pVSBlob));
    CHK_FATAL_HRESULT(
        pDevice->CreateVertexShader(
            pVSBlob->GetBufferPointer(), pVSBlob->GetBufferSize(), nullptr, &m_pVertexShader));

    // Geometry Shader
    CComPtr<ID3DBlob> pGSBlob = nullptr;
    CHK_FATAL_HRESULT(
        pD3DInteropHelper->CompileShaderFromResource(
            MAKEINTRESOURCE(IDR_SHADER_SIMPLE_D3D_GS), "GS", "gs_4_0", &pGSBlob));
    CHK_FATAL_HRESULT(
        pDevice->CreateGeometryShader(
            pGSBlob->GetBufferPointer(), pGSBlob->GetBufferSize(), nullptr, &m_pGeometryShader));

    // Pixel Shader
    CComPtr<ID3DBlob> pPSBlob = nullptr;
    CHK_FATAL_HRESULT(pD3DInteropHelper->CompileShaderFromResource(
                          MAKEINTRESOURCE(IDR_SHADER_SIMPLE_D3D_GS), "PS", "ps_4_0", &pPSBlob));
    CHK_FATAL_HRESULT(pDevice->CreatePixelShader(pPSBlob->GetBufferPointer(),
                      pPSBlob->GetBufferSize(), nullptr, &m_pPixelShader ));

    // Input Layout
    D3D11_INPUT_ELEMENT_DESC layout[] = {
        {"COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0},
    };
    UINT numElements = ARRAYSIZE(layout);
    CHK_FATAL_HRESULT(pDevice->CreateInputLayout(
                          layout,
                          numElements,
                          pVSBlob->GetBufferPointer(),
                          pVSBlob->GetBufferSize(),
                          &m_pInputLayout
                      ));

    D3D11_BUFFER_DESC bufDesc;
    ZeroMemory( &bufDesc, sizeof(bufDesc) );
    bufDesc.Usage = D3D11_USAGE_DYNAMIC;
    bufDesc.ByteWidth = sizeof( VertexInfoForGS ) * area * m_maxNumberOfGenerations;
    bufDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
    bufDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
    CHK_FATAL_HRESULT(pDevice->CreateBuffer(&bufDesc, nullptr, &m_pVertexBuffer));

    return S_OK;
}

HRESULT gameOfLife3D::draw3d::Draw3DWithGS::MoveNextGeneration(
    graphics::D3DInteropHelper *pD3DInteropHelper)
{
    UNREFERENCED_PARAMETER(pD3DInteropHelper);
    CHK_FATAL_HRESULT(m_pLifeSimulator->Compute());
    std::vector<gameOfLife3D::lifeSimulator::LifeData> lifeData(m_pLifeSimulator->GetMaxNumberOfLifeData());
    m_pLifeSimulator->CopyNext(lifeData.data());
    auto vertex = std::make_shared<std::vector<VertexInfoForGS>>(m_pLifeSimulator->GetMaxNumberOfLifeData());
    for (UINT i = 0; i < lifeData.size(); ++i) {
        (*vertex)[i].color.w = static_cast<FLOAT>(lifeData[i].alive);
        (*vertex)[i].color.x = lifeData[i].color.x;
        (*vertex)[i].color.y = lifeData[i].color.y;
        (*vertex)[i].color.z = lifeData[i].color.z;
    }
    m_lifeDataArray.push_back(vertex);
    while (m_lifeDataArray.size() > m_maxNumberOfGenerations) {
        m_lifeDataArray.erase(m_lifeDataArray.begin(), m_lifeDataArray.begin() + 1);
    }
    return S_OK;
}

HRESULT gameOfLife3D::draw3d::Draw3DWithGS::RecreateSizedResources(
    UINT nWidth,
    UINT nHeight,
    graphics::D3DInteropHelper *pD3DInteropHelper,
    ID3D11Device *pDevice,
    ID3D11DeviceContext *pImmeidiateContext )
{
    UNREFERENCED_PARAMETER(nWidth);
    UNREFERENCED_PARAMETER(nHeight);
    UNREFERENCED_PARAMETER(pD3DInteropHelper);
    UNREFERENCED_PARAMETER(pDevice);
    UNREFERENCED_PARAMETER(pImmeidiateContext);
    return S_OK;
}

HRESULT gameOfLife3D::draw3d::Draw3DWithGS::Draw3D(
    graphics::D3DInteropHelper *pD3DInteropHelper,
    ID3D11Device *pDevice,
    ID3D11DeviceContext *pImmediateContext )
{
    UNREFERENCED_PARAMETER(pD3DInteropHelper);
    UNREFERENCED_PARAMETER(pDevice);
    UINT width =  m_pLifeSimulator->GetWidth();
    UINT height = m_pLifeSimulator->GetHeight();
    UINT area = width * height;

    // Update vertexes
    D3D11_MAPPED_SUBRESOURCE mappedResource;
    pImmediateContext->Map(m_pVertexBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
    VertexInfoForGS *vertexes = (VertexInfoForGS*)mappedResource.pData;
    for (UINT generation = 0; generation < m_maxNumberOfGenerations; ++generation) {
        CopyMemory(
            &vertexes[area * generation],
            &m_lifeDataArray[generation]->front(),
            area * sizeof(gameOfLife3D::lifeSimulator::LifeData));
    }
    pImmediateContext->Unmap(m_pVertexBuffer, 0);

    pImmediateContext->IASetPrimitiveTopology( D3D11_PRIMITIVE_TOPOLOGY_POINTLIST );
    pImmediateContext->IASetInputLayout( m_pInputLayout );
    UINT stride = sizeof( gameOfLife3D::draw3d::VertexInfoForGS );
    UINT offset = 0;
    pImmediateContext->IASetVertexBuffers( 0, 1, &m_pVertexBuffer, &stride, &offset );
    pImmediateContext->IASetIndexBuffer(nullptr, DXGI_FORMAT_R16_UINT, 0);
    pImmediateContext->VSSetShader(m_pVertexShader, nullptr, 0);
    pImmediateContext->VSSetConstantBuffers(1, 1, &m_pCBParams);
    pImmediateContext->GSSetShader(m_pGeometryShader, nullptr, 0);
    pImmediateContext->GSSetConstantBuffers(1, 1, &m_pCBParams);
    pImmediateContext->PSSetShader(m_pPixelShader, nullptr, 0);
    pImmediateContext->Draw(area * m_maxNumberOfGenerations, 0);

    pImmediateContext->IASetPrimitiveTopology( D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST );
    pImmediateContext->IASetInputLayout( nullptr );
    pImmediateContext->VSSetShader(nullptr, nullptr, 0);
    pImmediateContext->GSSetShader(nullptr, nullptr, 0);
    pImmediateContext->PSSetShader(nullptr, nullptr, 0);
    return S_OK;
}

void gameOfLife3D::draw3d::Draw3DWithGS::DiscardDeviceResources()
{
}

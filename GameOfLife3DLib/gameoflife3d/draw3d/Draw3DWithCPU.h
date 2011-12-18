#ifndef GAMEOFLIFE3D_DRAW3D_DRAW3DWITHCPU_H_
#define GAMEOFLIFE3D_DRAW3D_DRAW3DWITHCPU_H_

#include "gameOfLife3D/lifeSimulator/LifeData.h"
#include "gameOfLife3D/draw3d/IDraw3D.h"

namespace gameOfLife3D
{
namespace draw3d
{
class VertexGenerator;
class Draw3DWithCPU :
    public gameOfLife3D::draw3d::IDraw3D
{
private:
    std::shared_ptr<gameOfLife3D::lifeSimulator::AbstractLifeSimulator> m_pLifeSimulator;
    UINT m_maxNumberOfGenerations;
    std::shared_ptr<VertexGenerator> m_pVertexGenerator;
    ID3D11VertexShader    *m_pVertexShader;
    ID3D11InputLayout     *m_pInputLayout;
    ID3D11PixelShader     *m_pPixelShader;

public:
    Draw3DWithCPU();
    virtual ~Draw3DWithCPU();
    virtual HRESULT Initialize(
        graphics::D3DInteropHelper *pD3DInteropHelper,
        std::shared_ptr<gameOfLife3D::lifeSimulator::AbstractLifeSimulator> pLifeSimulator,
        UINT maxNumberOfGenerations, FLOAT marginRate);
    virtual HRESULT MoveNextGeneration(
        graphics::D3DInteropHelper *pD3DInteropHelper);
    virtual HRESULT CreateD3DResources(
        graphics::D3DInteropHelper *pD3DInteropHelper,
        ID3D11Device *pDevice,
        ID3D11DeviceContext *pImmediateContext);
    virtual HRESULT RecreateSizedResources(
        UINT nWidth,
        UINT nHeight,
        graphics::D3DInteropHelper *pD3DInteropHelper,
        ID3D11Device *pDevice,
        ID3D11DeviceContext *pImmeidiateContext);
    virtual HRESULT Draw3D(
        graphics::D3DInteropHelper *pD3DInteropHelper,
        ID3D11Device *pDevice,
        ID3D11DeviceContext *pImmediateContext);
    virtual void DiscardDeviceResources();
};
}
}

#endif // GAMEOFLIFE3D_DRAW3D_DRAW3DWITHCPU_H_

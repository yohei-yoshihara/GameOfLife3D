#ifndef GRAPHICS_DRAW3D_IDRAW3D_H_
#define GRAPHICS_DRAW3D_IDRAW3D_H_

#include "gameOfLife3D/lifeSimulator/LifeData.h"
#include "gameOfLife3D/lifeSimulator/AbstractLifeSimulator.h"

namespace gameOfLife3D
{
namespace draw3d
{

class IDraw3D
{
public:
    virtual HRESULT Initialize(
        graphics::D3DInteropHelper *pD3DInteropHelper,
        std::shared_ptr<gameOfLife3D::lifeSimulator::AbstractLifeSimulator> pLifeSimulator,
        UINT maxNumberOfGenerations,
        FLOAT marginRate) = 0;
    virtual HRESULT MoveNextGeneration(
        graphics::D3DInteropHelper *pD3DInteropHelper) = 0;
    virtual HRESULT CreateD3DResources(
        graphics::D3DInteropHelper *pD3DInteropHelper,
        ID3D11Device *pDevice,
        ID3D11DeviceContext *pImmeidiateContext) = 0;
    virtual HRESULT RecreateSizedResources(
        UINT nWidth,
        UINT nHeight,
        graphics::D3DInteropHelper *pD3DInteropHelper,
        ID3D11Device *pDevice,
        ID3D11DeviceContext *pImmeidiateContext) = 0;
    virtual HRESULT Draw3D(
        graphics::D3DInteropHelper *pD3DInteropHelper,
        ID3D11Device *pDevice,
        ID3D11DeviceContext *pImmeidiateContext) = 0;
    virtual void DiscardDeviceResources() = 0;
};
}
}

#endif // GRAPHICS_DRAW3D_IDRAW3D_H_

#ifndef GAMEOFLIFE3D_LIFESIMULATOR_H_
#define GAMEOFLIFE3D_LIFESIMULATOR_H_

#include "graphics/GraphicsCommon.h"
#include "gameOfLife3D/lifeSimulator/LifeData.h"
#include "gameOfLife3D/lifeSimulator/AbstractLifeSimulator.h"

namespace gameOfLife3D
{
namespace lifeSimulator
{
#define NUMBER_OF_BORNCOLORS 7

struct CBParams {
    int width;
    int height;
    int r1;
    int r2;
    int r3;
    int r4;
};

class LifeSimulatorCS : public gameOfLife3D::lifeSimulator::AbstractLifeSimulator
{
private:
    static const XMFLOAT3 gameOfLife3D::lifeSimulator::LifeSimulatorCS::s_bornColors[NUMBER_OF_BORNCOLORS];

    UINT m_currentIndex;
    UINT m_nextIndex;

    ID3D11Device              *m_pDevice;
    ID3D11DeviceContext       *m_pImmediateContext;

    ID3D11ComputeShader       *m_pComputeShader;
    ID3D11Buffer              *m_pDataBuffer[NUMBER_OF_BUFFERS];
    ID3D11ShaderResourceView  *m_pDataBufferSRV[NUMBER_OF_BUFFERS];
    ID3D11UnorderedAccessView *m_pDataBufferUAV[NUMBER_OF_BUFFERS];

    ID3D11Buffer              *m_pCBParamsBuffer;

public:
    LifeSimulatorCS();
    virtual ~LifeSimulatorCS();

    virtual HRESULT Initialize(graphics::D3DInteropHelper *pD3DInteropHelper);
    virtual HRESULT Compute();
    virtual void CopyCurrent(OUT gameOfLife3D::lifeSimulator::LifeData *pLifeData);
    virtual void CopyNext(OUT gameOfLife3D::lifeSimulator::LifeData *pLifeData);
    virtual HRESULT LoadRuleFile(
        graphics::D3DInteropHelper *pD3DInteropHelper,
        const std::wstring &fileName,
        OUT std::wstring &errorMessage);

    void Copy(UINT index, OUT gameOfLife3D::lifeSimulator::LifeData *pLifeData);
    ID3D11Buffer* CreateAndCopyToDebugBuf(ID3D11Buffer* pBuffer);
    void Dump(gameOfLife3D::lifeSimulator::LifeData *data);
};

}
}

#endif // GAMEOFLIFE3D_LIFESIMULATOR_H_

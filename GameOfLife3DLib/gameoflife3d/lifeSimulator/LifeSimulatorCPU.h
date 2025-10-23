#ifndef GAMEOFLIFE3D_LIFESIMULATOR_LIFESIMULATORCPU_H_
#define GAMEOFLIFE3D_LIFESIMULATOR_LIFESIMULATORCPU_H_

#include "graphics/GraphicsCommon.h"
#include "gameOfLife3D/lifeSimulator/LifeData.h"
#include "gameOfLife3D/lifeSimulator/AbstractLifeSimulator.h"

namespace gameOfLife3D {
  namespace lifeSimulator {

    class LifeSimulatorCPU : public gameOfLife3D::lifeSimulator::AbstractLifeSimulator {
    private:
      UINT m_currentIndex;
      UINT m_nextIndex;
      std::vector<gameOfLife3D::lifeSimulator::LifeData> m_buffer[NUMBER_OF_BUFFERS];

    public:
      LifeSimulatorCPU();
      virtual ~LifeSimulatorCPU();
      virtual HRESULT Initialize(graphics::D3DInteropHelper *pD3DInteropHelper);
      virtual HRESULT Compute();
      virtual void CopyCurrent(OUT gameOfLife3D::lifeSimulator::LifeData *pLifeData);
      virtual void CopyNext(OUT gameOfLife3D::lifeSimulator::LifeData *pLifeData);
      virtual HRESULT LoadRuleFile(graphics::D3DInteropHelper *pD3DInteropHelper, const std::wstring &fileName,
                                   OUT std::wstring &errorMessage) {
        UNREFERENCED_PARAMETER(pD3DInteropHelper);
        UNREFERENCED_PARAMETER(fileName);
        UNREFERENCED_PARAMETER(errorMessage);
        return E_NOTIMPL;
      };
    };
  }
}

#endif // GAMEOFLIFE3D_LIFESIMULATOR_LIFESIMULATORCPU_H_

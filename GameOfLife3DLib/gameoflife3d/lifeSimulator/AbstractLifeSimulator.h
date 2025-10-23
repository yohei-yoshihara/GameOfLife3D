#ifndef GAMEOFLIFE3D_ABSTRACTLIFESIMULATOR_H_
#define GAMEOFLIFE3D_ABSTRACTLIFESIMULATOR_H_

#include "graphics/GraphicsCommon.h"
#include "gameOfLife3D/lifeSimulator/LifeData.h"

namespace gameOfLife3D {
  namespace lifeSimulator {
#define NUMBER_OF_BUFFERS 2

    class AbstractLifeSimulator {
    private:
      UINT m_width;
      UINT m_height;
      UINT m_r1;
      UINT m_r2;
      UINT m_r3;
      UINT m_r4;
      std::vector<gameOfLife3D::lifeSimulator::LifeData> m_initialData;

    public:
      AbstractLifeSimulator();
      virtual ~AbstractLifeSimulator();

      virtual HRESULT Initialize(graphics::D3DInteropHelper *pD3DInteropHelper) = 0;
      virtual HRESULT Compute() = 0;
      virtual void CopyCurrent(OUT gameOfLife3D::lifeSimulator::LifeData *pLifeData) = 0;
      virtual void CopyNext(OUT gameOfLife3D::lifeSimulator::LifeData *pLifeData) = 0;
      virtual HRESULT LoadRuleFile(graphics::D3DInteropHelper *pD3DInteropHelper, const std::wstring &fileName,
                                   OUT std::wstring &errorMessage)
          = 0;

      void SetInitialData(const std::vector<gameOfLife3D::lifeSimulator::LifeData> &initialData) {
        m_initialData = initialData;
      }
      const std::vector<gameOfLife3D::lifeSimulator::LifeData> &GetInitialData() const {
        return m_initialData;
      }
      UINT GetMaxNumberOfLifeData() const {
        return m_width * m_height;
      }
      void SetWidth(UINT width) {
        m_width = width;
      }
      UINT GetWidth() const {
        return m_width;
      }
      void SetHeight(UINT height) {
        m_height = height;
      }
      UINT GetHeight() const {
        return m_height;
      }
      void SetR1(UINT r1) {
        m_r1 = r1;
      }
      UINT GetR1() const {
        return m_r1;
      }
      void SetR2(UINT r2) {
        m_r2 = r2;
      }
      UINT GetR2() const {
        return m_r2;
      }
      void SetR3(UINT r3) {
        m_r3 = r3;
      }
      UINT GetR3() const {
        return m_r3;
      }
      void SetR4(UINT r4) {
        m_r4 = r4;
      }
      UINT GetR4() const {
        return m_r4;
      }
    };
  }
}
#endif // GAMEOFLIFE3D_ABSTRACTLIFESIMULATOR_H_

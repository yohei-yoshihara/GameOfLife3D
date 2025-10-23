#ifndef GAMEOFLIFE3D_LIFESIMULATOR_LIFEDATA_H_
#define GAMEOFLIFE3D_LIFESIMULATOR_LIFEDATA_H_

namespace gameOfLife3D {
  namespace lifeSimulator {
    class LifeData {
    public:
      LifeData()
          : alive(0)
          , color(0, 0, 0) {
      }
      int alive;
      DirectX::XMFLOAT3 color;
    };
  }
}

#endif // GAMEOFLIFE3D_LIFESIMULATOR_LIFEDATA_H_

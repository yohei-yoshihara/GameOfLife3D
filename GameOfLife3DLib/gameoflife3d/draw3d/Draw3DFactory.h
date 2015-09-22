#ifndef GAMEOFLIFE3D_DRAW3D_DRAW3DFACTORY_H_
#define GAMEOFLIFE3D_DRAW3D_DRAW3DFACTORY_H_

#include "gameOfLife3D/draw3d/IDraw3D.h"

namespace gameOfLife3D {
  namespace draw3d {
    enum DRAW3D_TYPE {
      DRAW3D_TYPE_CPU,
      DRAW3D_TYPE_GEOMETRYSHADER,
    };

    class Draw3DFactory {
    public:
      static std::shared_ptr<gameOfLife3D::draw3d::IDraw3D> Create(gameOfLife3D::draw3d::DRAW3D_TYPE draw3dType);
    };
  }
}

#endif // GAMEOFLIFE3D_DRAW3D_DRAW3DFACTORY_H_

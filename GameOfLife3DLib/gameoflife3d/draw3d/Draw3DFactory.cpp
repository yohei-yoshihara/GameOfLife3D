#include "stdafx.h"
#include "Common.h"
#include "gameOfLife3D/draw3d/IDraw3D.h"
#include "gameOfLife3D/draw3d/Draw3DWithCPU.h"
#include "gameOfLife3D/draw3d/Draw3DWithGS.h"
#include "gameOfLife3D/draw3d/Draw3DFactory.h"

std::shared_ptr<gameOfLife3D::draw3d::IDraw3D>
gameOfLife3D::draw3d::Draw3DFactory::Create(gameOfLife3D::draw3d::DRAW3D_TYPE draw3dType) {
  std::shared_ptr<gameOfLife3D::draw3d::IDraw3D> pDraw3D;
  switch (draw3dType) {
  case DRAW3D_TYPE_CPU:
    pDraw3D = std::make_shared<gameOfLife3D::draw3d::Draw3DWithCPU>();
    break;
  case DRAW3D_TYPE_GEOMETRYSHADER:
    pDraw3D = std::make_shared<gameOfLife3D::draw3d::Draw3DWithGS>();
    break;
  }
  return pDraw3D;
}

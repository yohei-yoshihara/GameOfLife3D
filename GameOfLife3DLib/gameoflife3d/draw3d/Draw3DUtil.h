#ifndef GAMEOFLIFE3D_DRAW3D_DRAW3DUTIL_H_
#define GAMEOFLIFE3D_DRAW3D_DRAW3DUTIL_H_

namespace gameOfLife3D {
  namespace draw3d {

    class Draw3DUtil {
    public:
      static void CalculateP0P1(UINT width, UINT height, UINT generations, OUT DirectX::XMFLOAT3 &p0,
                                OUT DirectX::XMFLOAT3 &p1);
      static void CalculateLengthAndMargin(UINT width, UINT height, UINT generations, DirectX::XMFLOAT3 p0,
                                           DirectX::XMFLOAT3 p1, FLOAT marginRate, OUT DirectX::XMFLOAT3 &cubeLength,
                                           OUT DirectX::XMFLOAT3 &cubeMargin);
    };
  }
}

#endif // GAMEOFLIFE3D_DRAW3D_DRAW3DUTIL_H_

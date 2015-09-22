#ifndef GAMEOFLIFE3D_DRAW3D_DRAW3DWITHGS_H_
#define GAMEOFLIFE3D_DRAW3D_DRAW3DWITHGS_H_

#include "gameOfLife3D/draw3d/IDraw3D.h"

namespace gameOfLife3D {
  namespace draw3d {

    struct CBParams {
      INT32 width;
      INT32 height;
      INT32 width_x_height;
      INT32 generations;
      DirectX::XMFLOAT3 p0;
      FLOAT packing0;
      DirectX::XMFLOAT3 cubeLength;
      FLOAT packing1;
      DirectX::XMFLOAT3 halfCubeLength;
      FLOAT packing2;
      DirectX::XMFLOAT3 cubeMargin;
      FLOAT packing3;
    };

    struct VertexInfoForGS {
      DirectX::XMFLOAT4 color;
    };

    class Draw3DWithGS : public gameOfLife3D::draw3d::IDraw3D {
    private:
      std::shared_ptr<gameOfLife3D::lifeSimulator::AbstractLifeSimulator> m_pLifeSimulator;
      UINT m_maxNumberOfGenerations;
      FLOAT m_marginRate;
      ID3D11Buffer *m_pCBParams;
      ID3D11VertexShader *m_pVertexShader;
      ID3D11GeometryShader *m_pGeometryShader;
      ID3D11PixelShader *m_pPixelShader;
      ID3D11InputLayout *m_pInputLayout;
      ID3D11Buffer *m_pVertexBuffer;
      std::vector<std::shared_ptr<std::vector<VertexInfoForGS>>> m_lifeDataArray;

    public:
      Draw3DWithGS();
      virtual ~Draw3DWithGS();
      virtual HRESULT Initialize(graphics::D3DInteropHelper *pD3DInteropHelper,
                                 std::shared_ptr<gameOfLife3D::lifeSimulator::AbstractLifeSimulator> pLifeSimulator,
                                 UINT maxNumberOfGenerations, FLOAT marginRate);
      virtual HRESULT MoveNextGeneration(graphics::D3DInteropHelper *pD3DInteropHelper);
      virtual HRESULT CreateD3DResources(graphics::D3DInteropHelper *pD3DInteropHelper, ID3D11Device *pDevice,
                                         ID3D11DeviceContext *pImmediateContext);
      virtual HRESULT RecreateSizedResources(UINT nWidth, UINT nHeight, graphics::D3DInteropHelper *pD3DInteropHelper,
                                             ID3D11Device *pDevice, ID3D11DeviceContext *pImmeidiateContext);
      virtual HRESULT Draw3D(graphics::D3DInteropHelper *pD3DInteropHelper, ID3D11Device *pDevice,
                             ID3D11DeviceContext *pImmediateContext);
      virtual void DiscardDeviceResources();
    };
  }
}

#endif // GAMEOFLIFE3D_DRAW3D_DRAW3DWITHGS_H_

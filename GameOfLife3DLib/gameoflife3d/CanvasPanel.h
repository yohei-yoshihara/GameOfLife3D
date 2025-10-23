#ifndef GAMEOFLIFE3D_CANVASPANEL_H_
#define GAMEOFLIFE3D_CANVASPANEL_H_

#include "Common.h"
#include "gameOfLife3D/lifeSimulator/AbstractLifeSimulator.h"
#include "gameOfLife3D/draw3d/IDraw3D.h"
#include "gameoflife3D/dialogs/SettingData.h"
#include "animation/IAnimationWindow.h"
#include "graphics/D3DInteropHelper.h"
#include "graphics/ID3DInteropAdapter.h"
#include "graphics/GraphicsCommon.h"
#include "ui/UICommon.h"

namespace graphics {
  class FPSUtil;
}

namespace touch {
  class ManipulationHelper;
}

namespace gameOfLife3D {
  namespace dialogs {
    class SettingsDialog;
  }
  namespace io {
    class LifeFile;
  }
  namespace draw3d {
    class VertexGenerator;
  }
  class MainWnd;

#define NUMBER_OF_LIGHTS 2

  struct ConstantBuffer {
    DirectX::XMMATRIX mWorld;
    DirectX::XMMATRIX mView;
    DirectX::XMMATRIX mProjection;
    DirectX::XMFLOAT4 vLightDir[NUMBER_OF_LIGHTS];
    DirectX::XMFLOAT4 vLightColor[NUMBER_OF_LIGHTS];
  };

  struct EyeInfo {
    DirectX::XMFLOAT4 eye;
    DirectX::XMFLOAT4 at;
    DirectX::XMFLOAT4 up;
  };

  struct EyePosInfo {
    float theta;
    float phi;
    float radius;
  };

  struct LightInfo {
    DirectX::XMFLOAT4 vLightDir;
    DirectX::XMFLOAT4 vLightColor;
  };

  class CanvasPanel : public graphics::ID3DInteropAdapter,
                      public animation::IAnimationWindow,
                      public std::enable_shared_from_this<CanvasPanel> {
  private:
    bool m_isInitialized;
    UINT m_ribbonHeight;
    HWND m_hwnd;
    std::unique_ptr<graphics::D3DInteropHelper> m_pD3DInteropHelper;
    std::shared_ptr<touch::ManipulationHelper> m_manipulationHelper;
    std::weak_ptr<MainWnd> m_mainWnd;
    std::shared_ptr<ui::UIRoot> m_uiRoot;
    std::shared_ptr<gameOfLife3D::dialogs::SettingsDialog> m_settingsDialog;

    // Direct 3D
    ID3D11Buffer *m_pConstantBuffer;
    DirectX::XMFLOAT4X4 m_world;
    DirectX::XMFLOAT4X4 m_view;
    DirectX::XMFLOAT4X4 m_projection;
    EyeInfo m_eyeInfo;
    EyePosInfo m_eyePosInfo;
    std::array<LightInfo, NUMBER_OF_LIGHTS> m_lightInfo;

    std::shared_ptr<gameOfLife3D::lifeSimulator::AbstractLifeSimulator> m_pLifeSimulator;
    std::shared_ptr<gameOfLife3D::draw3d::VertexGenerator> m_pVertexGenerator;
    UINT m_lifeAreaWidth;
    UINT m_lifeAreaHeight;
    UINT m_numberOfGenerations;
    FLOAT m_marginRate;
    UINT m_updateInterval; // m sec
    gameOfLife3D::dialogs::VERTEX_GENERATION_METHOD m_vertexGenerationMethod;
    bool m_parametersModified;
    bool m_running;
    std::vector<gameOfLife3D::lifeSimulator::LifeData> m_initialLifeData;
    std::wstring m_csFileName;

    bool m_fpsEnabled;
    std::unique_ptr<graphics::FPSUtil> m_fpsUtil;
    CComPtr<IDWriteTextFormat> m_pFpsTextFormat;
    CComPtr<IDWriteTextLayout> m_pFpsTextLayout;
    CComPtr<ID2D1SolidColorBrush> m_pFpsTextBrush;

    std::shared_ptr<gameOfLife3D::draw3d::IDraw3D> m_pDraw3D;

  protected:
    HRESULT _UpdateConstantBuffer(graphics::D3DInteropHelper *pD3DInteropHelper, ID3D11Device *pDevice,
                                  ID3D11DeviceContext *pImmediateContext);

  public:
    CanvasPanel(void);
    virtual ~CanvasPanel(void);
    bool IsInitialized(void) {
      return m_isInitialized;
    }
    void SetGestureConfigs(HWND hWnd);
    HRESULT Initialize(std::weak_ptr<MainWnd> mainWnd);
    HRESULT InitializeComponents();
    HRESULT OnCreate();
    HWND GetHWnd() const {
      return m_hwnd;
    }
    HRESULT OnRender();

    // Resize the render target.
    void OnResize(UINT width, UINT height);
    HRESULT ResizeByParentResized();
    HRESULT ResizeByRibbonResized(UINT ribbonHeight);
    virtual HRESULT Invalidate();

    // interface graphics::ID3DInteropAdapter
    virtual HRESULT CreateDeviceIndependentResources(graphics::D3DInteropHelper *pD3DInteropHelper,
                                                     ID2D1Factory *pD2DFactory, IWICImagingFactory *pWICFactory,
                                                     IDWriteFactory *pDWriteFactory);
    virtual HRESULT CreateD2DResources(graphics::D3DInteropHelper *pD3DInteropHelper, ID2D1Factory *pD2DFactory,
                                       ID2D1RenderTarget *pRenderTarget);
    virtual HRESULT Draw2D(graphics::D3DInteropHelper *pD3DInteropHelper, ID2D1RenderTarget *pRenderTarget);

    // Direct 3D
    virtual HRESULT CreateD3DResources(graphics::D3DInteropHelper *pD3DInteropHelper, ID3D11Device *pDevice,
                                       ID3D11DeviceContext *pImmeidiateContext);
    virtual HRESULT RecreateSizedResources(UINT nWidth, UINT nHeight, graphics::D3DInteropHelper *pD3DInteropHelper,
                                           ID3D11Device *pDevice, ID3D11DeviceContext *pImmeidiateContext);
    virtual HRESULT Draw3D(graphics::D3DInteropHelper *pD3DInteropHelper, ID3D11Device *pDevice,
                           ID3D11DeviceContext *pImmeidiateContext);
    virtual void DiscardDeviceResources();

    static LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

    void MoveToNextGeneration(graphics::D3DInteropHelper *pD3DInteropHelper);

    void NotifyHorizontalScrollbarChanged(float angle);
    void NotifyVerticalScrollbarChanged(float angle);
    void UpdateEyePos();
    void OnStart();
    void OnStop();
    void OnSettings();
    void InitializeLifeSimulator(graphics::D3DInteropHelper *pD3DInteropHelper);
    void UpdateEyeRadius(float delta);
    void SetLifeFile(const std::shared_ptr<gameOfLife3D::io::LifeFile> &pLifeFile);
    void GetLifeFile(std::shared_ptr<gameOfLife3D::io::LifeFile> &pLifeFile);
    void RandomizeInitialData();
    void OpenCSFile(const std::wstring &fileName);
    void SetFPSEnable(bool fpsEnabled) {
      m_fpsEnabled = fpsEnabled;
    }
    bool IsFPSEnabled() const {
      return m_fpsEnabled;
    }
  };
}

#endif // GAMEOFLIFE3D_CANVASPANEL_H_
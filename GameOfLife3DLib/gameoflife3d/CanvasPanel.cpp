#include "StdAfx.h"
#include "util/Utils.h"
#include "util/OSVersionChecker.h"
#include "graphics/GraphicsCommon.h"
#include "graphics/brush/BrushSet.h"
#include "graphics/brush/SolidColorBrush.h"
#include "graphics/brush/LinearGradientBrush.h"
#include "graphics/brush/RadialGradientBrush.h"
#include "graphics//color/ColorSet.h"
#include "graphics/color/GradientStop.h"
#include "graphics/color/GradientStopCollection.h"
#include "graphics/color/SolidColor.h"
#include "graphics/color/LinearGradientColor.h"
#include "graphics/color/RadialGradientColor.h"
#include "graphics/figure/RectangleFigure.h"
#include "graphics/figure/EllipseFigure.h"
#include "graphics/figure/TriangleFigure.h"
#include "graphics/filter/GaussFilter.h"
#include "graphics/filter/InnerGaussFilter.h"
#include "animation/AnimationManagerEventHandler.h"
#include "touch/ManipulationHelper.h"
#include "mui/MUI.h"
#include "ui/UIRoot.h"
#include "ui/UILabel.h"
#include "ui/UIButton.h"
#include "ui/UIRadioButtonGroup.h"
#include "ui/UIRadioButton.h"
#include "ui/UITextField.h"
#include "ui/UIInternalDialog.h"
#include "ui/UIListBox.h"
#include "ui/UIListBoxItem.h"
#include "ui/UIScrollBar.h"
#include "ui/event/UIActionEvent.h"
#include "graphics/decorator/Frame.h"
#include "graphics/decorator/FrameDecorator.h"
#include "graphics/FPSUtil.h"
#include "gameOfLife3D/io/LifeFile.h"
#include "gameOfLife3D/lifeSimulator/LifeSimulatorFactory.h"
#include "gameOfLife3D/dialogs/SettingsDialog.h"
#include "gameOfLife3D/MainWnd.h"
#include "gameOfLife3D/draw3d/VertexGenerator.h"
#include "gameOfLife3D/draw3d/Draw3DFactory.h"
#include "gameOfLife3D/CanvasPanel.h"

using namespace DirectX;

#if defined(DEBUG) || defined(_DEBUG)
//#define DEBUG_CANVASPANEL
//#define DEBUG_CANVASPANEL_WINMSG
#endif

#define INITIAL_SIZE 24

const wchar_t *COLORSETDEF =
    L"solid[solid1](rgba(1.0,0.0,0.0,1.0)),"
    L"linear[linear1](TOP_BOTTOM,rgba(1.0,0.0,0.0,1.0)@0.0,rgba(0.0,1.0,0.0,1.0)@1.0),"
    L"radial[radial1](-0.5,-0.5,rgba(1.0,0.0,0.0,1.0)@0.0,rgba(0.0,0.0,1.0,1.0)@1.0)";

gameOfLife3D::CanvasPanel::CanvasPanel(void) :
    m_isInitialized(false),
    m_ribbonHeight(0),
    m_hwnd(0),
    m_pD3DInteropHelper(std::make_unique<graphics::D3DInteropHelper>()),
    m_manipulationHelper(std::make_shared<touch::ManipulationHelper>()),
    m_mainWnd(),
    m_uiRoot(std::make_shared<ui::UIRoot>()),
    m_settingsDialog(std::make_shared<gameOfLife3D::dialogs::SettingsDialog>()),
    m_pConstantBuffer(nullptr),
    m_world(),
    m_view(),
    m_projection(),
    m_lightInfo(),
    m_pLifeSimulator(),
    m_pVertexGenerator(),
    m_lifeAreaWidth(INITIAL_SIZE),
    m_lifeAreaHeight(INITIAL_SIZE),
    m_numberOfGenerations(INITIAL_SIZE),
    m_marginRate(0.2f),
    m_updateInterval(1000),
    m_vertexGenerationMethod(gameOfLife3D::dialogs::VERTEX_GENERATION_METHOD_GPU),
    m_parametersModified(false),
    m_running(false),
    m_initialLifeData(INITIAL_SIZE * INITIAL_SIZE),
    m_fpsEnabled(false),
    m_fpsUtil(std::make_unique<graphics::FPSUtil>()),
    m_pFpsTextFormat(nullptr),
    m_pFpsTextLayout(nullptr),
    m_pFpsTextBrush(nullptr),
    m_pDraw3D()
{
    XMMATRIX world = XMMatrixIdentity();
    XMStoreFloat4x4(&m_world, world);

    m_eyePosInfo.phi = XM_PIDIV4;
    m_eyePosInfo.theta = XM_PIDIV4;
    m_eyePosInfo.radius = 3.0f;

    UpdateEyePos();
    m_eyeInfo.at  = XMFLOAT4(0.0f, 0.0f, 0.0f, 0.0f);
    m_eyeInfo.up  = XMFLOAT4(0.0f, 1.0f, 0.0f, 0.0f);

    m_lightInfo[0].vLightDir = XMFLOAT4( -0.577f, 0.577f, -0.577f, 1.0f );
    m_lightInfo[0].vLightColor = XMFLOAT4( 0.1f, 0.1f, 0.1f, 1.0f );

    m_lightInfo[1].vLightDir = XMFLOAT4( 0.0f, 0.5f, -1.0f, 1.0f );
    m_lightInfo[1].vLightColor = XMFLOAT4( 1.0f, 1.0f, 1.0f, 1.0f );

    RandomizeInitialData();
}

gameOfLife3D::CanvasPanel::~CanvasPanel(void)
{
    SafeRelease(&m_pConstantBuffer);
}

HRESULT gameOfLife3D::CanvasPanel::Initialize(std::weak_ptr<MainWnd> mainWnd)
{
#ifdef DEBUG_CANVASPANEL
    LOG_ENTER(SEVERITY_LEVEL_DEBUG);
#endif

    HRESULT hr;

    m_pD3DInteropHelper->SetD3DInteropAdapter(shared_from_this());
    m_pD3DInteropHelper->SetAnimationWindow(shared_from_this());
    CHK_FATAL_HRESULT(m_pD3DInteropHelper->CreateDeviceIndependentResources());

    auto gaussFilter = std::make_shared<graphics::filter::GaussFilter>();
    m_pD3DInteropHelper->RegisterGeometryFilter(L"GaussFilter", gaussFilter);
    CHK_FATAL_HRESULT(gaussFilter->Initialize());

    auto innerGaussFilter = std::make_shared<graphics::filter::InnerGaussFilter>();
    m_pD3DInteropHelper->RegisterGeometryFilter(L"InnerGaussFilter", innerGaussFilter);
    CHK_FATAL_HRESULT(innerGaussFilter->Initialize());


    m_mainWnd = mainWnd;
    auto _mainWnd = m_mainWnd.lock();

    m_ribbonHeight = _mainWnd->GetRibbonHeight();
#ifdef DEBUG_CANVASPANEL
    LOG(SEVERITY_LEVEL_DEBUG) << L"ribbon height = " << m_ribbonHeight;
#endif

    WNDCLASSEX wcex = {sizeof(WNDCLASSEX)};
    wcex.style         = CS_HREDRAW | CS_VREDRAW | CS_DBLCLKS;
    wcex.lpfnWndProc   = CanvasPanel::WndProc;
    wcex.cbClsExtra    = 0;
    wcex.cbWndExtra    = sizeof(LONG_PTR);
    wcex.hInstance     = HINST_THISCOMPONENT;
    wcex.hbrBackground = nullptr;
    wcex.lpszMenuName  = nullptr;
    wcex.hCursor       = LoadCursor(nullptr, IDC_ARROW);
    wcex.lpszClassName = L"CanvasPanel";
    RegisterClassEx(&wcex);

    FLOAT dpiX, dpiY;
    m_pD3DInteropHelper->GetD2DFactory()->GetDesktopDpi(&dpiX, &dpiY);

    HWND parentHwnd = _mainWnd->GetHWnd();
    RECT rc;
    ::GetClientRect(parentHwnd, &rc);
    rc.top = m_ribbonHeight;
    rc.bottom -= m_ribbonHeight;

    m_hwnd = CreateWindow(
                 L"CanvasPanel",
                 L"CanvasPanel",
                 WS_CHILD | WS_VISIBLE | WS_CLIPCHILDREN /*| WS_HSCROLL | WS_VSCROLL*/,
                 rc.left, rc.top, rc.right, rc.bottom,
                 parentHwnd,
                 nullptr,
                 HINST_THISCOMPONENT,
                 this
             );
    hr = m_hwnd ? S_OK : E_FAIL;
    if (SUCCEEDED(hr)) {
        m_pD3DInteropHelper->SetHWnd(m_hwnd);
        ::SetFocus(m_hwnd);
        m_uiRoot->SetHWnd(m_hwnd);
        InitializeComponents();
        SetGestureConfigs(m_hwnd);
        ShowWindow(m_hwnd, SW_SHOWNORMAL);
        UpdateWindow(m_hwnd);
    }

    m_isInitialized = true;
#ifdef DEBUG_CANVASPANEL
    LOG_LEAVE(SEVERITY_LEVEL_DEBUG);
#endif
    return hr;
}

HRESULT gameOfLife3D::CanvasPanel::InitializeComponents()
{
    // scroll bar
    auto horizontalScrollBar = std::make_shared<ui::UIScrollBar>();
    horizontalScrollBar->SetMinValue(0.0f);
    horizontalScrollBar->SetMaxValue(360.0f);
    horizontalScrollBar->SetCurrentValue(45.0f);
    horizontalScrollBar->SetPageSize(20.0f);
    horizontalScrollBar->SetBounds(0, 0, 0, 24);
    horizontalScrollBar->SetAnchor(
        ui::UIAnchor(ui::ANCHOR_LEFT | ui::ANCHOR_RIGHT | ui::ANCHOR_BOTTOM, 60.0f, 20.0f, 60.0f, 20.0f));
    horizontalScrollBar->SetDirection(ui::UIScrollBar::SCROLLBAR_DIRECTION_HORIZONTAL);
    horizontalScrollBar->AddPositionChangedCallback([&](float angle) {
        NotifyHorizontalScrollbarChanged(angle);
    });
    m_uiRoot->AddElement(horizontalScrollBar);

    auto verticalScrollBar = std::make_shared<ui::UIScrollBar>();
    verticalScrollBar->SetMinValue(0.0f);
    verticalScrollBar->SetMaxValue(360.0f);
    verticalScrollBar->SetCurrentValue(45.0f);
    verticalScrollBar->SetPageSize(20.0f);
    verticalScrollBar->SetBounds(0, 0, 24, 0);
    verticalScrollBar->SetAnchor(ui::UIAnchor(ui::ANCHOR_TOP | ui::ANCHOR_RIGHT | ui::ANCHOR_BOTTOM, 20.0f, 60.0f, 20.0f, 60.0f));
    verticalScrollBar->SetDirection(ui::UIScrollBar::SCROLLBAR_DIRECTION_VERTICAL);
    verticalScrollBar->AddPositionChangedCallback([&](float angle) {
        NotifyVerticalScrollbarChanged(angle);
    });
    m_uiRoot->AddElement(verticalScrollBar);

    m_settingsDialog->SetHWnd(m_mainWnd.lock()->GetHWnd());
    m_settingsDialog->SetSettingModifiedCallback([&](gameOfLife3D::dialogs::SettingData settingData) {
        m_lifeAreaWidth = settingData.width;
        m_lifeAreaHeight = settingData.height;
        m_numberOfGenerations = settingData.generations;
        m_updateInterval = settingData.updateInterval;
        m_vertexGenerationMethod = settingData.vertexGenerationMethod;
        RandomizeInitialData();
        m_parametersModified = true;
    });
    m_settingsDialog->Initialize(m_mainWnd.lock()->GetTSFManager());
    m_uiRoot->AddDialog(L"settingsDialog", m_settingsDialog->GetDialog());

    m_manipulationHelper->SetUIContainer(m_uiRoot);
    m_uiRoot->Initialize(m_pD3DInteropHelper.get());

    return S_OK;
}

HRESULT gameOfLife3D::CanvasPanel::OnCreate()
{
    return S_OK;
}

void gameOfLife3D::CanvasPanel::SetGestureConfigs(HWND hWnd)
{
    if (!util::OSVersionChecker::IsWin7()) {
        return;
    }
    GESTURECONFIG gc[5];
    UINT uiGcs = 5;
    gc[0].dwID = GID_ZOOM;
    gc[0].dwWant = GC_ZOOM;
    gc[0].dwBlock = 0;
    gc[1].dwID = GID_PAN;
    gc[1].dwWant = GC_PAN_WITH_INERTIA;
    gc[1].dwBlock = 0;
    gc[2].dwID = GID_ROTATE;
    gc[2].dwWant = 0;
    gc[2].dwBlock = GC_ROTATE;
    gc[3].dwID = GID_TWOFINGERTAP;
    gc[3].dwWant = GC_TWOFINGERTAP;
    gc[3].dwBlock = 0;
    gc[4].dwID = GID_PRESSANDTAP;
    gc[4].dwWant = GC_PRESSANDTAP;
    gc[4].dwBlock = 0;
    BOOL bResult = SetGestureConfig(hWnd, 0, uiGcs, gc, sizeof(GESTURECONFIG));
    if (!bResult) {
        DWORD lastError = GetLastError();
        LOG(SEVERITY_LEVEL_ERROR) << L"SetGestureConfig failed with error: " << lastError;
    }
}

LRESULT CALLBACK gameOfLife3D::CanvasPanel::WndProc( HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam )
{
#ifdef DEBUG_CANVASPANEL_WINMSG
    LOG_ENTER(SEVERITY_LEVEL_DEBUG);
    //LOG(SEVERITY_LEVEL_DEBUG) << util::WMessage(util::WMessageArgs(hWnd, message, wParam, lParam));
#endif

    LRESULT result = 0;

    if (message == WM_CREATE) {
        LPCREATESTRUCT pcs = (LPCREATESTRUCT) lParam;
        CanvasPanel *pCanvasPanel = (CanvasPanel *) pcs->lpCreateParams;

        ::SetWindowLongPtrW(
            hWnd,
            GWLP_USERDATA,
            reinterpret_cast<LONG_PTR>(pCanvasPanel)
        );

        pCanvasPanel->OnCreate();
        result = 1;
    } else {
        touch::ManipulationResult manipulationResult;
        CanvasPanel *pCanvasPanel = reinterpret_cast<CanvasPanel *>(static_cast<LONG_PTR>(
                                        ::GetWindowLongPtrW(
                                            hWnd,
                                            GWLP_USERDATA
                                        )));
        if (pCanvasPanel) {
            if (message == WM_LBUTTONDOWN && GetFocus() != pCanvasPanel->GetHWnd()) {
                // if mouse button down and the canvas panel has no focus, set focus to the canvas
                ::SetFocus(pCanvasPanel->GetHWnd());
            }
            manipulationResult = pCanvasPanel->m_manipulationHelper->WndProc(hWnd, message, wParam, lParam);
            switch (message) {
            case WM_ACTIVATE:
                if (WA_INACTIVE == LOWORD(wParam)) {
                    // do nothing
                    break;
                }
                // do SetFocus
            case WM_SETFOCUS:
                ::SetFocus(pCanvasPanel->m_hwnd);
                break;
            case WM_SIZE: {
                UINT width = LOWORD(lParam);
                UINT height = HIWORD(lParam);
                pCanvasPanel->OnResize(width, height);
            }
            manipulationResult.wasHandled = true;
            break;

            case WM_DISPLAYCHANGE:
            case WM_PAINT:
                PAINTSTRUCT ps;
                BeginPaint(hWnd, &ps);
                pCanvasPanel->OnRender();
                EndPaint(hWnd, &ps);
                manipulationResult.wasHandled = true;
                break;
            case WM_CONTEXTMENU: {
//                 POINT point;
//                 point.x = GET_X_LPARAM(lParam);
//                 point.y = GET_Y_LPARAM(lParam);
//                 pCanvasPanel->GetTouchMindApp()->OnContextMenu(point, hWnd, nullptr);
            }
            manipulationResult.wasHandled = true;
            break;

            case WM_MOUSEWHEEL: {
                float zDelta = -static_cast<float>(GET_WHEEL_DELTA_WPARAM(wParam)) / WHEEL_DELTA / 10.0f;
                pCanvasPanel->UpdateEyeRadius(zDelta);
            }
            manipulationResult.wasHandled = true;
            break;

            case WM_DESTROY:
                PostQuitMessage(0);
                result = 1;
                manipulationResult.wasHandled = true;
                break;
            }
        }

        if (!manipulationResult.wasHandled) {
            result = DefWindowProc(hWnd, message, wParam, lParam);
        }
        if (manipulationResult.requireInvalidateRect) {
            pCanvasPanel->Invalidate();
        }
    }

#ifdef DEBUG_CANVASPANEL_WINMSG
    LOG_LEAVE(SEVERITY_LEVEL_DEBUG);
#endif
    return result;
}

HRESULT gameOfLife3D::CanvasPanel::ResizeByRibbonResized( UINT ribbonHeight )
{
#ifdef DEBUG_CANVASPANEL
    LOG_ENTER(SEVERITY_LEVEL_DEBUG);
#endif

    m_ribbonHeight = ribbonHeight;
    HWND parentHwnd = m_mainWnd.lock()->GetHWnd();
    RECT rc;
    GetClientRect(parentHwnd, &rc);
    rc.top = ribbonHeight;
    rc.bottom -= ribbonHeight;
    HRESULT hr = MoveWindow(m_hwnd, rc.left, rc.top, rc.right, rc.bottom, TRUE) == 0 ? S_OK : E_FAIL;

    m_uiRoot->LayoutComponents();

#ifdef DEBUG_CANVASPANEL
    LOG_LEAVE(SEVERITY_LEVEL_DEBUG);
#endif
    return hr;
}

HRESULT gameOfLife3D::CanvasPanel::CreateDeviceIndependentResources(
    graphics::D3DInteropHelper *pD3DInteropHelper,
    ID2D1Factory *pD2DFactory,
    IWICImagingFactory *pWICFactory,
    IDWriteFactory *pDWriteFactory )
{
    UNREFERENCED_PARAMETER(pD3DInteropHelper);
    UNREFERENCED_PARAMETER(pD2DFactory);
    UNREFERENCED_PARAMETER(pWICFactory);
    UNREFERENCED_PARAMETER(pDWriteFactory);

    return S_OK;
}

HRESULT gameOfLife3D::CanvasPanel::CreateD2DResources(
    graphics::D3DInteropHelper *pD3DInteropHelper,
    ID2D1Factory *pD2DFactory,
    ID2D1RenderTarget *pRenderTarget )
{
    UNREFERENCED_PARAMETER(pD3DInteropHelper);
    UNREFERENCED_PARAMETER(pD2DFactory);
    m_uiRoot->LayoutComponents();
    m_uiRoot->CreateDeviceDependentResources(pD3DInteropHelper, pRenderTarget);

    CHK_FATAL_HRESULT(pD3DInteropHelper->GetDWriteFactory()->CreateTextFormat(L"Arial",
                      nullptr,
                      DWRITE_FONT_WEIGHT_REGULAR,
                      DWRITE_FONT_STYLE_NORMAL,
                      DWRITE_FONT_STRETCH_NORMAL,
                      16,
                      L"",
                      &m_pFpsTextFormat));

    CHK_FATAL_HRESULT(pD3DInteropHelper->GetDWriteFactory()->CreateTextLayout(
                          L"",
                          0,
                          m_pFpsTextFormat,
                          FLT_MAX,
                          FLT_MAX,
                          &m_pFpsTextLayout));

    CHK_FATAL_HRESULT(pRenderTarget->CreateSolidColorBrush(
                          D2D1::ColorF(D2D1::ColorF::White),
                          D2D1::BrushProperties(),
                          &m_pFpsTextBrush));

    return S_OK;
}

void gameOfLife3D::CanvasPanel::DiscardDeviceResources()
{
    m_uiRoot->DiscardDeviceDependentResources();
}

HRESULT gameOfLife3D::CanvasPanel::Draw2D(
    graphics::D3DInteropHelper *pD3DInteropHelper,
    ID2D1RenderTarget *pRenderTarget)
{
    UNREFERENCED_PARAMETER(pD3DInteropHelper);
#ifdef DEBUG_CANVASPANEL
    LOG_ENTER(SEVERITY_LEVEL_DEBUG);
#endif

    if (m_fpsEnabled) {
        std::wstring fps;
        bool updated;
        m_fpsUtil->UpdateFPS(fps, updated);
        LOG(SEVERITY_LEVEL_INFO) << L"fps = " << fps;
        if (updated) {
            m_pFpsTextLayout = nullptr;
            CHK_FATAL_HRESULT(pD3DInteropHelper->GetDWriteFactory()->CreateTextLayout(
                                  fps.c_str(),
                                  static_cast<UINT32>(fps.length()),
                                  m_pFpsTextFormat,
                                  FLT_MAX,
                                  FLT_MAX,
                                  &m_pFpsTextLayout));
        }
    }

    HRESULT hr = S_OK;
    pRenderTarget->BeginDraw();
    pRenderTarget->Clear(D2D1::ColorF(0, 0.0f));
    m_uiRoot->Render(pD3DInteropHelper, pRenderTarget);
    if (m_fpsEnabled) {
        pRenderTarget->DrawTextLayout(D2D1::Point2F(8.0f, 8.0f), m_pFpsTextLayout, m_pFpsTextBrush);
    }
    hr = pRenderTarget->EndDraw();

    if (hr == D2DERR_RECREATE_TARGET) {
        hr = S_OK;
        DiscardDeviceResources();
    }

#ifdef DEBUG_CANVASPANEL
    LOG_LEAVE(SEVERITY_LEVEL_DEBUG);
#endif
    return hr;
}

HRESULT gameOfLife3D::CanvasPanel::Invalidate()
{
    BOOL bResult = InvalidateRect(m_hwnd, nullptr, TRUE);
    HRESULT hr = bResult ? S_OK : E_FAIL;
    return hr;
}

void gameOfLife3D::CanvasPanel::OnResize( UINT width, UINT height )
{
#ifdef DEBUG_CANVASPANEL
    LOG_ENTER(SEVERITY_LEVEL_DEBUG) << L"width = " << width << L", height = " << height;
#endif
    m_pD3DInteropHelper->OnResize(width, height);
    m_uiRoot->OnResize(ui::UISize(static_cast<FLOAT>(width), static_cast<FLOAT>(height)));
#ifdef DEBUG_CANVASPANEL
    LOG_LEAVE(SEVERITY_LEVEL_DEBUG);
#endif
}

HRESULT gameOfLife3D::CanvasPanel::OnRender()
{
#ifdef DEBUG_CANVASPANEL
    LOG_ENTER(SEVERITY_LEVEL_DEBUG);
#endif

    HRESULT hr = S_OK;
    UI_ANIMATION_SECONDS secondsNow;
    hr = m_pD3DInteropHelper->GetAnimationTimer()->GetTime(&secondsNow);
    if (SUCCEEDED(hr)) {
        hr = m_pD3DInteropHelper->GetAnimationManager()->Update(secondsNow);
    }
    if (SUCCEEDED(hr)) {
        hr = m_pD3DInteropHelper->OnRender();
    }
    UI_ANIMATION_MANAGER_STATUS status;
    hr = m_pD3DInteropHelper->GetAnimationManager()->GetStatus(&status);
//     if (SUCCEEDED(hr) && status == UI_ANIMATION_MANAGER_BUSY) {
    Invalidate();
//     }


#ifdef DEBUG_CANVASPANEL
    LOG_LEAVE(SEVERITY_LEVEL_DEBUG);
#endif
    return hr;
}

HRESULT gameOfLife3D::CanvasPanel::ResizeByParentResized()
{
    HRESULT hr = ResizeByRibbonResized(m_ribbonHeight);
    return hr;
}

// *** Direct 3D ***

HRESULT gameOfLife3D::CanvasPanel::CreateD3DResources(
    graphics::D3DInteropHelper *pD3DInteropHelper,
    ID3D11Device *pDevice,
    ID3D11DeviceContext *pImmediateContext )
{
#ifdef DEBUG_CANVASPANEL
    LOG_ENTER(SEVERITY_LEVEL_DEBUG);
#endif
    InitializeLifeSimulator(pD3DInteropHelper);
    CHK_FATAL_HRESULT(m_pDraw3D->CreateD3DResources(pD3DInteropHelper, pDevice, pImmediateContext));
    // Create the constant buffers
    D3D11_BUFFER_DESC cbufDesc;
    ZeroMemory( &cbufDesc, sizeof(cbufDesc) );
    cbufDesc.Usage = D3D11_USAGE_DEFAULT;
    cbufDesc.ByteWidth = sizeof(ConstantBuffer);
    cbufDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
    cbufDesc.CPUAccessFlags = 0;
    CHK_FATAL_HRESULT(pDevice->CreateBuffer(&cbufDesc, nullptr, &m_pConstantBuffer));

#ifdef DEBUG_CANVASPANEL
    LOG_LEAVE(SEVERITY_LEVEL_DEBUG);
#endif
    return S_OK;
}

HRESULT gameOfLife3D::CanvasPanel::RecreateSizedResources(
    UINT nWidth,
    UINT nHeight,
    graphics::D3DInteropHelper *pD3DInteropHelper,
    ID3D11Device *pDevice,
    ID3D11DeviceContext *pImmediateContext )
{
    UNREFERENCED_PARAMETER(pD3DInteropHelper);
    UNREFERENCED_PARAMETER(pDevice);
    UNREFERENCED_PARAMETER(pImmediateContext);
#ifdef DEBUG_CANVASPANEL
    LOG_ENTER(SEVERITY_LEVEL_DEBUG);
#endif
    XMVECTOR eye = XMLoadFloat4(&m_eyeInfo.eye);
    XMVECTOR at = XMLoadFloat4(&m_eyeInfo.at);
    XMVECTOR up = XMLoadFloat4(&m_eyeInfo.up);
    XMMATRIX view = XMMatrixLookAtLH(eye, at, up);
    XMMATRIX projection = XMMatrixPerspectiveFovLH( XM_PIDIV4, nWidth / (FLOAT)nHeight, 0.01f, 100.0f );
    XMStoreFloat4x4(&m_view, view);
    XMStoreFloat4x4(&m_projection, projection);

    CHK_FATAL_HRESULT(_UpdateConstantBuffer(pD3DInteropHelper, pDevice, pImmediateContext));
#ifdef DEBUG_CANVASPANEL
    LOG_LEAVE(SEVERITY_LEVEL_DEBUG);
#endif
    return S_OK;
}

HRESULT gameOfLife3D::CanvasPanel::_UpdateConstantBuffer(
    graphics::D3DInteropHelper *pD3DInteropHelper,
    ID3D11Device *pDevice,
    ID3D11DeviceContext *pImmediateContext )
{
    UNREFERENCED_PARAMETER(pD3DInteropHelper);
    UNREFERENCED_PARAMETER(pDevice);
    UNREFERENCED_PARAMETER(pImmediateContext);
#ifdef DEBUG_CANVASPANEL
    LOG_ENTER(SEVERITY_LEVEL_DEBUG);
#endif
    ConstantBuffer cb;
    XMMATRIX world = XMLoadFloat4x4(&m_world);
    XMMATRIX view = XMLoadFloat4x4(&m_view);
    XMMATRIX projection = XMLoadFloat4x4(&m_projection);
    cb.mWorld = XMMatrixTranspose(world);
    cb.mView = XMMatrixTranspose(view);
    cb.mProjection = XMMatrixTranspose(projection);
    for (size_t i = 0; i < NUMBER_OF_LIGHTS; ++i) {
        cb.vLightDir[i] = m_lightInfo[i].vLightDir;
        cb.vLightColor[i] = m_lightInfo[i].vLightColor;
    }
    pImmediateContext->UpdateSubresource(m_pConstantBuffer, 0, nullptr, &cb, 0, 0);
#ifdef DEBUG_CANVASPANEL
    LOG_LEAVE(SEVERITY_LEVEL_DEBUG);
#endif
    return S_OK;
}

HRESULT gameOfLife3D::CanvasPanel::Draw3D(
    graphics::D3DInteropHelper *pD3DInteropHelper,
    ID3D11Device *pDevice,
    ID3D11DeviceContext *pImmediateContext )
{
    UNREFERENCED_PARAMETER(pD3DInteropHelper);
    UNREFERENCED_PARAMETER(pDevice);
    UNREFERENCED_PARAMETER(pImmediateContext);
#ifdef DEBUG_CANVASPANEL
    LOG_ENTER(SEVERITY_LEVEL_DEBUG);
#endif

    if (m_parametersModified) {
        InitializeLifeSimulator(pD3DInteropHelper);
        CHK_FATAL_HRESULT(m_pDraw3D->CreateD3DResources(pD3DInteropHelper, pDevice, pImmediateContext));
        m_parametersModified = false;
    }

    static float t = 0.0f;
    static int dwTimeStart = 0;
    static int dwTimePrevious = -9999;
    int dwTimeCur = static_cast<int>(GetTickCount());
    if ( dwTimeStart == 0 ) {
        dwTimeStart = dwTimeCur;
    }
    t = ( dwTimeCur - dwTimeStart ) / 1000.0f;
    if (m_running && dwTimeCur > dwTimePrevious + static_cast<int>(m_updateInterval)) {
        MoveToNextGeneration(pD3DInteropHelper);
        dwTimePrevious = dwTimeCur;
    }

    CHK_FATAL_HRESULT(_UpdateConstantBuffer(pD3DInteropHelper, pDevice, pImmediateContext));
    /*
    UINT stride = sizeof( gameOfLife3D::draw3d::VertexInfo );
    UINT offset = 0;
    pImmediateContext->IASetPrimitiveTopology( D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST );
    pImmediateContext->IASetInputLayout( m_pVertexLayout );
    m_pVertexGenerator->Update(pD3DInteropHelper);
    ID3D11Buffer *pVertexBuffer = m_pVertexGenerator->GetVertexBuffer();
    pImmediateContext->IASetVertexBuffers( 0, 1, &pVertexBuffer, &stride, &offset );
    ID3D11Buffer *pIndexBuffer = m_pVertexGenerator->GetIndexBuffer();
    pImmediateContext->IASetIndexBuffer( pIndexBuffer,
                                         sizeof(gameOfLife3D::draw3d::VERTEX_INDEX) == 2 ? DXGI_FORMAT_R16_UINT : DXGI_FORMAT_R32_UINT, 0 );
    pImmediateContext->VSSetShader( m_pVertexShader, nullptr, 0 );
    */
    pImmediateContext->VSSetConstantBuffers( 0, 1, &m_pConstantBuffer );
//     pImmediateContext->PSSetShader( m_pPixelShader, nullptr, 0 );
    pImmediateContext->GSSetConstantBuffers( 0, 1, &m_pConstantBuffer );
    pImmediateContext->PSSetConstantBuffers( 0, 1, &m_pConstantBuffer );
//     pImmediateContext->DrawIndexed( m_pVertexGenerator->GetNumberOfIndexes(), 0, 0 );
    m_pDraw3D->Draw3D(pD3DInteropHelper, pDevice, pImmediateContext);

#ifdef DEBUG_CANVASPANEL
    LOG_LEAVE(SEVERITY_LEVEL_DEBUG);
#endif
    return S_OK;
}

void gameOfLife3D::CanvasPanel::MoveToNextGeneration(graphics::D3DInteropHelper *pD3DInteropHelper)
{
    m_pDraw3D->MoveNextGeneration(pD3DInteropHelper);
}

void gameOfLife3D::CanvasPanel::NotifyHorizontalScrollbarChanged( float angle )
{
    m_eyePosInfo.phi = (angle / 360.0f) * XM_PI * 2.0f;
    UpdateEyePos();
}

void gameOfLife3D::CanvasPanel::NotifyVerticalScrollbarChanged( float angle )
{
    m_eyePosInfo.theta = (angle / 360.0f) * XM_PI * 2.0f;
    UpdateEyePos();
}

void gameOfLife3D::CanvasPanel::UpdateEyePos()
{
    m_eyeInfo.eye.x =  m_eyePosInfo.radius * sinf(m_eyePosInfo.phi) * sinf(m_eyePosInfo.theta);
    m_eyeInfo.eye.y =  m_eyePosInfo.radius * sinf(m_eyePosInfo.phi) * cosf(m_eyePosInfo.theta);
    m_eyeInfo.eye.z = -m_eyePosInfo.radius * cosf(m_eyePosInfo.phi);

    XMVECTOR eye = XMLoadFloat4(&m_eyeInfo.eye);
    XMVECTOR at = XMLoadFloat4(&m_eyeInfo.at);
    XMVECTOR up = XMLoadFloat4(&m_eyeInfo.up);
    XMMATRIX view = XMMatrixLookAtLH(eye, at, up);
    XMStoreFloat4x4(&m_view, view);
}

void gameOfLife3D::CanvasPanel::OnStart()
{
    m_running = true;
}

void gameOfLife3D::CanvasPanel::OnStop()
{
    m_running = false;
}

void gameOfLife3D::CanvasPanel::OnSettings()
{
    gameOfLife3D::dialogs::SettingData settingData;
    settingData.width = m_lifeAreaWidth;
    settingData.height = m_lifeAreaHeight;
    settingData.generations = m_numberOfGenerations;
    settingData.updateInterval = m_updateInterval;
    settingData.vertexGenerationMethod = m_vertexGenerationMethod;
    m_settingsDialog->SetData(settingData);
    m_uiRoot->ShowDialog(L"settingsDialog");
}

void gameOfLife3D::CanvasPanel::InitializeLifeSimulator(graphics::D3DInteropHelper *pD3DInteropHelper)
{
    m_pLifeSimulator = nullptr;
    m_pLifeSimulator = gameOfLife3D::lifeSimulator::LifeSimulatorFactory::Create(
                           pD3DInteropHelper->IsComputeShaderSupported() ?
                           gameOfLife3D::lifeSimulator::SIMULATOR_TYPE_COMPUTESHADER :
                           gameOfLife3D::lifeSimulator::SIMULATOR_TYPE_CPU);
    m_pLifeSimulator->SetWidth(m_lifeAreaWidth);
    m_pLifeSimulator->SetHeight(m_lifeAreaHeight);
    m_pLifeSimulator->SetInitialData(m_initialLifeData);
    m_pLifeSimulator->Initialize(pD3DInteropHelper);
    if (m_csFileName.length() > 0) {
        std::wstring errorMessage;
        m_pLifeSimulator->LoadRuleFile(pD3DInteropHelper, m_csFileName, errorMessage);
    }
    m_pDraw3D = nullptr;
    m_pDraw3D = gameOfLife3D::draw3d::Draw3DFactory::Create(m_vertexGenerationMethod ==
                gameOfLife3D::dialogs::VERTEX_GENERATION_METHOD_CPU ?
                gameOfLife3D::draw3d::DRAW3D_TYPE_CPU :
                gameOfLife3D::draw3d::DRAW3D_TYPE_GEOMETRYSHADER);
    m_pDraw3D->Initialize(pD3DInteropHelper, m_pLifeSimulator, m_numberOfGenerations, m_marginRate);
}

void gameOfLife3D::CanvasPanel::UpdateEyeRadius( float delta )
{
    m_eyePosInfo.radius += delta;
    if (m_eyePosInfo.radius < 1.2f) {
        m_eyePosInfo.radius = 1.2f;
    }
    if (m_eyePosInfo.radius > 5.0f) {
        m_eyePosInfo.radius = 5.0f;
    }
    UpdateEyePos();
}

void gameOfLife3D::CanvasPanel::SetLifeFile(const std::shared_ptr<gameOfLife3D::io::LifeFile> &pLifeFile)
{
    m_lifeAreaWidth = std::max(m_lifeAreaWidth, pLifeFile->GetWidth());
    m_lifeAreaHeight = std::max(m_lifeAreaHeight, pLifeFile->GetHeight());

    m_initialLifeData.resize(m_lifeAreaWidth * m_lifeAreaHeight);
    std::for_each(m_initialLifeData.begin(), m_initialLifeData.end(),
    [](gameOfLife3D::lifeSimulator::LifeData & lifeData) {
        lifeData.alive = 0;
        lifeData.color = XMFLOAT3(0.0f, 0.0f, 0.0f);
    });

    UINT offsetX = (m_lifeAreaWidth - pLifeFile->GetWidth()) / 2;
    UINT offsetY = (m_lifeAreaHeight - pLifeFile->GetHeight()) / 2;
    for (UINT y = 0; y < pLifeFile->GetHeight(); ++y) {
        for (UINT x = 0; x < pLifeFile->GetWidth(); ++x) {
            UINT index = (x + offsetX) + (y + offsetY) * m_lifeAreaWidth;
            m_initialLifeData[index] = pLifeFile->GetLifeData(x, y);
        }
    }

    m_parametersModified = true;
}

void gameOfLife3D::CanvasPanel::GetLifeFile( std::shared_ptr<gameOfLife3D::io::LifeFile> &pLifeFile )
{
    UNREFERENCED_PARAMETER(pLifeFile);
}

void gameOfLife3D::CanvasPanel::RandomizeInitialData()
{
    m_initialLifeData.resize(m_lifeAreaWidth * m_lifeAreaHeight);
    for (size_t i = 0; i < m_initialLifeData.size(); ++i) {
        if (rand() % 4 == 0) {
            m_initialLifeData[i].alive = 1;
            m_initialLifeData[i].color = XMFLOAT3(1.0f, 1.0f, 1.0f);
        } else {
            m_initialLifeData[i].alive = 0;
            m_initialLifeData[i].color = XMFLOAT3(0.0f, 0.0f, 0.0f);
        }
    }
    m_parametersModified = true;
}

std::wstring g_errorMessage;
INT_PTR CALLBACK DialogProc_CSERROR_DIALOG(
    __in  HWND hwndDlg,
    __in  UINT uMsg,
    __in  WPARAM wParam,
    __in  LPARAM lParam
)
{
    UNREFERENCED_PARAMETER(lParam);
    switch (uMsg) {
    case WM_INITDIALOG:
        BringWindowToTop(hwndDlg);
        SetDlgItemText(hwndDlg, IDC_CSERROR_EDIT, g_errorMessage.c_str());
        return TRUE;

    case WM_COMMAND:
        switch (wParam) {
        case IDOK:
            EndDialog(hwndDlg, 0);
            return TRUE;
        }
        break;
    }

    return FALSE;
}

INT_PTR CALLBACK DialogProc_CSNOTSUPPORT_DIALOG(
    __in  HWND hwndDlg,
    __in  UINT uMsg,
    __in  WPARAM wParam,
    __in  LPARAM lParam
)
{
    UNREFERENCED_PARAMETER(lParam);
    switch (uMsg) {
    case WM_INITDIALOG:
        BringWindowToTop(hwndDlg);
        return TRUE;

    case WM_COMMAND:
        switch (wParam) {
        case IDOK:
            EndDialog(hwndDlg, 0);
            return TRUE;
        }
        break;
    }

    return FALSE;
}

void gameOfLife3D::CanvasPanel::OpenCSFile( const std::wstring &fileName )
{
    m_csFileName = fileName;
    g_errorMessage = L"";
    HRESULT hr = m_pLifeSimulator->LoadRuleFile(m_pD3DInteropHelper.get(), fileName, g_errorMessage);
    if (FAILED(hr)) {
        if (hr == E_NOTIMPL) {
            DialogBox(
                mui::MUI::GetHModule(),
                MAKEINTRESOURCE(IDD_CSNOTSUPPORT_DIALOG),
                m_hwnd,
                DialogProc_CSNOTSUPPORT_DIALOG);
        } else {
            DialogBox(
                mui::MUI::GetHModule(),
                MAKEINTRESOURCE(IDD_CSERROR_DIALOG),
                m_hwnd,
                DialogProc_CSERROR_DIALOG);
        }
    }
}

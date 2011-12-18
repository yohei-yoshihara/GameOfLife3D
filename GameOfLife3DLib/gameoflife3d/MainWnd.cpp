#include "StdAfx.h"
#include "Common.h"
#include "util/Utils.h"
#include "mui/MUI.h"
#include "tsf/TSFManager.h"
#include "gameOfLife3D/Ribbon.h"
#include "gameOfLife3D/RibbonFactory.h"
#include "gameOfLife3D/CanvasPanel.h"
#include "gameOfLife3D/MainWnd.h"
#include "gameOfLife3D/io/LifeFile.h"

// #define DEBUG_MAINWND

gameOfLife3D::MainWnd::MainWnd(void) :
    m_hwnd(0),
    m_tsfManager(std::make_shared<tsf::TSFManager>()),
    m_ribbonFactory(std::make_shared<gameOfLife3D::RibbonFactory>()),
    m_canvasPanel(std::make_shared<gameOfLife3D::CanvasPanel>())
{
}

gameOfLife3D::MainWnd::~MainWnd(void)
{
}

HRESULT gameOfLife3D::MainWnd::Initialize()
{
#ifdef DEBUG_MAINWND
    LOG_ENTER(SEVERITY_LEVEL_DEBUG);
#endif

    HICON hIcon = LoadIcon(HINST_THISCOMPONENT, MAKEINTRESOURCE(IDI_APP));
    if (hIcon == nullptr) {
        DWORD lastError = GetLastError();
        LOG(SEVERITY_LEVEL_ERROR) << util::LastError(util::LastErrorArgs(L"LoadIcon", lastError));
        return E_FAIL;
    }

    HACCEL hAccel = LoadAccelerators(HINST_THISCOMPONENT, MAKEINTRESOURCE(IDR_ACCELERATOR_DEFAULT));
    if (hAccel == nullptr) {
        DWORD lastError = GetLastError();
        LOG(SEVERITY_LEVEL_ERROR) << util::LastError(util::LastErrorArgs(L"LoadAccelerators", lastError));
        return E_FAIL;
    }

    WNDCLASSEX wcex = { sizeof(WNDCLASSEX) };
    wcex.style = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc = MainWnd::WndProc;
    wcex.cbClsExtra = 0;
    wcex.cbWndExtra = sizeof(LONG_PTR);
    wcex.hInstance = HINST_THISCOMPONENT;
    wcex.hbrBackground = nullptr;
    wcex.lpszMenuName = nullptr;
    wcex.hIcon = hIcon;
    wcex.hCursor = LoadCursor(nullptr, IDC_ARROW);
    wcex.lpszClassName = L"MainWnd";

    RegisterClassEx(&wcex);

    HDC hdc = GetDC(nullptr);
    int dpiX = GetDeviceCaps(hdc, LOGPIXELSX);
    int dpiY = GetDeviceCaps(hdc, LOGPIXELSY);
    ReleaseDC(nullptr, hdc);

    m_hwnd = CreateWindow(
                 L"MainWnd",
                 mui::MUI::GetString(IDS_APP_NAME),
                 WS_OVERLAPPEDWINDOW | WS_CLIPCHILDREN,
                 CW_USEDEFAULT,
                 CW_USEDEFAULT,
                 static_cast<UINT>(std::ceil(1024.f * dpiX / 96.f)),
                 static_cast<UINT>(std::ceil(768.f * dpiY / 96.f)),
                 nullptr,
                 nullptr,
                 HINST_THISCOMPONENT,
                 this);
    HRESULT hr = m_hwnd ? S_OK : E_FAIL;
    if (FAILED(hr)) {
        DWORD lastError = GetLastError();
        LOG(SEVERITY_LEVEL_ERROR) << util::LastError(util::LastErrorArgs(L"CreateWindow", lastError));
    } else {
        m_tsfManager->SetHAccel(hAccel);
        m_tsfManager->SetHWnd(m_hwnd);
        m_tsfManager->Initialize();
        m_ribbonFactory->Initialize(shared_from_this());
        ShowWindow(m_hwnd, SW_SHOWNORMAL);
        UpdateWindow(m_hwnd);
    }

#ifdef DEBUG_MAINWND
    LOG_LEAVE(SEVERITY_LEVEL_DEBUG);
#endif
    return hr;
}

void gameOfLife3D::MainWnd::RunMessageLoop()
{
    m_tsfManager->RunMessageLoop();
}

HRESULT gameOfLife3D::MainWnd::OnCreate( HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam )
{
    UNREFERENCED_PARAMETER(hwnd);
    UNREFERENCED_PARAMETER(message);
    UNREFERENCED_PARAMETER(wParam);
    UNREFERENCED_PARAMETER(lParam);
#ifdef DEBUG_MAINWND
    LOG_ENTER(SEVERITY_LEVEL_DEBUG);
#endif

    HRESULT hr = m_canvasPanel->Initialize(shared_from_this());

#ifdef DEBUG_MAINWND
    LOG_LEAVE(SEVERITY_LEVEL_DEBUG);
#endif
    return hr;
}

LRESULT CALLBACK gameOfLife3D::MainWnd::WndProc( HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam )
{
#ifdef DEBUG_MAINWND
    LOG_ENTER(SEVERITY_LEVEL_DEBUG);
    LOG(SEVERITY_LEVEL_DEBUG) << util::WMessage(util::WMessageArgs(hWnd, message, wParam, lParam));
#endif

    LRESULT result = 0;
    if (message == WM_CREATE) {
        LPCREATESTRUCT pcs = (LPCREATESTRUCT) lParam;
        MainWnd *pMainWnd = (MainWnd*) pcs->lpCreateParams;
        SetWindowLongPtr(hWnd, GWLP_USERDATA, PtrToUlong(pMainWnd));
        result = 1;
    } else {
        MainWnd *pMainWnd = reinterpret_cast<MainWnd*>(
                                static_cast<LONG_PTR>(GetWindowLongPtr(hWnd, GWLP_USERDATA)));
        bool wasHandled = false;

        if (pMainWnd) {
            switch (message) {
            case WM_COMMAND: {
                // Accelerator
                switch (LOWORD(wParam)) {
                case ID_ACCELERATOR_FPS:
                    pMainWnd->SetFPSEnable(!pMainWnd->IsFPSEnabled());
                }
            }
            result = 0;
            wasHandled = true;
            break;
            case WM_SIZE:
                if (pMainWnd->m_canvasPanel.get() != nullptr
                        && pMainWnd->m_canvasPanel->IsInitialized()) {
                    pMainWnd->m_canvasPanel->ResizeByParentResized();
                }
                break;
            case WM_NCCREATE:
                result = 1;
                wasHandled = true;
                break;
            case WM_ACTIVATE:
                if (WA_INACTIVE == LOWORD(wParam)) {
                    // do nothing
                    break;
                }
                // do SetFocus
            case WM_SETFOCUS:
                ::SetFocus(pMainWnd->m_canvasPanel->GetHWnd());
                result = 1;
                wasHandled = true;
                break;
            case WM_RIBBON_RESIZED:
                if (pMainWnd->m_canvasPanel.get() != nullptr
                        && !pMainWnd->m_canvasPanel->IsInitialized()) {
                    pMainWnd->OnCreate(hWnd, message, wParam, lParam);
                } else if (pMainWnd->m_canvasPanel->IsInitialized()) {
                    pMainWnd->m_canvasPanel->ResizeByRibbonResized(static_cast < UINT >(wParam));
                }
                result = 0;
                wasHandled = true;
                break;
            case WM_DESTROY:
                PostQuitMessage(0);
                result = 1;
                wasHandled = true;
                break;
            }
        }
        if (!wasHandled) {
            result = DefWindowProc(hWnd, message, wParam, lParam);
        }
    }

#ifdef DEBUG_MAINWND
    LOG_LEAVE(SEVERITY_LEVEL_DEBUG);
#endif
    return result;
}

UINT gameOfLife3D::MainWnd::GetRibbonHeight() const
{
    return m_ribbonFactory->GetApplication()->GetRibbonHeight();
}

void gameOfLife3D::MainWnd::OnStart()
{
    m_canvasPanel->OnStart();
}

void gameOfLife3D::MainWnd::OnStop()
{
    m_canvasPanel->OnStop();
}

void gameOfLife3D::MainWnd::OnSettings()
{
    m_canvasPanel->OnSettings();
}

void gameOfLife3D::MainWnd::OnExit()
{
    ::PostMessage(m_hwnd, WM_CLOSE, 0, 0);
}

INT_PTR CALLBACK DialogProc(
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

void gameOfLife3D::MainWnd::OnVersionInfo()
{
    DialogBox(mui::MUI::GetHModule(), MAKEINTRESOURCE(IDD_VERSION_DIALOG), m_hwnd, DialogProc);
}

void gameOfLife3D::MainWnd::OnOpenFile()
{
    static COMDLG_FILTERSPEC dialogFiltersMM[] = {
        { L"Life File(*.lif;*.life)", L"*.lif;*.life" },
        { L"All types(*.*)", L"*.*" },
    };

    IFileOpenDialog *pFileOpenDialog = nullptr;

    HRESULT hr = CoCreateInstance(CLSID_FileOpenDialog,
                                  nullptr,
                                  CLSCTX_INPROC_SERVER,
                                  IID_PPV_ARGS(&pFileOpenDialog));

    if (SUCCEEDED(hr)) {
        hr = pFileOpenDialog->SetFileTypes(2, dialogFiltersMM);
    }
    IShellItem *pResult = nullptr;
    if (SUCCEEDED(hr)) {
        hr = pFileOpenDialog->Show(m_hwnd);
    }
    if (SUCCEEDED(hr)) {
        hr = pFileOpenDialog->GetResult(&pResult);
    }
    WCHAR *pPath = nullptr;
    if (SUCCEEDED(hr)) {
        hr = pResult->GetDisplayName(SIGDN_FILESYSPATH, &pPath);
    }
    if (SUCCEEDED(hr)) {
        std::wstring fileName(pPath);
        std::shared_ptr<gameOfLife3D::io::LifeFile> lifeFile =
            std::make_shared<gameOfLife3D::io::LifeFile>();
        lifeFile->Load(fileName);
        m_canvasPanel->SetLifeFile(lifeFile);
    }
    if (pPath != nullptr) {
        CoTaskMemFree(pPath);
    }
    SafeRelease(&pResult);
    SafeRelease(&pFileOpenDialog);
}

const int NUMBER_OF_DIALOG_FILTERS = 2;
COMDLG_FILTERSPEC DIALOG_FILTERS[] = {
    { L"GameOfLife3D(.l3d)", L"*.l3d" },
    { L"Life 1.06(.lif)", L"*.lif" },
};

void gameOfLife3D::MainWnd::OnSaveFileAs()
{
    IFileSaveDialog* pFileSaveDialog = nullptr;
    HRESULT hr = CoCreateInstance(CLSID_FileSaveDialog, nullptr, CLSCTX_INPROC_SERVER, IID_PPV_ARGS(&pFileSaveDialog));
    if (SUCCEEDED(hr)) {
        hr = pFileSaveDialog->SetFileTypes(NUMBER_OF_DIALOG_FILTERS, DIALOG_FILTERS);
    }
    if (SUCCEEDED(hr)) {
        hr = pFileSaveDialog->Show(m_hwnd);
    }
    IShellItem *pResult = nullptr;
    if (SUCCEEDED(hr)) {
        hr = pFileSaveDialog->GetResult(&pResult);
    }
    WCHAR *pPath = nullptr;
    if (SUCCEEDED(hr)) {
        hr = pResult->GetDisplayName(SIGDN_FILESYSPATH, &pPath);
    }
    UINT fileTypeIndex = 0;
    if (SUCCEEDED(hr)) {
        hr = pFileSaveDialog->GetFileTypeIndex(&fileTypeIndex);
    }
    if (SUCCEEDED(hr)) {
        std::shared_ptr<gameOfLife3D::io::LifeFile> lifeFile =
            std::make_shared<gameOfLife3D::io::LifeFile>();
        m_canvasPanel->GetLifeFile(lifeFile);

        std::wstring fileName(pPath);
        if (fileTypeIndex == 0) {
            std::wregex rx(L".*\\" L".l3d");
            bool ret = std::regex_match(fileName.begin(), fileName.end(), rx);
            if (!ret) {
                fileName += L".l3d";
            }
            lifeFile->Save(fileName, gameOfLife3D::io::LIFEFILE_FORMAT_LIFE3D100);
        } else {
            std::wregex rx(L".*\\" L".lif");
            bool ret = std::regex_match(fileName.begin(), fileName.end(), rx);
            if (!ret) {
                fileName += L".lif";
            }
            lifeFile->Save(fileName, gameOfLife3D::io::LIFEFILE_FORMAT_LIFE106);
        }
    }
    if (pPath != nullptr) {
        CoTaskMemFree(pPath);
    }
    SafeRelease(&pResult);
    SafeRelease(&pFileSaveDialog);
}

void gameOfLife3D::MainWnd::OnRandom()
{
    m_canvasPanel->RandomizeInitialData();
}

void gameOfLife3D::MainWnd::OnOpenCSFile()
{
    static COMDLG_FILTERSPEC dialogFiltersMM[] = {
        { L"effect File(*.fx)", L"*.fx" },
        { L"compute shader File(*.cs)", L"*.cs" },
        { L"All types(*.*)", L"*.*" },
    };

    IFileOpenDialog *pFileOpenDialog = nullptr;

    HRESULT hr = CoCreateInstance(CLSID_FileOpenDialog,
                                  nullptr,
                                  CLSCTX_INPROC_SERVER,
                                  IID_PPV_ARGS(&pFileOpenDialog));

    if (SUCCEEDED(hr)) {
        hr = pFileOpenDialog->SetFileTypes(3, dialogFiltersMM);
    }
    IShellItem *pResult = nullptr;
    if (SUCCEEDED(hr)) {
        hr = pFileOpenDialog->Show(m_hwnd);
    }
    if (SUCCEEDED(hr)) {
        hr = pFileOpenDialog->GetResult(&pResult);
    }
    WCHAR *pPath = nullptr;
    if (SUCCEEDED(hr)) {
        hr = pResult->GetDisplayName(SIGDN_FILESYSPATH, &pPath);
    }
    if (SUCCEEDED(hr)) {
        std::wstring fileName(pPath);
        m_canvasPanel->OpenCSFile(fileName);
    }
    if (pPath != nullptr) {
        CoTaskMemFree(pPath);
    }
    SafeRelease(&pResult);
    SafeRelease(&pFileOpenDialog);
}

void gameOfLife3D::MainWnd::SetFPSEnable( bool fpsEnabled )
{
    m_canvasPanel->SetFPSEnable(fpsEnabled);
}

bool gameOfLife3D::MainWnd::IsFPSEnabled() const
{
    return m_canvasPanel->IsFPSEnabled();
}

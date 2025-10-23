#include "StdAfx.h"
#include <Shobjidl.h>
#include "mui/MUI.h"
#include "GameOfLife3D/IMainWnd.h"
#include "GameOfLife3D/Ribbon.h"
#include "GameOfLife3D/RibbonFactory.h"

using namespace mui;

gameOfLife3D::RibbonFactory::RibbonFactory()
    : m_pFramework(nullptr)
    , m_pApplication(nullptr)
    , m_mainWnd() {
}

gameOfLife3D::RibbonFactory::~RibbonFactory() {
}

HRESULT gameOfLife3D::RibbonFactory::Initialize(std::weak_ptr<IMainWnd> mainWnd) {
  m_mainWnd = mainWnd;
  HRESULT hr = CoCreateInstance(CLSID_UIRibbonFramework, nullptr, CLSCTX_INPROC_SERVER, IID_PPV_ARGS(&m_pFramework));
  if (FAILED(hr)) {
    LOG(SEVERITY_LEVEL_ERROR) << L"CoCreateInstance failed, hr = " << hr;
    return hr;
  }

  hr = CComObject<gameOfLife3D::Ribbon>::CreateInstance(&m_pApplication);
  if (FAILED(hr)) {
    LOG(SEVERITY_LEVEL_ERROR) << L"CComObject<Ribbon>::CreateInstance failed, hr = " << hr;
    return hr;
  }

  hr = m_pFramework->Initialize(mainWnd.lock()->GetHWnd(), m_pApplication);
  if (FAILED(hr)) {
    LOG(SEVERITY_LEVEL_ERROR) << L"IUIFramework::Initialize failed, hr = " << hr;
    return hr;
  }

  hr = m_pFramework->LoadUI(MUI::GetHModule(), L"APPLICATION_RIBBON");
  if (FAILED(hr)) {
    LOG(SEVERITY_LEVEL_ERROR) << L"IUIFramework::LoadUI failed, hr = " << hr;
    return hr;
  }

  hr = m_pApplication->Initialize(this);
  if (FAILED(hr)) {
    LOG(SEVERITY_LEVEL_ERROR) << L"Ribbon::Initialize failed, hr = " << hr;
    return hr;
  }

  return S_OK;
}

HWND gameOfLife3D::RibbonFactory::GetParentHWnd() const {
  if (!m_mainWnd.expired()) {
    return m_mainWnd.lock()->GetHWnd();
  } else {
    throw std::runtime_error("main window is expired");
  }
}

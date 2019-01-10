#include "StdAfx.h"
#include <Shobjidl.h>
#include <propvarutil.h>
#include "Common.h"
#include "GameOfLife3D/IMainWnd.h"
#include "GameOfLife3D/RibbonFactory.h"
#include "GameOfLife3D/Ribbon.h"

// #define RIBBON_DEBUG

// convert HSB(360, 100, 100) -> HSB(255, 255, 255)
UI_HSBCOLOR UI_HSB_256(unsigned int hue, unsigned int saturation, unsigned int brightness) {
  return UI_HSB(static_cast<BYTE>(hue / 360.0 * 256.0), static_cast<BYTE>(saturation / 100.0 * 256.0),
                static_cast<BYTE>(brightness / 100.0 * 256.0));
}

gameOfLife3D::Ribbon::Ribbon(void)
    : m_pRibbonFactory(nullptr)
    , m_pUiRibbon(nullptr) {
}

gameOfLife3D::Ribbon::~Ribbon(void) {
}

HRESULT gameOfLife3D::Ribbon::Initialize(RibbonFactory *pRibbonFactory) {
#ifdef RIBBON_DEBUG
  LOG_ENTER(SEVERITY_LEVEL_DEBUG);
#endif
  m_pRibbonFactory = pRibbonFactory;

  //CComPtr<IPropertyStore> spPropertyStore;

  //if (SUCCEEDED(pRibbonFactory->GetFramework()->QueryInterface(&spPropertyStore))) {
  //  PROPVARIANT propvarBackground;
  //  PROPVARIANT propvarHighlight;
  //  PROPVARIANT propvarText;
  //
  //  // UI_HSBCOLOR BackgroundColor = UI_HSB_256(210, 20, 20);
  //  UI_HSBCOLOR BackgroundColor = UI_HSB_256(35, 0, 30);
  //  UI_HSBCOLOR HighlightColor = UI_HSB_256(360, 100, 100);
  //  UI_HSBCOLOR TextColor = UI_HSB_256(360, 0, 100);

  //  InitPropVariantFromUInt32(BackgroundColor, &propvarBackground);
  //  InitPropVariantFromUInt32(HighlightColor, &propvarHighlight);
  //  InitPropVariantFromUInt32(TextColor, &propvarText);

  //  spPropertyStore->SetValue(UI_PKEY_GlobalBackgroundColor, propvarBackground);
  //  spPropertyStore->SetValue(UI_PKEY_GlobalHighlightColor, propvarHighlight);
  //  spPropertyStore->SetValue(UI_PKEY_GlobalTextColor, propvarText);

  //  spPropertyStore->Commit();
  //}

  updateRibbonHeight();
#ifdef RIBBON_DEBUG
  LOG_LEAVE(SEVERITY_LEVEL_DEBUG);
#endif
  return S_OK;
}

HRESULT gameOfLife3D::Ribbon::OnViewChanged(UINT32 nViewID, __in UI_VIEWTYPE typeID, __in IUnknown *pView,
                                            UI_VIEWVERB verb, INT32 uReasonCode) {
  UNREFERENCED_PARAMETER(nViewID);
  UNREFERENCED_PARAMETER(uReasonCode);
#ifdef RIBBON_DEBUG
  LOG_ENTER(SEVERITY_LEVEL_DEBUG);
#endif

  HRESULT hr = E_NOTIMPL;
  if (UI_VIEWTYPE_RIBBON == typeID) {
    switch (verb) {
    case UI_VIEWVERB_CREATE:
      if (nullptr == m_pUiRibbon) {
        hr = pView->QueryInterface(&m_pUiRibbon);
      }
      break;
    case UI_VIEWVERB_SIZE:
      if (nullptr != m_pUiRibbon && nullptr != m_pRibbonFactory) {
        updateRibbonHeight();
      }
      break;
    case UI_VIEWVERB_DESTROY:
      m_pUiRibbon = nullptr;
      hr = S_OK;
      break;
    }
  }

#ifdef RIBBON_DEBUG
  LOG_LEAVE(SEVERITY_LEVEL_DEBUG);
#endif
  return hr;
}

HRESULT gameOfLife3D::Ribbon::OnCreateUICommand(UINT32 nCmdID, __in UI_COMMANDTYPE typeID,
                                                __deref_out IUICommandHandler **ppCommandHandler) {
  UNREFERENCED_PARAMETER(typeID);
#ifdef RIBBON_DEBUG
  LOG_ENTER(SEVERITY_LEVEL_DEBUG);
#endif

  switch (nCmdID) {
  case cmdStart:
    return QueryInterface(IID_PPV_ARGS(ppCommandHandler));
  case cmdStop:
    return QueryInterface(IID_PPV_ARGS(ppCommandHandler));
  case cmdRandom:
    return QueryInterface(IID_PPV_ARGS(ppCommandHandler));
  case cmdOpenFile:
    return QueryInterface(IID_PPV_ARGS(ppCommandHandler));
  case cmdSaveAs:
    return QueryInterface(IID_PPV_ARGS(ppCommandHandler));
  case cmdSettings:
    return QueryInterface(IID_PPV_ARGS(ppCommandHandler));
  case cmdVersionInfo:
    return QueryInterface(IID_PPV_ARGS(ppCommandHandler));
  case cmdExit:
    return QueryInterface(IID_PPV_ARGS(ppCommandHandler));
  case cmdOpenCSFile:
    return QueryInterface(IID_PPV_ARGS(ppCommandHandler));
  }

#ifdef RIBBON_DEBUG
  LOG_LEAVE(SEVERITY_LEVEL_DEBUG);
#endif
  return E_NOTIMPL;
}

HRESULT gameOfLife3D::Ribbon::OnDestroyUICommand(UINT32 commandId, __in UI_COMMANDTYPE typeID,
                                                 __in_opt IUICommandHandler *pCommandHandler) {
  UNREFERENCED_PARAMETER(commandId);
  UNREFERENCED_PARAMETER(typeID);
  UNREFERENCED_PARAMETER(pCommandHandler);
#ifdef RIBBON_DEBUG
  LOG_ENTER(SEVERITY_LEVEL_DEBUG);
#endif

#ifdef RIBBON_DEBUG
  LOG_LEAVE(SEVERITY_LEVEL_DEBUG);
#endif
  return E_NOTIMPL;
}

HRESULT gameOfLife3D::Ribbon::Execute(UINT nCmdID, UI_EXECUTIONVERB verb, __in_opt const PROPERTYKEY *key,
                                      __in_opt const PROPVARIANT *ppropvarValue,
                                      __in_opt IUISimplePropertySet *pCommandExecutionProperties) {
  UNREFERENCED_PARAMETER(key);
  UNREFERENCED_PARAMETER(ppropvarValue);
  UNREFERENCED_PARAMETER(pCommandExecutionProperties);
#ifdef RIBBON_DEBUG
  LOG_ENTER(SEVERITY_LEVEL_DEBUG);
#endif

  std::weak_ptr<IMainWnd> mainWnd = m_pRibbonFactory->GetMainWindow();

  switch (verb) {
  case UI_EXECUTIONVERB_EXECUTE:
    switch (nCmdID) {
    case cmdStart:
      if (!mainWnd.expired()) {
        mainWnd.lock()->OnStart();
      }
      break;
    case cmdStop:
      if (!mainWnd.expired()) {
        mainWnd.lock()->OnStop();
      }
      break;
    case cmdRandom:
      if (!mainWnd.expired()) {
        mainWnd.lock()->OnRandom();
      }
      break;
    case cmdOpenFile:
      if (!mainWnd.expired()) {
        mainWnd.lock()->OnOpenFile();
      }
      break;
    case cmdSaveAs:
      if (!mainWnd.expired()) {
        mainWnd.lock()->OnSaveFileAs();
      }
      break;
    case cmdSettings:
      if (!mainWnd.expired()) {
        mainWnd.lock()->OnSettings();
      }
      break;
    case cmdVersionInfo:
      if (!mainWnd.expired()) {
        mainWnd.lock()->OnVersionInfo();
      }
      break;
    case cmdExit:
      if (!mainWnd.expired()) {
        mainWnd.lock()->OnExit();
      }
      break;
    case cmdOpenCSFile:
      if (!mainWnd.expired()) {
        mainWnd.lock()->OnOpenCSFile();
      }
      break;
    }
  }

#ifdef RIBBON_DEBUG
  LOG_LEAVE(SEVERITY_LEVEL_DEBUG);
#endif
  return E_NOTIMPL;
}

HRESULT gameOfLife3D::Ribbon::UpdateProperty(UINT nCmdID, __in REFPROPERTYKEY key,
                                             __in_opt const PROPVARIANT *ppropvarCurrentValue,
                                             __out PROPVARIANT *ppropvarNewValue) {
  UNREFERENCED_PARAMETER(nCmdID);
  UNREFERENCED_PARAMETER(key);
  UNREFERENCED_PARAMETER(ppropvarCurrentValue);
  UNREFERENCED_PARAMETER(ppropvarNewValue);
#ifdef RIBBON_DEBUG
  LOG_ENTER(SEVERITY_LEVEL_DEBUG);
  LOG_LEAVE(SEVERITY_LEVEL_DEBUG);
#endif
  return E_NOTIMPL;
}

void gameOfLife3D::Ribbon::updateRibbonHeight() {
  UINT ribbonHeight = GetRibbonHeight();
  PostMessage(m_pRibbonFactory->GetParentHWnd(), WM_RIBBON_RESIZED, static_cast<WPARAM>(ribbonHeight), 0);
}

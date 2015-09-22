#pragma once

#include "Common.h"
#include "gameOfLife3D/IMainWnd.h"

namespace gameOfLife3D {
  class Ribbon;

  class RibbonFactory {
  private:
    IUIFramework *m_pFramework;
    CComObject<Ribbon> *m_pApplication;
    std::weak_ptr<IMainWnd> m_mainWnd;

  public:
    RibbonFactory();
    ~RibbonFactory();
    HRESULT Initialize(std::weak_ptr<IMainWnd> mainWnd);
    CComObject<Ribbon> *GetApplication() const {
      return m_pApplication;
    }
    IUIFramework *GetFramework() const {
      return m_pFramework;
    }
    std::weak_ptr<IMainWnd> GetMainWindow() const {
      return m_mainWnd;
    }
    HWND GetParentHWnd() const;
  };
}

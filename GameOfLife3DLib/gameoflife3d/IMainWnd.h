#pragma once

#include "Common.h"

namespace gameOfLife3D {
  class IMainWnd {
  public:
    virtual UINT GetRibbonHeight() const = 0;
    virtual HWND GetHWnd() const = 0;
    virtual void OnStart() = 0;
    virtual void OnStop() = 0;
    virtual void OnRandom() = 0;
    virtual void OnOpenFile() = 0;
    virtual void OnSaveFileAs() = 0;
    virtual void OnSettings() = 0;
    virtual void OnVersionInfo() = 0;
    virtual void OnExit() = 0;
    virtual void OnOpenCSFile() = 0;
    virtual void SetFPSEnable(bool fpsEnabled) = 0;
    virtual bool IsFPSEnabled() const = 0;
  };
}
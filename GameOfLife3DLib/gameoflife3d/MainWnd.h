#pragma once

#include "GameOfLife3D/IMainWnd.h"

namespace tsf
{
class TSFManager;
}

namespace gameOfLife3D
{
class RibbonFactory;
class CanvasPanel;

class MainWnd :
    public IMainWnd,
    public std::enable_shared_from_this<MainWnd>
{
private:
    HWND m_hwnd;
    std::shared_ptr<tsf::TSFManager> m_tsfManager;
    std::shared_ptr<RibbonFactory> m_ribbonFactory;
    std::shared_ptr<CanvasPanel> m_canvasPanel;

protected:
    static LRESULT CALLBACK WndProc(
        HWND hWnd,
        UINT message,
        WPARAM wParam,
        LPARAM lParam);

public:
    MainWnd(void);
    ~MainWnd(void);
    HRESULT Initialize();
    HRESULT OnCreate(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);
    void RunMessageLoop();
    std::shared_ptr<tsf::TSFManager> GetTSFManager() const {
        return m_tsfManager;
    }
    virtual UINT GetRibbonHeight() const;
    virtual HWND GetHWnd() const {
        return m_hwnd;
    }
    virtual void OnStart();
    virtual void OnStop();
    virtual void OnRandom();
    virtual void OnOpenFile();
    virtual void OnSaveFileAs();
    virtual void OnSettings();
    virtual void OnVersionInfo();
    virtual void OnExit();
    virtual void OnOpenCSFile();
    virtual void SetFPSEnable(bool fpsEnabled);
    virtual bool IsFPSEnabled() const;
};
}


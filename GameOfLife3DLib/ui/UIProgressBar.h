#ifndef UI_UIPROGRESSBAR_H_
#define UI_UIPROGRESSBAR_H_

#include "graphics/GraphicsCommon.h"
#include "ui/UICommon.h"
#include "ui/UIBase.h"

namespace ui
{
class UIProgressBar :
    public UIBase,
    public std::enable_shared_from_this<UIProgressBar>
{
public:
    UIProgressBar(void);
    virtual ~UIProgressBar(void);
    virtual HRESULT Render(
        graphics::D3DInteropHelper *pD3DInteropHelper,
        ID2D1RenderTarget *pRenderTarget);
};
}

#endif // UI_UIPROGRESSBAR_H_
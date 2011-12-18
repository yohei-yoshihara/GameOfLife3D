#ifndef GRAPHICS_BRUSH_IBRUSH_H_
#define GRAPHICS_BRUSH_IBRUSH_H_

namespace graphics
{
namespace brush
{
class IBrush
{
public:
    virtual const std::wstring& GetLabel() const = 0;
    virtual HRESULT CreateDeviceDependentResource(
        ID2D1RenderTarget *pRenderTarget,
        const D2D1_RECT_F &rect,
        FLOAT opacity) = 0;
    virtual ID2D1Brush* GetBrush(
        const D2D1_RECT_F &rect,
        FLOAT opacity) = 0;
    virtual ID2D1Brush* GetBrush(
        FLOAT opacity) = 0;
    virtual ID2D1Brush* GetBrush() = 0;
    virtual void DiscardDeviceDependentResource() = 0;
    virtual bool IsDeviceDependentResourceCreated() const = 0;
    virtual void Clear() = 0;
};

}
}
#endif // GRAPHICS_BRUSH_IBRUSH_H_

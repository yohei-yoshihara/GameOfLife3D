#ifndef GRAPHICS_IGEOMETRYFILTER_H_
#define GRAPHICS_IGEOMETRYFILTER_H_

#include "graphics/GraphicsCommon.h"

namespace graphics
{
namespace filter
{
class IGeometryFilter
{
public:
    virtual HRESULT Initialize() = 0;
    // ジオメトリフィルターの適用
    // widthとheightには、通常はID2D1GeometryのGetBoundsから得られる値を与えればよい。
    // ただ、フィルターによってはジオメトリとは関係ない部分にもエフェクトを適用する場合も
    // ある。その場合は、適用してもらいたい領域全体を覆うwidthとheightを指定する。
    virtual HRESULT ApplyFilter(
        IN graphics::D3DInteropHelper *pD3DInteropHelper,
        IN ID2D1RenderTarget *pRenderTarget,
        IN ID2D1Geometry *pGeometry,
        IN FLOAT width,
        IN FLOAT height,
        IN ID2D1Brush *pBrush,
        OUT ID2D1Bitmap **ppBitmap,
        OUT D2D1_RECT_F *pBitmapRect
    ) = 0;
    // 与えられたwidthとheightから、上下左右のマージン（追加される領域）を計算する。
    // これは、ガウスフィルタのように図形の周りに効果を作成するフィルターの場合に
    // widthとheightより大きくなるために必要となる。
    virtual void CalculateMargins(
        IN FLOAT width, IN FLOAT height,
        OUT FLOAT &leftMargin,
        OUT FLOAT &topMargin,
        OUT FLOAT &rightMargin,
        OUT FLOAT &bottomMargin
    ) = 0;
};
}
}

#endif // GRAPHICS_IGEOMETRYFILTER_H_
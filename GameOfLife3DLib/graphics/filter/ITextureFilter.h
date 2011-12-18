#ifndef GRAPHICS_ITEXTUREFILTER_H_
#define GRAPHICS_ITEXTUREFILTER_H_

#include "graphics/GraphicsCommon.h"

namespace graphics
{
namespace filter
{
class ITextureFilter
{
public:
    virtual HRESULT Initialize() = 0;
    // テクスチャフィルターの適用
    // 出力先のテクスチャは、CalculateMarginsから計算されたマージンを持つテクスチャサイズ
    // のものを作成しておかなければならない。
    virtual HRESULT ApplyFilter(
        IN graphics::D3DInteropHelper *pD3DInteropHelper,
        IN ID3D10Texture2D *pSourceTexture,
        OUT ID3D10Texture2D **ppOutputTexture
    ) = 0;
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

#endif // GRAPHICS_ITEXTUREFILTER_H_
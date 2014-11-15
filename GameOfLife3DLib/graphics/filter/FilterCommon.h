#ifndef GRAPHICS_FILTER_FILTERCOMMON_H_
#define GRAPHICS_FILTER_FILTERCOMMON_H_

namespace graphics
{
namespace filter
{
struct Vertex {
    DirectX::XMFLOAT3 Pos;
	DirectX::XMFLOAT2 Tex;
};

class FilterCommon
{
public:
    static const D3D10_INPUT_ELEMENT_DESC INPUT_LAYOUT[];
    static const UINT INPUT_LAYOUT_ARRAYSIZE;
    static const graphics::filter::Vertex VERTEX_ARRAY[];
    static const UINT VERTEX_ARRAY_BYTEWIDTH;
    static const SHORT INDEX_ARRAY[];
    static const UINT INDEX_ARRAY_BYTEWIDTH;
};

}
}


#endif // GRAPHICS_FILTER_FILTERCOMMON_H_

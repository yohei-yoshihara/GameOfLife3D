#include "stdafx.h"
#include "graphics/filter/FilterCommon.h"

using namespace DirectX;

const D3D10_INPUT_ELEMENT_DESC graphics::filter::FilterCommon::INPUT_LAYOUT[] = {
    {"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D10_INPUT_PER_VERTEX_DATA, 0},
    {"TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 12, D3D10_INPUT_PER_VERTEX_DATA, 0},
};

const UINT graphics::filter::FilterCommon::INPUT_LAYOUT_ARRAYSIZE
    = ARRAYSIZE(graphics::filter::FilterCommon::INPUT_LAYOUT);

const graphics::filter::Vertex graphics::filter::FilterCommon::VERTEX_ARRAY[]
    = {{XMFLOAT3(-1.0f, -1.0f, 0.0f), XMFLOAT2(0.0f, 1.0f)},
       {XMFLOAT3(1.0f, -1.0f, 0.0f), XMFLOAT2(1.0f, 1.0f)},
       {XMFLOAT3(1.0f, 1.0f, 0.0f), XMFLOAT2(1.0f, 0.0f)},
       {XMFLOAT3(-1.0f, 1.0f, 0.0f), XMFLOAT2(0.0f, 0.0f)}};

const UINT graphics::filter::FilterCommon::VERTEX_ARRAY_BYTEWIDTH
    = sizeof(graphics::filter::FilterCommon::VERTEX_ARRAY);

const SHORT graphics::filter::FilterCommon::INDEX_ARRAY[] = {3, 1, 0, 2, 1, 3};

const UINT graphics::filter::FilterCommon::INDEX_ARRAY_BYTEWIDTH = sizeof(graphics::filter::FilterCommon::INDEX_ARRAY);

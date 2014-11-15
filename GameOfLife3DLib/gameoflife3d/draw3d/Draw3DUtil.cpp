#include "stdafx.h"
#include "gameOfLife3D/draw3d/Draw3DUtil.h"

using namespace DirectX;

void gameOfLife3D::draw3d::Draw3DUtil::CalculateP0P1(
    UINT width, UINT height, UINT generations,
    OUT XMFLOAT3 &p0, OUT XMFLOAT3 &p1 )
{
    FLOAT n = static_cast<FLOAT>(std::max(std::max(
            width, height), generations));
    FLOAT widthPos = width / n;
    FLOAT heightPos = height / n;
    FLOAT generationsPos = generations / n;

    p0 = XMFLOAT3(-widthPos, -heightPos, -generationsPos);
    p1 = XMFLOAT3(widthPos, heightPos, generationsPos);
}

void gameOfLife3D::draw3d::Draw3DUtil::CalculateLengthAndMargin(
    UINT width, UINT height, UINT generations, XMFLOAT3 p0, XMFLOAT3 p1, FLOAT marginRate,
    OUT XMFLOAT3 &cubeLength, OUT XMFLOAT3 &cubeMargin )
{
    if (width == 1) {
        cubeLength.x = std::abs(p1.x - p0.x);
        cubeMargin.x = 0.0f;
    } else {
        cubeLength.x = std::abs(p1.x - p0.x) * (1.0f - marginRate) / width;
        cubeMargin.x = std::abs(p1.x - p0.x) * marginRate / (width - 1);
    }
    if (height == 1) {
        cubeLength.y = std::abs(p1.y - p0.y);
        cubeMargin.y = 0.0f;
    } else {
        cubeLength.y = std::abs(p1.y - p0.y) * (1.0f - marginRate) / height;
        cubeMargin.y = std::abs(p1.y - p0.y) * marginRate / (height - 1);
    }
    if (generations == 1) {
        cubeLength.z = std::abs(p1.z - p0.z);
        cubeMargin.z = 0.0f;
    } else {
        cubeLength.z = std::abs(p1.z - p0.z) * (1.0f - marginRate) / generations;
        cubeMargin.z = std::abs(p1.z - p0.z) * marginRate / (generations - 1);
    }
}

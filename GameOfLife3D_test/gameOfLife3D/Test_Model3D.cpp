#include "stdafx.h"
#include "Common.h"
#include "gameOfLife3D/draw3d/VertexGenerator.h"

void MinMax(const float value, float &minValue, float &maxValue)
{
    if (value < minValue) {
        minValue = value;
    }
    if (value > maxValue) {
        maxValue = value;
    }
}

void AnalyzeVertices(const std::vector<gameOfLife3D::draw3d::VertexInfo> &vertices, OUT std::vector<XMFLOAT3> &cubes)
{
    for (size_t i = 0; i < vertices.size(); i += 24) {
        LOG(SEVERITY_LEVEL_DEBUG) << L"*** i = " << i;
        XMFLOAT3 minPos = XMFLOAT3(FLT_MAX, FLT_MAX, FLT_MAX);
        XMFLOAT3 maxPos = XMFLOAT3(-FLT_MAX, -FLT_MAX, -FLT_MAX);
        for (size_t j = 0; j < 24; ++j) {
            LOG(SEVERITY_LEVEL_DEBUG) << L"****** j = " << j;
            LOG(SEVERITY_LEVEL_DEBUG) << L"****** index = " << (i + j);
            const gameOfLife3D::draw3d::VertexInfo &v = vertices[i + j];
            MinMax(v.pos.x, minPos.x, maxPos.x);
            MinMax(v.pos.y, minPos.y, maxPos.y);
            MinMax(v.pos.z, minPos.z, maxPos.z);
        }
        LOG(SEVERITY_LEVEL_DEBUG) << L"*** min = " << minPos.x << L"," << minPos.y << L"," << minPos.z;
        LOG(SEVERITY_LEVEL_DEBUG) << L"*** max = " << maxPos.x << L"," << maxPos.y << L"," << maxPos.z;
        XMFLOAT3 centerPos;
        centerPos.x = (maxPos.x - minPos.x) / 2.0f + minPos.x;
        centerPos.y = (maxPos.y - minPos.y) / 2.0f + minPos.y;
        centerPos.z = (maxPos.z - minPos.z) / 2.0f + minPos.z;
        LOG(SEVERITY_LEVEL_DEBUG) << L"*** center = " << centerPos.x << L"," << centerPos.y << L"," << centerPos.z;
        cubes.push_back(centerPos);
    }
}
/*
TEST( gameOfLife3D_Model3D, UpdateLengthAndMargin_001 )
{
    const static UINT width = 1;
    const static UINT generation = 1;
    gameOfLife3D::Model3D model3d;
    model3d.SetWidth(width);
    model3d.SetMaxNumberOfGenerations(generation);
    model3d.SetMarginRate(0.0f);
    model3d.SetP0(XMFLOAT3(0.0f, 0.0f, 0.0f));
    model3d.SetP1(XMFLOAT3(10.0f, 10.0f, 10.0f));
    model3d.UpdateLengthAndMargin();

    XMFLOAT3 cubeLength = model3d.GetCubeLength();
    ASSERT_FLOAT_EQ(10.0f, cubeLength.x);
    ASSERT_FLOAT_EQ(10.0f, cubeLength.y);
    ASSERT_FLOAT_EQ(10.0f, cubeLength.z);

    XMFLOAT3 cubeMargin = model3d.GetCubeMargin();
    ASSERT_FLOAT_EQ(0.0f, cubeMargin.x);
    ASSERT_FLOAT_EQ(0.0f, cubeMargin.y);
    ASSERT_FLOAT_EQ(0.0f, cubeMargin.z);
}

TEST( gameOfLife3D_Model3D, UpdateLengthAndMargin_002 )
{
    const static UINT width = 2;
    const static UINT generation = 1;
    gameOfLife3D::Model3D model3d;
    model3d.SetWidth(width);
    model3d.SetMaxNumberOfGenerations(generation);
    model3d.SetMarginRate(0.2f);
    model3d.SetP0(XMFLOAT3(0.0f, 0.0f, 0.0f));
    model3d.SetP1(XMFLOAT3(10.0f, 10.0f, 10.0f));
    model3d.UpdateLengthAndMargin();

    XMFLOAT3 cubeLength = model3d.GetCubeLength();
    ASSERT_FLOAT_EQ(4.0f, cubeLength.x);
    ASSERT_FLOAT_EQ(4.0f, cubeLength.y);
    ASSERT_FLOAT_EQ(10.0f, cubeLength.z);

    XMFLOAT3 cubeMargin = model3d.GetCubeMargin();
    ASSERT_FLOAT_EQ(2.0f, cubeMargin.x);
    ASSERT_FLOAT_EQ(2.0f, cubeMargin.y);
    ASSERT_FLOAT_EQ(0.0f, cubeMargin.z);
}

TEST( gameOfLife3D_Model3D, UpdateLengthAndMargin_003 )
{
    const static UINT width = 2;
    const static UINT generation = 2;
    gameOfLife3D::Model3D model3d;
    model3d.SetWidth(width);
    model3d.SetMaxNumberOfGenerations(generation);
    model3d.SetMarginRate(0.2f);
    model3d.SetP0(XMFLOAT3(0.0f, 0.0f, 0.0f));
    model3d.SetP1(XMFLOAT3(10.0f, 10.0f, 10.0f));
    model3d.UpdateLengthAndMargin();

    XMFLOAT3 cubeLength = model3d.GetCubeLength();
    ASSERT_FLOAT_EQ(4.0f, cubeLength.x);
    ASSERT_FLOAT_EQ(4.0f, cubeLength.y);
    ASSERT_FLOAT_EQ(4.0f, cubeLength.z);

    XMFLOAT3 cubeMargin = model3d.GetCubeMargin();
    ASSERT_FLOAT_EQ(2.0f, cubeMargin.x);
    ASSERT_FLOAT_EQ(2.0f, cubeMargin.y);
    ASSERT_FLOAT_EQ(2.0f, cubeMargin.z);
}

TEST( gameOfLife3D_Model3D, test_001 )
{
    const static UINT width = 1;
    gameOfLife3D::Model3D model3d;
    model3d.SetWidth(width);
    model3d.SetMaxNumberOfGenerations(1);
    model3d.SetMarginRate(0.0f);
    model3d.SetP0(XMFLOAT3(0.0f, 0.0f, 0.0f));
    model3d.SetP1(XMFLOAT3(10.0f, 10.0f, 10.0f));
    model3d.Initialize(nullptr);
    std::array<gameOfLife3D::LifeData, width * width> array;
    array[0].alive = 1;
    array[0].color = XMFLOAT3(1.0f, 0.0f, 0.0f);
    model3d.Add(&array.front());
    std::vector<gameOfLife3D::VertexInfo> vertexes;
    std::vector<VERTEX_INDEX> indexes;
    model3d.CopyVertexes(vertexes);
    model3d.CopyIndexes(indexes);

    ASSERT_EQ(24, vertexes.size());
    std::for_each(vertexes.begin(), vertexes.end(), [](gameOfLife3D::VertexInfo & v) {
        LOG(SEVERITY_LEVEL_INFO) << v;
    });

    ASSERT_EQ(36, indexes.size());
    std::for_each(indexes.begin(), indexes.end(), [](VERTEX_INDEX index) {
        LOG(SEVERITY_LEVEL_INFO) << index;
    });

    std::vector<XMFLOAT3> cubes;
    AnalyzeVertices(vertexes, cubes);
    ASSERT_EQ(1, cubes.size());
    ASSERT_FLOAT_EQ(5.0f, cubes[0].x);
    ASSERT_FLOAT_EQ(5.0f, cubes[0].y);
    ASSERT_FLOAT_EQ(5.0f, cubes[0].z);
}

TEST( gameOfLife3D_Model3D, test_002 )
{
    const static UINT width = 2;
    gameOfLife3D::Model3D model3d;
    model3d.SetWidth(width);
    model3d.SetMaxNumberOfGenerations(1);
    model3d.SetMarginRate(0.0f);
    model3d.SetP0(XMFLOAT3(0.0f, 0.0f, 0.0f));
    model3d.SetP1(XMFLOAT3(10.0f, 10.0f, 10.0f));
    model3d.Initialize(nullptr);
    std::array<gameOfLife3D::LifeData, width * width> array;
    std::for_each(array.begin(), array.end(), [](gameOfLife3D::LifeData & lifeData) {
        lifeData.alive = 1;
        lifeData.color = XMFLOAT3(1.0f, 0.0f, 0.0f);
    });
    model3d.Add(&array.front());
    std::vector<gameOfLife3D::VertexInfo> vertexes;
    std::vector<VERTEX_INDEX> indexes;
    model3d.CopyVertexes(vertexes);
    model3d.CopyIndexes(indexes);

    ASSERT_EQ(24 * 4, vertexes.size());
    std::for_each(vertexes.begin(), vertexes.end(), [](gameOfLife3D::VertexInfo & v) {
        LOG(SEVERITY_LEVEL_INFO) << v;
    });

    ASSERT_EQ(36 * 4, indexes.size());
    std::for_each(indexes.begin(), indexes.end(), [](VERTEX_INDEX index) {
        LOG(SEVERITY_LEVEL_INFO) << index;
    });

    std::vector<XMFLOAT3> cubes;
    AnalyzeVertices(vertexes, cubes);
    ASSERT_EQ(4, cubes.size());
    std::for_each(cubes.begin(), cubes.end(), [](XMFLOAT3 &f3){
        LOG(SEVERITY_LEVEL_INFO) << f3.x << "," << f3.y << "," << f3.z;
    });
    ASSERT_FLOAT_EQ(2.5f, cubes[0].x);
    ASSERT_FLOAT_EQ(2.5f, cubes[0].y);
    ASSERT_FLOAT_EQ(5.0f, cubes[0].z);

    ASSERT_FLOAT_EQ(7.5f, cubes[1].x);
    ASSERT_FLOAT_EQ(2.5f, cubes[1].y);
    ASSERT_FLOAT_EQ(5.0f, cubes[1].z);

    ASSERT_FLOAT_EQ(2.5f, cubes[2].x);
    ASSERT_FLOAT_EQ(7.5f, cubes[2].y);
    ASSERT_FLOAT_EQ(5.0f, cubes[2].z);

    ASSERT_FLOAT_EQ(7.5f, cubes[3].x);
    ASSERT_FLOAT_EQ(7.5f, cubes[3].y);
    ASSERT_FLOAT_EQ(5.0f, cubes[3].z);
}

TEST( gameOfLife3D_Model3D, test_003 )
{
    const static UINT width = 2;
    gameOfLife3D::Model3D model3d;
    model3d.SetWidth(width);
    model3d.SetMaxNumberOfGenerations(1);
    model3d.SetMarginRate(0.2f);
    model3d.SetP0(XMFLOAT3(0.0f, 0.0f, 0.0f));
    model3d.SetP1(XMFLOAT3(10.0f, 10.0f, 10.0f));
    model3d.Initialize(nullptr);
    std::array<gameOfLife3D::LifeData, width * width> array;
    std::for_each(array.begin(), array.end(), [](gameOfLife3D::LifeData & lifeData) {
        lifeData.alive = 1;
        lifeData.color = XMFLOAT3(1.0f, 0.0f, 0.0f);
    });
    model3d.Add(&array.front());
    std::vector<gameOfLife3D::VertexInfo> vertexes;
    std::vector<VERTEX_INDEX> indexes;
    model3d.CopyVertexes(vertexes);
    model3d.CopyIndexes(indexes);

    ASSERT_EQ(24 * 4, vertexes.size());
    std::for_each(vertexes.begin(), vertexes.end(), [](gameOfLife3D::VertexInfo & v) {
        LOG(SEVERITY_LEVEL_INFO) << v;
    });

    ASSERT_EQ(36 * 4, indexes.size());
    std::for_each(indexes.begin(), indexes.end(), [](VERTEX_INDEX index) {
        LOG(SEVERITY_LEVEL_INFO) << index;
    });

    std::vector<XMFLOAT3> cubes;
    AnalyzeVertices(vertexes, cubes);
    ASSERT_EQ(4, cubes.size());
    std::for_each(cubes.begin(), cubes.end(), [](XMFLOAT3 &f3){
        LOG(SEVERITY_LEVEL_INFO) << f3.x << "," << f3.y << "," << f3.z;
    });
    ASSERT_FLOAT_EQ(2.0f, cubes[0].x);
    ASSERT_FLOAT_EQ(2.0f, cubes[0].y);
    ASSERT_FLOAT_EQ(5.0f, cubes[0].z);

    ASSERT_FLOAT_EQ(8.0f, cubes[1].x);
    ASSERT_FLOAT_EQ(2.0f, cubes[1].y);
    ASSERT_FLOAT_EQ(5.0f, cubes[1].z);

    ASSERT_FLOAT_EQ(2.0f, cubes[2].x);
    ASSERT_FLOAT_EQ(8.0f, cubes[2].y);
    ASSERT_FLOAT_EQ(5.0f, cubes[2].z);

    ASSERT_FLOAT_EQ(8.0f, cubes[3].x);
    ASSERT_FLOAT_EQ(8.0f, cubes[3].y);
    ASSERT_FLOAT_EQ(5.0f, cubes[3].z);
}
*/
TEST( gameOfLife3D_Model3D, test_004 )
{
    const static UINT width = 1;
    gameOfLife3D::draw3d::VertexGenerator model3d;
    model3d.SetWidth(width);
    model3d.SetMaxNumberOfGenerations(2);
    model3d.SetMarginRate(0.2f);
    model3d.SetP0(XMFLOAT3(0.0f, 0.0f, 0.0f));
    model3d.SetP1(XMFLOAT3(10.0f, 10.0f, 10.0f));
    model3d.Initialize(nullptr);
    std::array<gameOfLife3D::lifeSimulator::LifeData, width * width> array;
    std::for_each(array.begin(), array.end(), [](gameOfLife3D::lifeSimulator::LifeData & lifeData) {
        lifeData.alive = 1;
        lifeData.color = XMFLOAT3(1.0f, 0.0f, 0.0f);
    });
    ASSERT_EQ(0, model3d.GetNumberOfGenerations());
    model3d.Add(&array.front());
    ASSERT_EQ(1, model3d.GetNumberOfGenerations());
    model3d.Add(&array.front());
    ASSERT_EQ(2, model3d.GetNumberOfGenerations());
    std::vector<gameOfLife3D::draw3d::VertexInfo> vertexes;
    std::vector<gameOfLife3D::draw3d::VERTEX_INDEX> indexes;
    model3d.CopyVertexes(vertexes);
    model3d.CopyIndexes(indexes);

    ASSERT_EQ(24 * 1 * 2, vertexes.size());
    std::for_each(vertexes.begin(), vertexes.end(), [](gameOfLife3D::draw3d::VertexInfo & v) {
        LOG(SEVERITY_LEVEL_INFO) << v;
    });

    ASSERT_EQ(36 * 1 * 2, indexes.size());
    std::for_each(indexes.begin(), indexes.end(), [](gameOfLife3D::draw3d::VERTEX_INDEX index) {
        LOG(SEVERITY_LEVEL_INFO) << index;
    });

    std::vector<XMFLOAT3> cubes;
    AnalyzeVertices(vertexes, cubes);
    ASSERT_EQ(2, cubes.size());
    std::for_each(cubes.begin(), cubes.end(), [](XMFLOAT3 & f3) {
        LOG(SEVERITY_LEVEL_INFO) << f3.x << "," << f3.y << "," << f3.z;
    });
    ASSERT_FLOAT_EQ(5.0f, cubes[0].x);
    ASSERT_FLOAT_EQ(5.0f, cubes[0].y);
    ASSERT_FLOAT_EQ(2.0f, cubes[0].z);

    ASSERT_FLOAT_EQ(5.0f, cubes[1].x);
    ASSERT_FLOAT_EQ(5.0f, cubes[1].y);
    ASSERT_FLOAT_EQ(8.0f, cubes[1].z);

    model3d.Add(&array.front());
    ASSERT_EQ(2, model3d.GetNumberOfGenerations());
    model3d.CopyVertexes(vertexes);
    model3d.CopyIndexes(indexes);
    ASSERT_EQ(24 * 1 * 2, vertexes.size());
    ASSERT_EQ(36 * 1 * 2, indexes.size());

    AnalyzeVertices(vertexes, cubes);

    ASSERT_FLOAT_EQ(5.0f, cubes[0].x);
    ASSERT_FLOAT_EQ(5.0f, cubes[0].y);
    ASSERT_FLOAT_EQ(2.0f, cubes[0].z);

    ASSERT_FLOAT_EQ(5.0f, cubes[1].x);
    ASSERT_FLOAT_EQ(5.0f, cubes[1].y);
    ASSERT_FLOAT_EQ(8.0f, cubes[1].z);

}

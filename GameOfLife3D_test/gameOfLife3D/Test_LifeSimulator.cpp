#include "stdafx.h"
#include "Common.h"
#include "graphics/D3DInteropHelper.h"
#include "graphics/ID3DInteropAdapter.h"
#include "gameOfLife3D/lifeSimulator/LifeSimulatorCS.h"

#include "DummyAdapter.h"

//#define TEST_LIFESIMULATOR

#ifdef TEST_LIFESIMULATOR

TEST( gameOfLife3D_LifeSimulator, test_001 )
{
    static const UINT width = 3;
    int data[] = {
        0, 0, 0,
        0, 1, 0,
        0, 0, 0,
    };
    int result[] = {
        0, 0, 0,
        0, 0, 0,
        0, 0, 0,
    };

    CoInitialize(nullptr);
    std::shared_ptr<DummyAdapter> dummy = std::make_shared<DummyAdapter>();
    graphics::D3DInteropHelper helper(true);
    helper.SetD3DInteropAdapter(dummy);
    helper.SetHWnd((HWND)1);
    EXPECT_HRESULT_SUCCEEDED(helper.CreateDeviceResources(0));
    EXPECT_HRESULT_SUCCEEDED(helper.CreateDeviceIndependentResources());

    gameOfLife3D::LifeSimulatorCS sim;
    sim.SetWidth(width);
    sim.SetInitialDataGenerator([&](UINT x, UINT y)->gameOfLife3D::LifeData {
        size_t index = x + y * width;
        gameOfLife3D::LifeData lifeData;
        lifeData.alive = data[index];
        return lifeData;
    });
    EXPECT_HRESULT_SUCCEEDED(sim.Initialize(&helper));
    EXPECT_HRESULT_SUCCEEDED(sim.Compute());

    std::array<gameOfLife3D::LifeData, width * width> answer;
    sim.CopyCurrent(&answer.front());
    for (size_t i = 0; i < answer.size(); ++i) {
        ASSERT_EQ(result[i], answer[i].alive);
    }
}

TEST( gameOfLife3D_LifeSimulator, test_002 )
{
    static const UINT width = 3;
    int data[] = {
        1, 1, 0,
        0, 0, 0,
        0, 0, 1,
    };
    int result[] = {
        0, 0, 0,
        0, 1, 0,
        0, 0, 0,
    };

    CoInitialize(nullptr);
    std::shared_ptr<DummyAdapter> dummy = std::make_shared<DummyAdapter>();
    graphics::D3DInteropHelper helper(true);
    helper.SetD3DInteropAdapter(dummy);
    helper.SetHWnd((HWND)1);
    EXPECT_HRESULT_SUCCEEDED(helper.CreateDeviceResources(0));
    EXPECT_HRESULT_SUCCEEDED(helper.CreateDeviceIndependentResources());

    gameOfLife3D::LifeSimulatorCS sim;
    sim.SetWidth(width);
    sim.SetInitialDataGenerator([&](UINT x, UINT y)->gameOfLife3D::LifeData {
        size_t index = x + y * width;
        gameOfLife3D::LifeData lifeData;
        lifeData.alive = data[index];
        return lifeData;
    });
    EXPECT_HRESULT_SUCCEEDED(sim.Initialize(&helper));
    EXPECT_HRESULT_SUCCEEDED(sim.Compute());

    std::array<gameOfLife3D::LifeData, width * width> answer;
    sim.CopyCurrent(&answer.front());
    for (size_t i = 0; i < answer.size(); ++i) {
        ASSERT_EQ(result[i], answer[i].alive);
    }
}

TEST( gameOfLife3D_LifeSimulator, test_003 )
{
    static const UINT width = 3;
    int data[] = {
        1, 1, 0,
        0, 1, 0,
        0, 0, 0,
    };
    int result[] = {
        1, 1, 0,
        1, 1, 0,
        0, 0, 0,
    };

    CoInitialize(nullptr);
    std::shared_ptr<DummyAdapter> dummy = std::make_shared<DummyAdapter>();
    graphics::D3DInteropHelper helper(true);
    helper.SetD3DInteropAdapter(dummy);
    helper.SetHWnd((HWND)1);
    EXPECT_HRESULT_SUCCEEDED(helper.CreateDeviceResources(0));
    EXPECT_HRESULT_SUCCEEDED(helper.CreateDeviceIndependentResources());

    gameOfLife3D::LifeSimulatorCS sim;
    sim.SetWidth(width);
    sim.SetInitialDataGenerator([&](UINT x, UINT y)->gameOfLife3D::LifeData {
        size_t index = x + y * width;
        gameOfLife3D::LifeData lifeData;
        lifeData.alive = data[index];
        return lifeData;
    });
    EXPECT_HRESULT_SUCCEEDED(sim.Initialize(&helper));
    EXPECT_HRESULT_SUCCEEDED(sim.Compute());

    std::array<gameOfLife3D::LifeData, width * width> answer;
    sim.CopyCurrent(&answer.front());
    for (size_t i = 0; i < answer.size(); ++i) {
        ASSERT_EQ(result[i], answer[i].alive);
    }
}

TEST( gameOfLife3D_LifeSimulator, test_004 )
{
    static const UINT width = 3;
    int data[] = {
        0, 0, 0,
        0, 1, 1,
        0, 0, 0,
    };
    int result[] = {
        0, 0, 0,
        0, 0, 0,
        0, 0, 0,
    };

    CoInitialize(nullptr);
    std::shared_ptr<DummyAdapter> dummy = std::make_shared<DummyAdapter>();
    graphics::D3DInteropHelper helper(true);
    helper.SetD3DInteropAdapter(dummy);
    helper.SetHWnd((HWND)1);
    EXPECT_HRESULT_SUCCEEDED(helper.CreateDeviceResources(0));
    EXPECT_HRESULT_SUCCEEDED(helper.CreateDeviceIndependentResources());

    gameOfLife3D::LifeSimulatorCS sim;
    sim.SetWidth(width);
    sim.SetInitialDataGenerator([&](UINT x, UINT y)->gameOfLife3D::LifeData {
        size_t index = x + y * width;
        gameOfLife3D::LifeData lifeData;
        lifeData.alive = data[index];
        return lifeData;
    });
    EXPECT_HRESULT_SUCCEEDED(sim.Initialize(&helper));
    EXPECT_HRESULT_SUCCEEDED(sim.Compute());

    std::array<gameOfLife3D::LifeData, width * width> answer;
    sim.CopyCurrent(&answer.front());
    for (size_t i = 0; i < answer.size(); ++i) {
        ASSERT_EQ(result[i], answer[i].alive);
    }
}

TEST( gameOfLife3D_LifeSimulator, test_005 )
{
    static const UINT width = 3;
    int data[] = {
        1, 1, 1,
        1, 1, 0,
        0, 0, 0,
    };
    int result[] = {
        1, 0, 1,
        1, 0, 1,
        0, 0, 0,
    };

    CoInitialize(nullptr);
    std::shared_ptr<DummyAdapter> dummy = std::make_shared<DummyAdapter>();
    graphics::D3DInteropHelper helper(true);
    helper.SetD3DInteropAdapter(dummy);
    helper.SetHWnd((HWND)1);
    EXPECT_HRESULT_SUCCEEDED(helper.CreateDeviceResources(0));
    EXPECT_HRESULT_SUCCEEDED(helper.CreateDeviceIndependentResources());

    gameOfLife3D::LifeSimulatorCS sim;
    sim.SetWidth(width);
    sim.SetInitialDataGenerator([&](UINT x, UINT y)->gameOfLife3D::LifeData {
        size_t index = x + y * width;
        gameOfLife3D::LifeData lifeData;
        lifeData.alive = data[index];
        return lifeData;
    });
    EXPECT_HRESULT_SUCCEEDED(sim.Initialize(&helper));
    EXPECT_HRESULT_SUCCEEDED(sim.Compute());

    std::array<gameOfLife3D::LifeData, width * width> answer;
    sim.CopyCurrent(&answer.front());
    for (size_t i = 0; i < answer.size(); ++i) {
        ASSERT_EQ(result[i], answer[i].alive);
    }
}

TEST( gameOfLife3D_LifeSimulator, test_006 )
{
    static const UINT width = 3;
    int data[] = {
        0, 1, 0,
        0, 1, 0,
        0, 1, 0,
    };
    int result1[] = {
        0, 0, 0,
        1, 1, 1,
        0, 0, 0,
    };
    int result2[] = {
        0, 1, 0,
        0, 1, 0,
        0, 1, 0,
    };

    CoInitialize(nullptr);
    std::shared_ptr<DummyAdapter> dummy = std::make_shared<DummyAdapter>();
    graphics::D3DInteropHelper helper(true);
    helper.SetD3DInteropAdapter(dummy);
    helper.SetHWnd((HWND)1);
    EXPECT_HRESULT_SUCCEEDED(helper.CreateDeviceResources(0));
    EXPECT_HRESULT_SUCCEEDED(helper.CreateDeviceIndependentResources());

    gameOfLife3D::LifeSimulatorCS sim;
    sim.SetWidth(width);
    sim.SetInitialDataGenerator([&](UINT x, UINT y)->gameOfLife3D::LifeData {
        size_t index = x + y * width;
        gameOfLife3D::LifeData lifeData;
        lifeData.alive = data[index];
        return lifeData;
    });
    EXPECT_HRESULT_SUCCEEDED(sim.Initialize(&helper));

    // generation 1
    EXPECT_HRESULT_SUCCEEDED(sim.Compute());
    std::array<gameOfLife3D::LifeData, width * width> answer;
    sim.CopyCurrent(&answer.front());
    for (size_t i = 0; i < answer.size(); ++i) {
        ASSERT_EQ(result1[i], answer[i].alive);
    }

    // generation 2
    EXPECT_HRESULT_SUCCEEDED(sim.Compute());
    sim.CopyCurrent(&answer.front());
    for (size_t i = 0; i < answer.size(); ++i) {
        ASSERT_EQ(result2[i], answer[i].alive);
    }
}

TEST( gameOfLife3D_LifeSimulator, test_007 )
{
    static const UINT width = 8;
    int data[] = {
        0, 0, 0, 1, 1, 0, 0, 0,
        0, 0, 1, 0, 0, 1, 0, 0,
        0, 1, 0, 0, 0, 0, 1, 0,
        1, 0, 0, 0, 0, 0, 0, 1,
        1, 0, 0, 0, 0, 0, 0, 1,
        0, 1, 0, 0, 0, 0, 1, 0,
        0, 0, 1, 0, 0, 1, 0, 0,
        0, 0, 0, 1, 1, 0, 0, 0,
    };
    int result1[] = {
        0, 0, 0, 1, 1, 0, 0, 0,
        0, 0, 1, 1, 1, 1, 0, 0,
        0, 1, 0, 0, 0, 0, 1, 0,
        1, 1, 0, 0, 0, 0, 1, 1,
        1, 1, 0, 0, 0, 0, 1, 1,
        0, 1, 0, 0, 0, 0, 1, 0,
        0, 0, 1, 1, 1, 1, 0, 0,
        0, 0, 0, 1, 1, 0, 0, 0,
    };
    int result2[] = {
        0, 0, 1, 0, 0, 1, 0, 0,
        0, 0, 1, 0, 0, 1, 0, 0,
        1, 1, 0, 1, 1, 0, 1, 1,
        0, 0, 1, 0, 0, 1, 0, 0,
        0, 0, 1, 0, 0, 1, 0, 0,
        1, 1, 0, 1, 1, 0, 1, 1,
        0, 0, 1, 0, 0, 1, 0, 0,
        0, 0, 1, 0, 0, 1, 0, 0,
    };
    int result3[] = {
        0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 1, 0, 0, 1, 0, 0,
        0, 1, 0, 1, 1, 0, 1, 0,
        0, 0, 1, 0, 0, 1, 0, 0,
        0, 0, 1, 0, 0, 1, 0, 0,
        0, 1, 0, 1, 1, 0, 1, 0,
        0, 0, 1, 0, 0, 1, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0,
    };
    int result4[] = {
        0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 1, 1, 1, 1, 0, 0,
        0, 1, 0, 1, 1, 0, 1, 0,
        0, 1, 1, 0, 0, 1, 1, 0,
        0, 1, 1, 0, 0, 1, 1, 0,
        0, 1, 0, 1, 1, 0, 1, 0,
        0, 0, 1, 1, 1, 1, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0,
    };

    CoInitialize(nullptr);
    std::shared_ptr<DummyAdapter> dummy = std::make_shared<DummyAdapter>();
    graphics::D3DInteropHelper helper(true);
    helper.SetD3DInteropAdapter(dummy);
    helper.SetHWnd((HWND)1);
    EXPECT_HRESULT_SUCCEEDED(helper.CreateDeviceResources(0));
    EXPECT_HRESULT_SUCCEEDED(helper.CreateDeviceIndependentResources());

    gameOfLife3D::LifeSimulatorCS sim;
    sim.SetWidth(width);
    sim.SetInitialDataGenerator([&](UINT x, UINT y)->gameOfLife3D::LifeData {
        size_t index = x + y * width;
        gameOfLife3D::LifeData lifeData;
        lifeData.alive = data[index];
        return lifeData;
    });
    EXPECT_HRESULT_SUCCEEDED(sim.Initialize(&helper));

    // generation 1
    EXPECT_HRESULT_SUCCEEDED(sim.Compute());
    std::array<gameOfLife3D::LifeData, width * width> answer;
    sim.CopyCurrent(&answer.front());
    for (size_t i = 0; i < answer.size(); ++i) {
        ASSERT_EQ(result1[i], answer[i].alive);
    }

    // generation 2
    EXPECT_HRESULT_SUCCEEDED(sim.Compute());
    sim.CopyCurrent(&answer.front());
    for (size_t i = 0; i < answer.size(); ++i) {
        ASSERT_EQ(result2[i], answer[i].alive);
    }

    // generation 3
    EXPECT_HRESULT_SUCCEEDED(sim.Compute());
    sim.CopyCurrent(&answer.front());
    for (size_t i = 0; i < answer.size(); ++i) {
        ASSERT_EQ(result3[i], answer[i].alive);
    }

    // generation 4
    EXPECT_HRESULT_SUCCEEDED(sim.Compute());
    sim.CopyCurrent(&answer.front());
    for (size_t i = 0; i < answer.size(); ++i) {
        ASSERT_EQ(result4[i], answer[i].alive);
    }

    // generation 5
    EXPECT_HRESULT_SUCCEEDED(sim.Compute());
    sim.CopyCurrent(&answer.front());
    for (size_t i = 0; i < answer.size(); ++i) {
        ASSERT_EQ(data[i], answer[i].alive);
    }
}

#endif

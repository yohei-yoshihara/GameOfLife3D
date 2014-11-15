#include "stdafx.h"
#include "Common.h"
#include "gameOfLife3D/lifeSimulator/LifeSimulatorCPU.h"

using namespace DirectX;

static const int g_sumIndex[8][2] = {
    { -1, -1},
    { 0, -1},
    { 1, -1},
    { -1,  0},
    { 1,  0},
    { -1,  1},
    { 0,  1},
    { 1,  1},
};


gameOfLife3D::lifeSimulator::LifeSimulatorCPU::LifeSimulatorCPU() :
    m_currentIndex(0),
    m_nextIndex(1)
{
}

gameOfLife3D::lifeSimulator::LifeSimulatorCPU::~LifeSimulatorCPU()
{
}

HRESULT gameOfLife3D::lifeSimulator::LifeSimulatorCPU::Initialize( graphics::D3DInteropHelper *pD3DInteropHelper )
{
    UNREFERENCED_PARAMETER(pD3DInteropHelper);
    for (size_t bufIndex = 0; bufIndex < NUMBER_OF_BUFFERS; ++bufIndex) {
        m_buffer[bufIndex].resize(GetMaxNumberOfLifeData());
        for (UINT y = 0; y < GetHeight(); ++y) {
            for (UINT x = 0; x < GetWidth(); ++x) {
                UINT index = x + y * GetWidth();
                if (bufIndex == m_currentIndex) {
                    m_buffer[bufIndex].at(index) = GetInitialData()[index];
                } else {
                    gameOfLife3D::lifeSimulator::LifeData data;
                    ZeroMemory(&data, sizeof(data));
                    m_buffer[bufIndex].at(index) = data;
                }
            }
        }
    }
    return S_OK;
}

HRESULT gameOfLife3D::lifeSimulator::LifeSimulatorCPU::Compute()
{
    UINT start = 0;
    UINT end = GetMaxNumberOfLifeData();

    // PPL (Parallel Pattern Library)
    Concurrency::parallel_for(start, end, [&](UINT tid) {
        int cx = tid % GetWidth();
        int cy = tid / GetWidth();

        int sum = 0;
        XMFLOAT3 colorSum = XMFLOAT3(0.0f, 0.0f, 0.0f);
        for (int i = 0; i < 8; ++i) {
            int x = cx + g_sumIndex[i][0];
            int y = cy + g_sumIndex[i][1];
            if (0 <= x && x < static_cast<int>(GetWidth()) && 0 <= y && y < static_cast<int>(GetWidth())) {
                int id = x + y * GetWidth();
                if (m_buffer[m_currentIndex][id].alive != 0) {
                    sum++;
                    colorSum.x += m_buffer[m_currentIndex][id].color.x;
                    colorSum.y += m_buffer[m_currentIndex][id].color.y;
                    colorSum.z += m_buffer[m_currentIndex][id].color.z;
                }
            }
        }
        colorSum.x /= sum;
        colorSum.y /= sum;
        colorSum.z /= sum;

        if (m_buffer[m_currentIndex][tid].alive == 0 &&
        sum >= static_cast<int>(GetR1()) && sum <= static_cast<int>(GetR2())) {
            m_buffer[m_nextIndex][tid].alive = 1;
            m_buffer[m_nextIndex][tid].color = XMFLOAT3(1.0f, 1.0f, 1.0f);
        } else if (m_buffer[m_currentIndex][tid].alive != 0 &&
        (sum > static_cast<int>(GetR3()) || sum < static_cast<int>(GetR4()))) {
            m_buffer[m_nextIndex][tid].alive = 0;
            m_buffer[m_nextIndex][tid].color = XMFLOAT3(0.0f, 0.0f, 0.0f);
        } else {
            m_buffer[m_nextIndex][tid].alive = m_buffer[m_currentIndex][tid].alive;
            m_buffer[m_nextIndex][tid].color = m_buffer[m_currentIndex][tid].color;
        }
    });

    m_currentIndex = (m_currentIndex + 1) % 2;
    m_nextIndex = (m_currentIndex + 1) % 2;
    return S_OK;
}

void gameOfLife3D::lifeSimulator::LifeSimulatorCPU::CopyCurrent( OUT gameOfLife3D::lifeSimulator::LifeData *pLifeData )
{
    for (UINT i = 0; i < m_buffer[m_currentIndex].size(); ++i) {
        pLifeData[i] = m_buffer[m_currentIndex][i];
    }
}

void gameOfLife3D::lifeSimulator::LifeSimulatorCPU::CopyNext( OUT gameOfLife3D::lifeSimulator::LifeData *pLifeData )
{
    for (UINT i = 0; i < m_buffer[m_currentIndex].size(); ++i) {
        pLifeData[i] = m_buffer[m_currentIndex][i];
    }
}

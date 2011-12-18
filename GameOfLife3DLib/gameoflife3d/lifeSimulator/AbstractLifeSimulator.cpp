#include "stdafx.h"
#include "Common.h"
#include "gameOfLife3D/lifeSimulator/AbstractLifeSimulator.h"

gameOfLife3D::lifeSimulator::AbstractLifeSimulator::AbstractLifeSimulator() :
    m_width(32),
    m_height(32),
    m_r1(3),
    m_r2(3),
    m_r3(3),
    m_r4(2),
    m_initialData(32 * 32)
{
}

gameOfLife3D::lifeSimulator::AbstractLifeSimulator::~AbstractLifeSimulator()
{
}

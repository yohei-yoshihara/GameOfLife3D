#include "stdafx.h"
#include "FPSUtil.h"

graphics::FPSUtil::FPSUtil() :
    m_ticksPerSec(0),
    m_frames(0)
{
    LARGE_INTEGER qwTicksPerSec = { 0 };
    QueryPerformanceFrequency( &qwTicksPerSec );
    m_ticksPerSec = qwTicksPerSec.QuadPart;
    m_lastTime = GetCurrentTime();
}

graphics::FPSUtil::~FPSUtil()
{
}

void graphics::FPSUtil::UpdateFPS( std::wstring &fps, bool &updated )
{
    updated = false;
    ++m_frames;
    double currentTime = GetCurrentTime();
    if (currentTime - m_lastTime > 1.0f) {
        m_fps = (float) (m_frames / (currentTime - m_lastTime));
        m_frames = 0;
        m_lastTime = currentTime;
        ZeroMemory(m_sFps, FPS_BUFFER_SIZE);
        swprintf(m_sFps, FPS_BUFFER_SIZE, L"FPS %3.1f", m_fps);
        updated = true;
    }
    fps = m_sFps;
}

double graphics::FPSUtil::GetCurrentTime() const
{
    LARGE_INTEGER qwTime = { 0 };
    QueryPerformanceCounter( &qwTime );
    double fTime = qwTime.QuadPart / ( double )m_ticksPerSec;
    return fTime;
}

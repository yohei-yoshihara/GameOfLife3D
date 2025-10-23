#ifndef GRAPHICS_FPSUTIL_H_
#define GRAPHICS_FPSUTIL_H_

namespace graphics {
#define FPS_BUFFER_SIZE 16
  class FPSUtil {
  private:
    LONGLONG m_ticksPerSec;
    UINT m_frames;
    double m_lastTime;
    float m_fps;
    wchar_t m_sFps[FPS_BUFFER_SIZE];

  public:
    FPSUtil();
    ~FPSUtil();
    double GetCurrentTime() const;
    void UpdateFPS(std::wstring &fps, bool &updated);
    void GetFPS(std::wstring &fps) {
      fps = m_sFps;
    }
  };
}

#endif // GRAPHICS_FPSUTIL_H_

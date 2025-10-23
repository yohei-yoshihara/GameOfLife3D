#ifndef GRAPHICS_COLOR_GRADIENTCOLORSET_H_
#define GRAPHICS_COLOR_GRADIENTCOLORSET_H_

#include "graphics/color/GradientStop.h"
#include "graphics/GraphicsCommon.h"

namespace graphics {
  namespace color {
    class GradientStopCollection {
    private:
      std::vector<GradientStop> m_gradientStops;
      GRADIENT_DIRECTION m_gradientDirection;

    public:
      GradientStopCollection(void)
          : m_gradientStops()
          , m_gradientDirection(GRADIENT_DIRECTION_TOP_BOTTOM) {
      }
      GradientStopCollection(const GradientStopCollection &other)
          : m_gradientStops(other.m_gradientStops)
          , m_gradientDirection(other.m_gradientDirection) {
      }
      GradientStopCollection(GradientStopCollection &&other)
          : m_gradientStops(std::move(other.m_gradientStops))
          , m_gradientDirection(other.m_gradientDirection) {
      }
      ~GradientStopCollection(void) {
      }
      GradientStopCollection &operator=(const GradientStopCollection &other) {
        if (this != &other) {
          m_gradientStops = other.m_gradientStops;
          m_gradientDirection = other.m_gradientDirection;
        }
        return *this;
      }
      GradientStopCollection &operator=(GradientStopCollection &&other) {
        if (this != &other) {
          m_gradientStops = std::move(other.m_gradientStops);
          m_gradientDirection = other.m_gradientDirection;
        }
        return *this;
      }
      bool operator==(const GradientStopCollection &other) const {
        return m_gradientStops.size() == other.m_gradientStops.size()
               && std::equal(m_gradientStops.begin(), m_gradientStops.end(), other.m_gradientStops.begin())
               && m_gradientDirection == other.m_gradientDirection;
      }
      bool operator!=(const GradientStopCollection &other) const {
        return !(*this == other);
      }
      void AddGradientStop(const GradientStop &stop) {
        m_gradientStops.push_back(stop);
      }
      void RemoveGradientStop(const GradientStop &stop) {
        m_gradientStops.erase(std::remove(m_gradientStops.begin(), m_gradientStops.end(), stop), m_gradientStops.end());
      }
      size_t GetNumberOfStops() const {
        return m_gradientStops.size();
      }
      const GradientStop &GetGradientStop(size_t index) const {
        return m_gradientStops.at(index);
      }
      GradientStop &GetGradientStop(size_t index) {
        return m_gradientStops.at(index);
      }
      void SetGradientDirection(GRADIENT_DIRECTION gradientDirection) {
        m_gradientDirection = gradientDirection;
      }
      GRADIENT_DIRECTION GetGradientDirection() const {
        return m_gradientDirection;
      }
      void ToD2D1_GRADIENT_STOP(OUT std::vector<D2D1_GRADIENT_STOP> &gradientStops) const {
        gradientStops.clear();
        std::for_each(m_gradientStops.begin(), m_gradientStops.end(), [&](const GradientStop &stop) {
          gradientStops.push_back(D2D1::GradientStop(stop.GetPosition(), stop.GetColorValue().ToD2D1_COLOR_F()));
        });
      }
      friend std::wostream &operator<<(std::wostream &os, const GradientStopCollection &collection) {
        os << typeid(collection).name() << L"[gradientStops[" << collection.m_gradientStops.size() << L"]{";
        for (size_t i = 0; i < collection.m_gradientStops.size(); ++i) {
          os << collection.m_gradientStops[i];
          if (i != (collection.m_gradientStops.size() - 1)) {
            os << L",";
          }
        }
        os << L"}]";
        return os;
      }
    };
  }
}
#endif // GRAPHICS_COLOR_GRADIENTCOLORSET_H_
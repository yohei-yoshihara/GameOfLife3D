#ifndef GRAPHICS_COLOR_GRADIENTSTOP_H_
#define GRAPHICS_COLOR_GRADIENTSTOP_H_

#include "graphics/color/ColorValue.h"

namespace graphics {
  namespace color {
    class GradientStop {
    private:
      FLOAT m_position;
      ColorValue m_color;

    public:
      GradientStop(void)
          : m_position(0.0f)
          , m_color() {
      }
      GradientStop(FLOAT position, ColorValue color)
          : m_position(position)
          , m_color(color) {
      }
      GradientStop(const GradientStop &other)
          : m_position(other.m_position)
          , m_color(other.m_color) {
      }
      GradientStop(GradientStop &&other)
          : m_position(other.m_position)
          , m_color(std::move(other.m_color)) {
      }
      ~GradientStop(void) {
      }
      GradientStop &operator=(const GradientStop &other) {
        if (this != &other) {
          m_position = other.m_position;
          m_color = other.m_color;
        }
        return *this;
      }
      GradientStop &operator=(GradientStop &&other) {
        if (this != &other) {
          m_position = other.m_position;
          m_color = std::move(other.m_color);
        }
        return *this;
      }
      bool operator==(const GradientStop &other) const {
        return m_position == other.m_position && m_color == other.m_color;
      }
      bool operator!=(const GradientStop &other) const {
        return !(*this == other);
      }
      void SetPosition(FLOAT position) {
        m_position = position;
      }
      FLOAT GetPosition() const {
        return m_position;
      }
      void SetColorValue(const ColorValue &color) {
        m_color = color;
      }
      const ColorValue &GetColorValue() const {
        return m_color;
      }
      ColorValue &GetColorValue() {
        return m_color;
      }
      friend std::wostream &operator<<(std::wostream &os, const GradientStop &gradientStop) {
        os << typeid(gradientStop).name() << L"[position=" << gradientStop.m_position << L",color="
           << gradientStop.m_color << L"]";
        return os;
      }
    };
  }
}

#endif // GRAPHICS_COLOR_GRADIENTSTOP_H_
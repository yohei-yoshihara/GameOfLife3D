#ifndef GRAPHICS_COLOR_SOLIDCOLOR_H_
#define GRAPHICS_COLOR_SOLIDCOLOR_H_

#include "graphics/color/IColor.h"
#include "graphics/color/ColorValue.h"

namespace graphics {
  namespace color {
    class SolidColor : public graphics::color::IColor {
    private:
      std::wstring m_label;
      graphics::color::ColorValue m_originalColor;
      graphics::color::ColorValue m_currentColor;

    public:
      SolidColor(void)
          : m_label()
          , m_originalColor()
          , m_currentColor() {
      }
      SolidColor(const graphics::color::ColorValue &color)
          : m_label()
          , m_originalColor(color)
          , m_currentColor(color) {
      }
      virtual ~SolidColor() {
      }
      void SetLabel(const std::wstring &label) {
        m_label = label;
      }
      virtual const std::wstring &GetLabel() const {
        return m_label;
      }
      void SetColor(const ColorValue &color) {
        m_originalColor = color;
        m_currentColor = color;
      }
      const ColorValue &GetOriginalColor() const {
        return m_originalColor;
      }
      const ColorValue &GetCurrentColor() const {
        return m_currentColor;
      }

      // interface IColor
      virtual COLOR_TYPE GetColorType() const {
        return graphics::color::IColor::COLOR_TYPE_SOLID_COLOR;
      }
      virtual void Brighter(FLOAT rate) {
        m_currentColor = m_currentColor.CloneAndBrighter(rate);
      }
      virtual void Darker(FLOAT rate) {
        m_currentColor = m_currentColor.CloneAndDarker(rate);
      }
      virtual void SetAlpha(FLOAT alpha) {
        m_currentColor = m_currentColor.CloneAndChangeAlpha(alpha);
      }
      virtual void Reset() {
        m_currentColor = m_originalColor;
      }
      friend std::wostream &operator<<(std::wostream &os, SolidColor const &solidColor);
    };
  }
}

#endif // GRAPHICS_COLOR_SOLIDCOLOR_H_
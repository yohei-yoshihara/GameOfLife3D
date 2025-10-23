#ifndef GRAPHICS_COLOR_LINEARGRADIENTCOLOR_H_
#define GRAPHICS_COLOR_LINEARGRADIENTCOLOR_H_

#include "graphics/color/IColor.h"
#include "graphics/color/GradientStopCollection.h"

namespace graphics {
  namespace color {
    class LinearGradientColor : public graphics::color::IColor {
    private:
      std::wstring m_label;
      graphics::color::GradientStopCollection m_originalStopCollection;
      graphics::color::GradientStopCollection m_currentStopCollection;
      graphics::GRADIENT_DIRECTION m_gradientDirection;

    public:
      LinearGradientColor(void);
      virtual ~LinearGradientColor(void);
      void SetLabel(const std::wstring &label) {
        m_label = label;
      }
      virtual const std::wstring &GetLabel() const {
        return m_label;
      }
      void SetColor(const graphics::color::GradientStopCollection &stopCollection) {
        m_originalStopCollection = stopCollection;
        m_currentStopCollection = stopCollection;
      }
      void SetGradientDirection(graphics::GRADIENT_DIRECTION gradientDirection) {
        m_gradientDirection = gradientDirection;
      }
      const graphics::color::GradientStopCollection &GetOriginalColor() const {
        return m_originalStopCollection;
      }
      const graphics::color::GradientStopCollection &GetCurrentColor() const {
        return m_currentStopCollection;
      }
      graphics::GRADIENT_DIRECTION GetGradientDirection() const {
        return m_gradientDirection;
      }

      // interface IColor
      virtual COLOR_TYPE GetColorType() const {
        return graphics::color::IColor::COLOR_TYPE_LINEAR_GRADIENT_COLOR;
      }
      virtual void Brighter(FLOAT rate);
      virtual void Darker(FLOAT rate);
      virtual void SetAlpha(FLOAT alpha);
      virtual void Reset();
    };
  }
}

#endif // GRAPHICS_COLOR_LINEARGRADIENTCOLOR_H_

#ifndef GRAPHICS_COLOR_RADIENTGRADIENTCOLOR_H_
#define GRAPHICS_COLOR_RADIENTGRADIENTCOLOR_H_

#include "graphics/color/IColor.h"
#include "graphics/color/GradientStopCollection.h"

namespace graphics
{
namespace color
{
class RadialGradientColor :
    public graphics::color::IColor
{
private:
    std::wstring m_label;
    graphics::color::GradientStopCollection m_originalStopCollection;
    graphics::color::GradientStopCollection m_currentStopCollection;
    FLOAT m_gradientOffsetX;
    FLOAT m_gradientOffsetY;

public:
    RadialGradientColor(void);
    virtual ~RadialGradientColor(void);
    void SetLabel(const std::wstring &label) {
        m_label = label;
    }
    virtual const std::wstring& GetLabel() const {
        return m_label;
    }
    void SetColor(const graphics::color::GradientStopCollection& stopCollection) {
        m_originalStopCollection = stopCollection;
        m_currentStopCollection = stopCollection;
    }
    void SetGradientOffset(FLOAT gradientOffsetX, FLOAT gradientOffsetY) {
        m_gradientOffsetX = gradientOffsetX;
        m_gradientOffsetY = gradientOffsetY;
    }
    const graphics::color::GradientStopCollection& GetOriginalColor() const {
        return m_originalStopCollection;
    }
    const graphics::color::GradientStopCollection& GetCurrentColor() const {
        return m_currentStopCollection;
    }
    FLOAT GetGradientOffsetX() const {
        return m_gradientOffsetX;
    }
    FLOAT GetGradientOffsetY() const {
        return m_gradientOffsetY;
    }

    // interface IColor
    virtual COLOR_TYPE GetColorType() const {
        return graphics::color::IColor::COLOR_TYPE_RADIAL_GRADIENT_COLOR;
    }
    virtual void Brighter(FLOAT rate);
    virtual void Darker(FLOAT rate);
    virtual void SetAlpha(FLOAT alpha);
    virtual void Reset();
};
}
}

#endif // GRAPHICS_COLOR_RADIENTGRADIENTCOLOR_H_

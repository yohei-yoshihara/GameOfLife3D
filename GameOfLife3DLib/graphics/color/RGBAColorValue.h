#ifndef GRAPHICS_COLOR_RGBCOLORVALUE_H_
#define GRAPHICS_COLOR_RGBCOLORVALUE_H_

namespace graphics
{
namespace color
{
class HSBAColorValue;
class ColorValue;
class RGBAColorValue
{
private:
    FLOAT m_red;
    FLOAT m_green;
    FLOAT m_blue;
    FLOAT m_alpha;

public:
    RGBAColorValue(void) :
        m_red(0.0f), m_green(0.0f), m_blue(0.0f), m_alpha(0.0f)
    {}
    RGBAColorValue(FLOAT red, FLOAT green, FLOAT blue, FLOAT alpha) :
        m_red(red), m_green(green), m_blue(blue), m_alpha(alpha)
    {}
    RGBAColorValue(const RGBAColorValue& other) :
        m_red(other.m_red), m_green(other.m_green), m_blue(other.m_blue), m_alpha(other.m_alpha)
    {}
    explicit RGBAColorValue(D2D1_COLOR_F color) :
        m_red(color.r), m_green(color.g), m_blue(color.b), m_alpha(color.a)
    {}
    explicit RGBAColorValue(COLORREF color) :
        m_red(GetRValue(color) / 255.0f),
        m_green(GetGValue(color) / 255.0f),
        m_blue(GetBValue(color) / 255.0f),
        m_alpha(1.0f)
    {}
    explicit RGBAColorValue(const HSBAColorValue &hsbaColorValue);
    explicit RGBAColorValue(const ColorValue &colorValue);
    ~RGBAColorValue(void) {}
    RGBAColorValue& operator=(const RGBAColorValue& other) {
        if (this != &other) {
            m_red = other.m_red;
            m_green = other.m_green;
            m_blue = other.m_blue;
            m_alpha = other.m_alpha;
        }
        return *this;
    }
    bool operator==(const RGBAColorValue& other) const {
        return m_red == other.m_red &&
               m_green == other.m_green &&
               m_blue == other.m_blue &&
               m_alpha == other.m_alpha;
    }
    bool operator!=(const RGBAColorValue& other) const {
        return !(*this == other);
    }
    void SetRed(FLOAT red) {
        m_red = red;
    }
    FLOAT GetRed() const {
        return m_red;
    }
    void SetGreen(FLOAT green) {
        m_green = green;
    }
    FLOAT GetGreen() const {
        return m_green;
    }
    void SetBlue(FLOAT blue) {
        m_blue = blue;
    }
    FLOAT GetBlue() const {
        return m_blue;
    }
    void SetAlpha(FLOAT alpha) {
        m_alpha = alpha;
    }
    FLOAT GetAlpha() const {
        return m_alpha;
    }
    friend std::wostream& operator<<(std::wostream& os, const RGBAColorValue &rgbaColorValue) {
        os << typeid(rgbaColorValue).name() << L"[red=" << rgbaColorValue.GetRed()
           << L",green=" << rgbaColorValue.GetGreen()
           << L",blue=" << rgbaColorValue.GetBlue()
           << L",alpha=" << rgbaColorValue.GetAlpha()
           << L"]";
        return os;
    }
};
}
}

#endif // GRAPHICS_COLOR_RGBCOLORVALUE_H_
#ifndef GRAPHICS_COLOR_HSVCOLORVALUE_H_
#define GRAPHICS_COLOR_HSVCOLORVALUE_H_

namespace graphics
{
namespace color
{
class RGBAColorValue;
class ColorValue;
class HSBAColorValue
{
private:
    FLOAT m_hue;
    FLOAT m_saturation;
    FLOAT m_brightness;
    FLOAT m_alpha;

public:
    HSBAColorValue(void) :
        m_hue(0.0f), m_saturation(0.0f), m_brightness(0.0f), m_alpha(0.0f)
    {}
    HSBAColorValue(FLOAT hue, FLOAT saturation, FLOAT brightness, FLOAT alpha) :
        m_hue(hue), m_saturation(saturation), m_brightness(brightness), m_alpha(alpha)
    {}
    HSBAColorValue(const HSBAColorValue& other) :
        m_hue(other.m_hue),
        m_saturation(other.m_saturation),
        m_brightness(other.m_brightness),
        m_alpha(other.m_alpha)
    {}
    explicit HSBAColorValue(const RGBAColorValue &rgbaColorValue);
    explicit HSBAColorValue(const ColorValue &colorValue);
    ~HSBAColorValue(void) {}
    HSBAColorValue& operator=(const HSBAColorValue& other) {
        if (this != &other) {
            m_hue = other.m_hue;
            m_saturation = other.m_saturation;
            m_brightness = other.m_brightness;
            m_alpha = other.m_alpha;
        }
        return *this;
    }
    bool operator==(const HSBAColorValue& other) const {
        return m_hue == other.m_hue &&
               m_saturation == other.m_saturation &&
               m_brightness == other.m_brightness &&
               m_alpha == other.m_alpha;
    }
    bool operator!=(const HSBAColorValue& other) const {
        return !(*this == other);
    }
    void SetHue(FLOAT hue) {
        m_hue = hue;
    }
    FLOAT GetHue() const {
        return m_hue;
    }
    void SetSaturation(FLOAT saturation) {
        m_saturation = saturation;
    }
    FLOAT GetSaturation() const {
        return m_saturation;
    }
    void SetBrightness(FLOAT brightness) {
        m_brightness = brightness;
    }
    FLOAT GetBrightness() const {
        return m_brightness;
    }
    void SetAlpha(FLOAT alpha) {
        m_alpha = alpha;
    }
    FLOAT GetAlpha() const {
        return m_alpha;
    }
    void Brighter(FLOAT rate) {
        m_brightness = m_brightness * (1.0f + rate);
    }
    void Darker(FLOAT rate) {
        m_brightness = m_brightness * (1.0f - rate);
    }
    friend std::wostream& operator<<(std::wostream& os, const HSBAColorValue &hsbaColorValue) {
        os << typeid(hsbaColorValue).name() << L"[hue=" << hsbaColorValue.GetHue()
           << L",saturation=" << hsbaColorValue.GetSaturation()
           << L",brightness=" << hsbaColorValue.GetBrightness()
           << L",alpha=" << hsbaColorValue.GetAlpha()
           << L"]";
        return os;
    }
};
}
}

#endif // GRAPHICS_COLOR_HSVCOLORVALUE_H_
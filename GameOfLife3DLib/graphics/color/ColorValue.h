#ifndef GRAPHICS_COLOR_COLORVALUE_H_
#define GRAPHICS_COLOR_COLORVALUE_H_

#include "graphics/color/ColorUtil.h"
#include "graphics/color/RGBAColorValue.h"
#include "graphics/color/HSBAColorValue.h"

namespace graphics {
  namespace color {
    inline FLOAT saturate(FLOAT v, FLOAT l) {
      return v > l ? l : v;
    }

    class ColorValue {
    public:
      enum COLOR_TYPE {
        COLOR_TYPE_RGBA = 0,
        COLOR_TYPE_HSBA = 1,
      };

    private:
      COLOR_TYPE m_colorType;
      FLOAT m_value1; // red or hue
      FLOAT m_value2; // green or saturation
      FLOAT m_value3; // blue or brightness
      FLOAT m_alpha;

    public:
      ColorValue(void)
          : m_colorType(COLOR_TYPE_RGBA)
          , m_value1(0.0f)
          , m_value2(0.0f)
          , m_value3(0.0f)
          , m_alpha(0.0f) {
      }
      ColorValue(COLOR_TYPE colorType)
          : m_colorType(colorType)
          , m_value1(0.0f)
          , m_value2(0.0f)
          , m_value3(0.0f)
          , m_alpha(0.0f) {
      }
      ColorValue(COLOR_TYPE colorType, unsigned int value, FLOAT alpha)
          : m_colorType(colorType)
          , m_value1(0.0f)
          , m_value2(0.0f)
          , m_value3(0.0f)
          , m_alpha(alpha) {
        if (colorType == COLOR_TYPE_RGBA) {
          m_value1 = ((value & 0xff0000) >> 16) / 255.0f;
          m_value2 = ((value & 0xff00) >> 8) / 255.0f;
          m_value3 = (value & 0xff) / 255.0f;
        } else {
          m_value1 = ((value & 0xff0000) >> 16) / 255.0f * 360.0f;
          m_value2 = ((value & 0xff00) >> 8) / 255.0f;
          m_value3 = (value & 0xff) / 255.0f;
        }
      }
      ColorValue(COLOR_TYPE colorType, FLOAT value1, FLOAT value2, FLOAT value3, FLOAT alpha)
          : m_colorType(colorType)
          , m_value1(value1)
          , m_value2(value2)
          , m_value3(value3)
          , m_alpha(alpha) {
      }
      ColorValue(const ColorValue &colorValue)
          : m_colorType(colorValue.m_colorType)
          , m_value1(colorValue.m_value1)
          , m_value2(colorValue.m_value2)
          , m_value3(colorValue.m_value3)
          , m_alpha(colorValue.m_alpha) {
      }
      ColorValue &operator=(const ColorValue &colorValue) {
        if (this != &colorValue) {
          m_colorType = colorValue.m_colorType;
          m_value1 = colorValue.m_value1;
          m_value2 = colorValue.m_value2;
          m_value3 = colorValue.m_value3;
          m_alpha = colorValue.m_alpha;
        }
        return *this;
      }
      ~ColorValue(void) {
      }
      bool operator==(const ColorValue &colorValue) const {
        return m_colorType == colorValue.m_colorType && m_value1 == colorValue.m_value1
               && m_value2 == colorValue.m_value2 && m_value3 == colorValue.m_value3 && m_alpha == colorValue.m_alpha;
      }
      bool operator!=(const ColorValue &other) const {
        return !(*this == other);
      }
      void SetColorType(COLOR_TYPE colorType) {
        m_colorType = colorType;
      }
      COLOR_TYPE GetColorType() const {
        return m_colorType;
      }
      void SetRGBA(FLOAT red, FLOAT green, FLOAT blue, FLOAT alpha) {
        if (m_colorType == COLOR_TYPE_RGBA) {
          m_value1 = red;
          m_value2 = green;
          m_value3 = blue;
          m_alpha = alpha;
        } else {
          ColorUtil::RgbToHsv(red, green, blue, m_value1, m_value2, m_value3);
          m_alpha = alpha;
        }
      }
      void SetRGBA(RGBAColorValue rgba) {
        SetRGBA(rgba.GetRed(), rgba.GetGreen(), rgba.GetBlue(), rgba.GetAlpha());
      }
      RGBAColorValue GetRGBA() const {
        if (m_colorType == COLOR_TYPE_RGBA) {
          return RGBAColorValue(m_value1, m_value2, m_value3, m_alpha);
        } else {
          FLOAT red, green, blue;
          ColorUtil::HsvToRgb(m_value1, m_value2, m_value3, red, green, blue);
          return RGBAColorValue(red, green, blue, m_alpha);
        }
      }
      void SetHSBA(FLOAT hue, FLOAT saturation, FLOAT brightness, FLOAT alpha) {
        if (m_colorType == COLOR_TYPE_RGBA) {
          ColorUtil::HsvToRgb(hue, saturation, brightness, m_value1, m_value2, m_value3);
          m_alpha = alpha;
        } else {
          m_value1 = hue;
          m_value2 = saturation;
          m_value3 = brightness;
          m_alpha = alpha;
        }
      }
      void SetHSBA(HSBAColorValue hsva) {
        SetHSBA(hsva.GetHue(), hsva.GetSaturation(), hsva.GetBrightness(), hsva.GetAlpha());
      }
      HSBAColorValue GetHSBA() const {
        if (m_colorType == COLOR_TYPE_RGBA) {
          FLOAT hue, saturation, brightness;
          ColorUtil::RgbToHsv(m_value1, m_value2, m_value3, hue, saturation, brightness);
          return HSBAColorValue(hue, saturation, brightness, m_alpha);
        } else {
          return HSBAColorValue(m_value1, m_value2, m_value3, m_alpha);
        }
      }
      FLOAT GetRed() const {
        return GetRGBA().GetRed();
      }
      FLOAT GetGreen() const {
        return GetRGBA().GetGreen();
      }
      FLOAT GetBlue() const {
        return GetRGBA().GetBlue();
      }
      FLOAT GetHue() const {
        return GetHSBA().GetHue();
      }
      FLOAT GetSaturation() const {
        return GetHSBA().GetSaturation();
      }
      FLOAT GetBrightness() const {
        return GetHSBA().GetBrightness();
      }
      FLOAT GetAlpha() const {
        return m_alpha;
      }
      ColorValue Clone(void) const {
        return ColorValue(m_colorType, m_value1, m_value2, m_value3, m_alpha);
      }
      ColorValue CloneAndBrighter(FLOAT rate) const {
        return ColorValue(COLOR_TYPE_HSBA, GetHue(), GetSaturation(), saturate(GetBrightness() * (1.0f + rate), 1.0f),
                          GetAlpha());
      }
      ColorValue CloneAndDarker(FLOAT rate) const {
        return ColorValue(COLOR_TYPE_HSBA, GetHue(), GetSaturation(), saturate(GetBrightness() * (1.0f - rate), 1.0f),
                          GetAlpha());
      }
      ColorValue CloneAndChangeBrightness(FLOAT brightness) const {
        return ColorValue(COLOR_TYPE_HSBA, GetHue(), GetSaturation(), brightness, GetAlpha());
      }
      ColorValue CloneAndChangeAlpha(FLOAT alpha) const {
        return ColorValue(COLOR_TYPE_HSBA, GetHue(), GetSaturation(), GetBrightness(), alpha);
      }
      void Brighter(FLOAT rate) {
        if (m_colorType == COLOR_TYPE_RGBA) {
          HSBAColorValue hsba = GetHSBA();
          hsba.Brighter(rate);
          SetHSBA(hsba);
        } else {
          m_value3 = m_value3 * (1.0f + rate);
        }
      }
      void Darker(FLOAT rate) {
        if (m_colorType == COLOR_TYPE_RGBA) {
          HSBAColorValue hsba = GetHSBA();
          hsba.Darker(rate);
          SetHSBA(hsba);
        } else {
          m_value3 = m_value3 * (1.0f - rate);
        }
      }
      void SetAlpha(FLOAT alpha) {
        m_alpha = alpha;
      }
      D2D1_COLOR_F ToD2D1_COLOR_F() const {
        return D2D1::ColorF(GetRed(), GetGreen(), GetBlue(), GetAlpha());
      }

      friend std::wostream &operator<<(std::wostream &os, ColorValue const &colorValue) {
        os << typeid(colorValue).name();
        switch (colorValue.m_colorType) {
        case graphics::color::ColorValue::COLOR_TYPE_RGBA:
          os << L"[colorType=RGBA,red=" << colorValue.m_value1 << L",green=" << colorValue.m_value2 << L",blue="
             << colorValue.m_value3 << L",alpha=" << colorValue.m_alpha << L"]";
          break;
        case graphics::color::ColorValue::COLOR_TYPE_HSBA:
          os << L"[colorType=HSBA,hue=" << colorValue.m_value1 << L",saturation=" << colorValue.m_value2
             << L",brightness=" << colorValue.m_value3 << L",alpha=" << colorValue.m_alpha << L"]";
          break;
        default:
          throw std::runtime_error("unknown color type");
        }
        return os;
      }
      friend std::ostream &operator<<(std::ostream &os, ColorValue const &colorValue) {
        os << typeid(colorValue).name();
        switch (colorValue.m_colorType) {
        case graphics::color::ColorValue::COLOR_TYPE_RGBA:
          os << "[colorType=RGBA,red=" << colorValue.m_value1 << ",green=" << colorValue.m_value2
             << ",blue=" << colorValue.m_value3 << ",alpha=" << colorValue.m_alpha << "]";
          break;
        case graphics::color::ColorValue::COLOR_TYPE_HSBA:
          os << "[colorType=HSBA,hue=" << colorValue.m_value1 << ",saturation=" << colorValue.m_value2
             << ",brightness=" << colorValue.m_value3 << ",alpha=" << colorValue.m_alpha << "]";
          break;
        default:
          throw std::runtime_error("unknown color type");
        }
        return os;
      }
    };
  }
}

#endif // GRAPHICS_COLOR_COLORVALUE_H_
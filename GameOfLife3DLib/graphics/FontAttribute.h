#ifndef GRAPHICS_FONTATTRIBUTE_H_
#define GRAPHICS_FONTATTRIBUTE_H_

namespace graphics {
  class FontAttribute {
  public:
    FontAttribute(void)
        : fontFamilyName(L"Arial")
        , fontSize(14.0f)
        , bold(false)
        , italic(false) {
    }
    FontAttribute(FLOAT _fontSize)
        : fontFamilyName(L"Arial")
        , fontSize(_fontSize)
        , bold(false)
        , italic(false) {
    }
    FontAttribute(const FontAttribute &other)
        : fontFamilyName(other.fontFamilyName)
        , fontSize(other.fontSize)
        , bold(other.bold)
        , italic(other.italic) {
    }
    FontAttribute(FontAttribute &&other)
        : fontFamilyName(std::move(other.fontFamilyName))
        , fontSize(other.fontSize)
        , bold(other.bold)
        , italic(other.italic) {
    }
    FontAttribute(const std::wstring &_fontFamilyName, FLOAT _fontSize, bool _bold, bool _italic)
        : fontFamilyName(_fontFamilyName)
        , fontSize(_fontSize)
        , bold(_bold)
        , italic(_italic) {
    }
    ~FontAttribute(void) {
    }
    FontAttribute &operator=(const FontAttribute &fontAttribute) {
      if (this != &fontAttribute) {
        fontFamilyName = fontAttribute.fontFamilyName;
        fontSize = fontAttribute.fontSize;
        bold = fontAttribute.bold;
        italic = fontAttribute.italic;
      }
      return *this;
    }
    FontAttribute &operator=(FontAttribute &&fontAttribute) {
      if (this != &fontAttribute) {
        fontFamilyName = std::move(fontAttribute.fontFamilyName);
        fontSize = fontAttribute.fontSize;
        bold = fontAttribute.bold;
        italic = fontAttribute.italic;
      }
      return *this;
    }
    std::wstring fontFamilyName;
    FLOAT fontSize;
    bool bold;
    bool italic;
    DWRITE_FONT_WEIGHT GetFontWeight() const {
      return bold ? DWRITE_FONT_WEIGHT_BOLD : DWRITE_FONT_WEIGHT_NORMAL;
    }
    DWRITE_FONT_STYLE GetFontStyle() const {
      return italic ? DWRITE_FONT_STYLE_OBLIQUE : DWRITE_FONT_STYLE_NORMAL;
    }
    DWRITE_FONT_STRETCH GetFontStretch() const {
      return DWRITE_FONT_STRETCH_NORMAL;
    }
    friend std::wostream &operator<<(std::wostream &strm, FontAttribute const &fontAttribute);
  };
}

#endif // GRAPHICS_FONTATTRIBUTE_H_
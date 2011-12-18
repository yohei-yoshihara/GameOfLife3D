#include "Includes.h"
#include "ColorSetDef.h"
#include "ColorSetDefLexer.h"
#include "ColorSetDefParser.h"

#ifdef _DEBUG
#pragma comment(lib, "antlr3cd.lib")
#else
#pragma comment(lib, "antlr3c.lib")
#endif

std::shared_ptr<graphics::color::colorsetdef::ColorSetDO> graphics::color::colorsetdef::Decode(const std::wstring &s)
{
    pANTLR3_UINT8 input_string = (pANTLR3_UINT8)s.c_str();
    ANTLR3_UINT32 size = static_cast<ANTLR3_UINT32>(s.size() * 2);
    pANTLR3_INPUT_STREAM stream = antlr3StringStreamNew(
                                      input_string,
                                      ANTLR3_ENC_UTF16,
                                      size,
                                      (pANTLR3_UINT8)"colorsetdef");

    pANTLR3_UINT8 fName;
    pANTLR3_COMMON_TOKEN_STREAM tstream;

    pColorSetDefLexer lxr = ColorSetDefLexerNew(stream);
    tstream = antlr3CommonTokenStreamSourceNew(ANTLR3_SIZE_HINT, TOKENSOURCE(lxr));
    pColorSetDefParser psr = ColorSetDefParserNew(tstream);

    std::shared_ptr<graphics::color::colorsetdef::ColorSetDO> pColorSetDO(psr->colorSetDef(psr));
    if (pColorSetDO != nullptr) {
        size_t n = pColorSetDO->colors.size();
        std::wcout << n << std::endl;
        for (size_t i = 0; i < n; ++i) {
            std::shared_ptr<graphics::color::colorsetdef::IColorDO> color = pColorSetDO->colors[i];
            switch (color->GetColorType()) {
            case COLOR_TYPE_SOLID: {
                std::shared_ptr<graphics::color::colorsetdef::SolidColorDO> solid =
                    std::dynamic_pointer_cast<graphics::color::colorsetdef::SolidColorDO>(color);
                std::wcout << *solid << std::endl;
            }
            break;
            case COLOR_TYPE_LINEAR: {
                std::shared_ptr<graphics::color::colorsetdef::LinearColorDO> linear =
                    std::dynamic_pointer_cast<graphics::color::colorsetdef::LinearColorDO>(color);
                std::wcout << *linear << std::endl;
            }
            break;
            case COLOR_TYPE_RADIAL: {
                std::shared_ptr<graphics::color::colorsetdef::RadialColorDO> radial =
                    std::dynamic_pointer_cast<graphics::color::colorsetdef::RadialColorDO>(color);
                std::wcout << *radial << std::endl;
            }
            break;
            }
        }
    }

    psr->free(psr);
    psr = NULL;
    tstream->free(tstream);
    tstream = NULL;
    lxr->free(lxr);
    lxr = NULL;
    stream->close(stream);
    stream = NULL;

    std::shared_ptr<graphics::color::colorsetdef::ColorSetDO> p(pColorSetDO);
    return p;
}


std::wostream& graphics::color::colorsetdef::operator<<(std::wostream& os, const ColorValueDO &colorValue)
{
    os << typeid(colorValue).name()
       << L"[colorValueType="
       << (colorValue.colorValueType == COLORVALUE_TYPE_RGBA ? L"RGBA" : L"HSBA");
    os << L",values={" << colorValue.value1
       << L"," << colorValue.value2
       << L"," << colorValue.value3
       << L"," << colorValue.value4
       << L"}";
    if (colorValue.position <= 1.0 || colorValue.position >= -1.0) {
        os << L",position=" << colorValue.position;
    }
    os << L"]";
    return os;
}

std::wostream& graphics::color::colorsetdef::operator<<(std::wostream& os, const SolidColorDO &solidColor)
{
    os << typeid(solidColor).name()
       << L"[label=" << solidColor.label
       << L",colorValue=" << solidColor.colorValue << L"]";
    return os;
}

std::wostream& graphics::color::colorsetdef::operator<<(std::wostream& os, const LinearColorDO &linearColor)
{
    os << typeid(linearColor).name()
       << L"[label=" << linearColor.label
       << L",colorValues={";
    for (size_t i = 0; i < linearColor.colorValues.size(); ++i) {
        os << linearColor.colorValues[i];
        if (i != (linearColor.colorValues.size() - 1)) {
            os << L",";
        }
    }
    os << L"}]";
    return os;
}

std::wostream& graphics::color::colorsetdef::operator<<(std::wostream& os, const RadialColorDO &radialColor)
{
    os << typeid(radialColor).name()
       << L"[label=" << radialColor.label
       << L",offsetX=" << radialColor.offsetX
       << L",offsetY=" << radialColor.offsetY;
    os << L",colorValues={";
    for (size_t i = 0; i < radialColor.colorValues.size(); ++i) {
        os << radialColor.colorValues[i];
        if (i != (radialColor.colorValues.size() - 1)) {
            os << L",";
        }
    }
    os << L"}]";
    return os;
}

std::wostream& graphics::color::colorsetdef::operator<<(std::wostream& os, const ColorSetDO &colorSet)
{
    os << typeid(colorSet).name()
       << L"[colors={";
    for (size_t i = 0; i < colorSet.colors.size(); ++i) {
        std::shared_ptr<graphics::color::colorsetdef::IColorDO> color = colorSet.colors[i];
        switch (color->GetColorType()) {
        case COLOR_TYPE_SOLID: {
            std::shared_ptr<graphics::color::colorsetdef::SolidColorDO> solid =
                std::dynamic_pointer_cast<graphics::color::colorsetdef::SolidColorDO>(color);
            os << *solid;
        }
        break;
        case COLOR_TYPE_LINEAR: {
            std::shared_ptr<graphics::color::colorsetdef::LinearColorDO> linear =
                std::dynamic_pointer_cast<graphics::color::colorsetdef::LinearColorDO>(color);
            os << *linear;
        }
        break;
        case COLOR_TYPE_RADIAL: {
            std::shared_ptr<graphics::color::colorsetdef::RadialColorDO> radial =
                std::dynamic_pointer_cast<graphics::color::colorsetdef::RadialColorDO>(color);
            os << *radial;
        }
        break;
        }
        if (i != (colorSet.colors.size() - 1)) {
            os << L",";
        }
    }
    os << L"}]";
    return os;
}

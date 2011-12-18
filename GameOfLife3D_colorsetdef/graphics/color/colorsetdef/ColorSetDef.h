#ifndef COLORSETDEF_CPP_H_
#define COLORSETDEF_CPP_H_

#include "Includes.h"

namespace graphics
{
namespace color
{
namespace colorsetdef
{
typedef enum _COLORVALUE_TYPE {
    COLORVALUE_TYPE_RGBA = 0,
    COLORVALUE_TYPE_HSBA = 1
}
COLORVALUE_TYPE;

typedef enum _COLOR_TYPE {
    COLOR_TYPE_SOLID,
    COLOR_TYPE_LINEAR,
    COLOR_TYPE_RADIAL
} COLOR_TYPE;

class ColorValueDO
{
public:
    ColorValueDO() :
        colorValueType(COLORVALUE_TYPE_RGBA),
        value1(0.0f),
        value2(0.0f),
        value3(0.0f),
        value4(0.0f),
        position(0.0f)
    {}
    ColorValueDO(
        COLORVALUE_TYPE _colorValueType,
        float _value1,
        float _value2,
        float _value3,
        float _value4) :
        colorValueType(_colorValueType),
        value1(_value1),
        value2(_value2),
        value3(_value3),
        value4(_value4),
        position(0.0f) {}
    ColorValueDO(
        COLORVALUE_TYPE _colorValueType,
        float _value1,
        float _value2,
        float _value3,
        float _value4,
        float _position) :
        colorValueType(_colorValueType),
        value1(_value1),
        value2(_value2),
        value3(_value3),
        value4(_value4),
        position(_position) {}
    ~ColorValueDO() {}
    COLORVALUE_TYPE colorValueType;
    float value1;
    float value2;
    float value3;
    float value4;
    float position;

    friend std::wostream& operator<<(std::wostream& os, const ColorValueDO &colorValue);
};

class IColorDO
{
public:
    virtual COLOR_TYPE GetColorType() = 0;
};

class SolidColorDO : public IColorDO
{
public:
    SolidColorDO() : label(), colorValue() {}
    ~SolidColorDO() {}
    virtual COLOR_TYPE GetColorType() {
        return COLOR_TYPE_SOLID;
    }
    std::wstring label;
    ColorValueDO colorValue;

    friend std::wostream& operator<<(std::wostream& os, const SolidColorDO &solidColor);
};

class LinearColorDO : public IColorDO
{
public:
    enum DIRECTION {
        DIRECTION_TOP_BOTTOM = 0,
        DIRECTION_TOPLEFT_BOTTOMRIGHT = 1
    };
    LinearColorDO() :
        label(),
        direction(DIRECTION_TOP_BOTTOM),
        colorValues()
    {}
    ~LinearColorDO() {}
    virtual COLOR_TYPE GetColorType() {
        return COLOR_TYPE_LINEAR;
    }
    std::wstring label;
    DIRECTION direction;
    std::vector<ColorValueDO> colorValues;

    friend std::wostream& operator<<(std::wostream& os, const LinearColorDO &linearColor);
};

class RadialColorDO : public IColorDO
{
public:
    RadialColorDO() :
        label(),
        offsetX(0.0f),
        offsetY(0.0f),
        colorValues() {}
    ~RadialColorDO() {}
    virtual COLOR_TYPE GetColorType() {
        return COLOR_TYPE_RADIAL;
    }
    std::wstring label;
    float offsetX;
    float offsetY;
    std::vector<ColorValueDO> colorValues;

    friend std::wostream& operator<<(std::wostream& os, const RadialColorDO &radialColor);
};

class ColorSetDO
{
public:
    ColorSetDO() : colors() {}
    ~ColorSetDO() {}
    std::vector<std::shared_ptr<IColorDO>> colors;

    friend std::wostream& operator<<(std::wostream& os, const ColorSetDO &colorSet);
};

extern std::shared_ptr<colorsetdef::ColorSetDO> Decode(const std::wstring &s);

} // colorsetdef
} // color
} // graphics

#endif // COLORSETDEF_CPP_H_
#ifndef GRAPHICS_COLOR_ICOLOR_H_
#define GRAPHICS_COLOR_ICOLOR_H_

#include "graphics/color/ColorValue.h"

namespace graphics
{
namespace color
{
class IColor
{
public:
    enum COLOR_TYPE {
        COLOR_TYPE_SOLID_COLOR,
        COLOR_TYPE_LINEAR_GRADIENT_COLOR,
        COLOR_TYPE_RADIAL_GRADIENT_COLOR
    };
    virtual COLOR_TYPE GetColorType() const = 0;
    virtual const std::wstring& GetLabel() const = 0;
    virtual void Brighter(FLOAT rate) = 0;
    virtual void Darker(FLOAT rate) = 0;
    virtual void SetAlpha(FLOAT alpha) = 0;
    virtual void Reset() = 0;
};
}
}
#endif // GRAPHICS_COLOR_ICOLOR_H_
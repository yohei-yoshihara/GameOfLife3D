#ifndef GRAPHICS_COLOR_COLORSET_H_
#define GRAPHICS_COLOR_COLORSET_H_

#include "graphics/color/IColor.h"

namespace graphics
{
namespace color
{
class ColorSet
{
private:
    std::vector<std::shared_ptr<graphics::color::IColor>> m_colors;
    std::unordered_map<std::wstring, std::shared_ptr<graphics::color::IColor>> m_labelToColorMap;

public:
    ColorSet() :
        m_colors(), m_labelToColorMap()
    {}
    ColorSet(const wchar_t *colorDesc) :
        m_colors(), m_labelToColorMap() {
        AddColors(colorDesc);
    }
    ~ColorSet() {}
    void AddColors(const wchar_t *colorDesc);
    void AddColor(const std::shared_ptr<graphics::color::IColor> &color);
    void Clear();
    void SetColors(const wchar_t *colorDesc) {
        Clear();
        AddColors(colorDesc);
    }
    size_t GetNumberOfColors() const {
        return m_colors.size();
    }
    std::shared_ptr<graphics::color::IColor> GetColor(size_t index) const {
        return m_colors[index];
    }
    std::shared_ptr<graphics::color::IColor> GetColor(const std::wstring &label) const {
        if (m_labelToColorMap.count(label) > 0) {
            return m_labelToColorMap.at(label);
        } else {
            throw std::runtime_error("a color which has the specified label is not found");
        }
    }
};
}
}

#endif // GRAPHICS_COLOR_COLORSET_H_

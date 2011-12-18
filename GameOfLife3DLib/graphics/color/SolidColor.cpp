#include "stdafx.h"
#include "graphics/color/IColor.h"
#include "graphics/color/SolidColor.h"

std::wostream& graphics::color::operator<<( std::wostream& os, graphics::color::SolidColor const& solidColor )
{
    os << typeid(solidColor).name() << L"[label=" << solidColor.m_label
       << L",originalColor=" << solidColor.m_originalColor
       << L",currentColor=" << solidColor.m_currentColor
       << L"]";
    return os;
}

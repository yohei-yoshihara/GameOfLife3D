#include "StdAfx.h"
#include "graphics/FontAttribute.h"

std::wostream& graphics::operator<<( std::wostream& os, FontAttribute const& fontAttribute )
{
    os << L"[FontAttribute[fontFamilyName=" << fontAttribute.fontFamilyName
       << L",fontSize=" << fontAttribute.fontSize
       << L",bold=" << fontAttribute.bold
       << L",italic=" << fontAttribute.italic
       << L"]";
    return os;
}

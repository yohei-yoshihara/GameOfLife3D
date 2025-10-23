#include "StdAfx.h"
#include "ui/UICommon.h"

std::wostream &ui::operator<<(std::wostream &os, UIPoint const &point) {
  os << L"UIPoint[x=" << point.x << L",y=" << point.y << L"]";
  return os;
}

std::wostream &ui::operator<<(std::wostream &os, UISize const &size) {
  os << L"UISize[width=" << size.width << L",height=" << size.height << L"]";
  return os;
}

std::wostream &ui::operator<<(std::wostream &os, UIRectangle const &rect) {
  os << L"UIRectangle[x=" << rect.x << L",y=" << rect.y << L",width=" << rect.width << L",height=" << rect.height
     << L"]";
  return os;
}

std::wostream &ui::operator<<(std::wostream &os, UIDelta const &delta) {
  os << L"UIDelta[dx=" << delta.dx << L",dy=" << delta.dy << L"]";
  return os;
}

std::wostream &ui::operator<<(std::wostream &os, UI2Points const &points) {
  os << L"UI2Points[start.x=" << points.start.x << L",start.y=" << points.start.y << L",end.x=" << points.end.x
     << L",end.y=" << points.end.y << L"]";
  return os;
}

std::wostream &ui::operator<<(std::wostream &os, UIInsets const &insets) {
  os << L"UIInsets[left=" << insets.left << L",top=" << insets.top << L",right=" << insets.right << L",bottom="
     << insets.bottom << L"]";
  return os;
}

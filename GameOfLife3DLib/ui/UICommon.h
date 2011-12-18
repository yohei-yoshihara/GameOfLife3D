#ifndef UI_UICOMMON_H_
#define UI_UICOMMON_H_

namespace ui
{
enum ANCHOR {
    ANCHOR_NONE = 0,
    ANCHOR_LEFT = 1,
    ANCHOR_TOP = 2,
    ANCHOR_RIGHT = 4,
    ANCHOR_BOTTOM = 8,
};

class UIAnchor
{
public:
    UIAnchor() :
        anchor(ANCHOR_NONE), left(0), top(0), bottom(0), right(0)
    {}
    UIAnchor(int _anchor, FLOAT _left, FLOAT _top, FLOAT _right, FLOAT _bottom) :
        anchor(_anchor), left(_left), top(_top), right(_right), bottom(_bottom)
    {}
    int anchor;
    FLOAT left;
    FLOAT top;
    FLOAT right;
    FLOAT bottom;
};

class UIPoint
{
public:
    UIPoint() : x(0), y(0) {}
    UIPoint(FLOAT _x, FLOAT _y) : x(_x), y(_y) {}
    explicit UIPoint(POINT p) : x(static_cast<FLOAT>(p.x)), y(static_cast<FLOAT>(p.y)) {}
    FLOAT x;
    FLOAT y;
};
std::wostream& operator<< (std::wostream& os, UIPoint const& point);

class UISize
{
public:
    UISize() : width(0), height(0) {}
    UISize(FLOAT _width, FLOAT _height) : width(_width), height(_height) {}
    FLOAT width;
    FLOAT height;
};
std::wostream& operator<< (std::wostream& os, UISize const& size);

class UIRectangle
{
public:
    UIRectangle() : x(0), y(0), width(0), height(0) {}
    UIRectangle(FLOAT _x, FLOAT _y, FLOAT _width, FLOAT _height) :
        x(_x), y(_y), width(_width), height(_height) {}
    UIRectangle(UIPoint point, UISize size) :
        x(point.x), y(point.y), width(size.width), height(size.height) {}
    explicit UIRectangle(RECT r) : x(static_cast<FLOAT>(r.left)), y(static_cast<FLOAT>(r.top)),
        width(static_cast<FLOAT>(r.right - r.left + 1)), height(static_cast<FLOAT>(r.bottom - r.top + 1)) {}
    D2D1_RECT_F ToRectF() {
        return D2D1::RectF(x, y, x + width, y + height);
    }
    FLOAT x;
    FLOAT y;
    FLOAT width;
    FLOAT height;
};
std::wostream& operator<< (std::wostream& os, UIRectangle const& rect);

class UIDelta
{
public:
    UIDelta() : dx(0), dy(0) {}
    UIDelta(FLOAT _dx, FLOAT _dy) :
        dx(_dx), dy(_dy) {}
    FLOAT dx;
    FLOAT dy;
};
std::wostream& operator<< (std::wostream& os, UIDelta const& delta);

class UI2Points
{
public:
    UI2Points() : start(0, 0), end(0, 0) {}
    UI2Points(FLOAT startX, FLOAT startY, FLOAT endX, FLOAT endY) :
        start(startX, startY), end(endX, endY) {}
    UI2Points(UIPoint _start, UIPoint _end) :
        start(_start), end(_end) {}
    UI2Points(RECT rect) : start(static_cast<FLOAT>(rect.left), static_cast<FLOAT>(rect.top)),
        end(static_cast<FLOAT>(rect.right), static_cast<FLOAT>(rect.bottom)) {}
    UIPoint start;
    UIPoint end;
};
std::wostream& operator<< (std::wostream& os, UI2Points const& points);

class UIInsets
{
public:
    UIInsets() : left(0), top(0), right(0), bottom(0) {}
    UIInsets(FLOAT _left, FLOAT _top, FLOAT _right, FLOAT _bottom) :
        left(_left), top(_top), right(_right), bottom(_bottom) {}

    FLOAT left;
    FLOAT top;
    FLOAT right;
    FLOAT bottom;
};
std::wostream& operator<< (std::wostream& os, UIInsets const& insets);

class UIBase;
class UIButton;
class UIContainer;
class IUIDialog;
class UIInternalDialog;
class UILabel;
class UIListBox;
class UIListBoxItem;
class UIPanel;
class UIProgressBar;
class UIRadioButton;
class UIRadioButtonGroup;
class UIRoot;
class UIScrollBar;
class UITextField;
class UITextStoreACP;
class UITextEditSink;
class UITextDisplayAttribute;

enum CHAR_TYPE {
    CHAR_TYPE_NORMAL,
    CHAR_TYPE_CURSOR_UP,
    CHAR_TYPE_CURSOR_DOWN,
    CHAR_TYPE_CURSOR_LEFT,
    CHAR_TYPE_CURSOR_RIGHT,
    CHAR_TYPE_BACKSPACE,
    CHAR_TYPE_DELETE,
};

inline std::wostream& operator<< (std::wostream& os, D2D1_MATRIX_3X2_F const& matrix)
{
    os << L"D2D1_MATRIX_3X2_F[" << matrix._11 << L"," << matrix._12 << L"/"
       << matrix._21 << L"," << matrix._22 << L"/"
       << matrix._31 << L"," << matrix._32 << L"]";
    return os;
}

}

#endif // UI_UICOMMON_H_
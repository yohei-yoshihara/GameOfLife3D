#include "StdAfx.h"
#include "ui/UIContainer.h"
#include "ui/UIRoot.h"
#include "ui/UIBase.h"

ui::UIBase::UIBase()
    : m_parentContainer()
    , m_bounds()
    , m_enable(true)
    , m_focusable(false)
    , m_focus(false)
    , m_propertyChanged(false) {
}

ui::UIBase::~UIBase() {
}

HRESULT _ClientToScreen(std::shared_ptr<ui::UIContainer> parent, ui::UIPoint &out) {
  if (parent.get() == nullptr) {
    LOG(SEVERITY_LEVEL_ERROR) << L"parent container null, " << *parent;
    return E_FAIL;
  }

  auto root = std::dynamic_pointer_cast<ui::UIRoot>(parent);
  if (root.get() != nullptr) {
    out.x += root->GetX();
    out.y += root->GetY();
    HWND hWnd = root->GetHWnd();
    if (hWnd == 0) {
      // for test
    } else {
      FLOAT dx = out.x - static_cast<LONG>(out.x);
      FLOAT dy = out.y - static_cast<LONG>(out.y);
      POINT point;
      point.x = static_cast<LONG>(out.x);
      point.y = static_cast<LONG>(out.y);
      ::ClientToScreen(hWnd, &point);
      out.x = point.x + dx;
      out.y = point.y + dy;
    }
  } else {
    out.x += parent->GetX();
    out.y += parent->GetY();
    auto _p = parent->GetParentContainer();
    if (_p.expired()) {
      _ClientToScreen(_p.lock(), out);
    }
  }
  return S_OK;
}

HRESULT ui::UIBase::ClientToScreen(ui::UIPoint clientPoint, ui::UIPoint &screenPoint) const {
  ui::UIPoint out;
  out.x = GetX() + clientPoint.x;
  out.y = GetY() + clientPoint.y;
  auto container = GetParentContainer();
  if (!container.expired()) {
    _ClientToScreen(container.lock(), out);
    screenPoint = out;
  }
  return S_OK;
}

HRESULT ui::UIBase::ClientToScreen(ui::UIRectangle clientRect, ui::UIRectangle &screenRect) const {
  ui::UIPoint clientPoint;
  clientPoint.x = clientRect.x;
  clientPoint.y = clientRect.y;
  ui::UIPoint screenPoint;
  HRESULT hr = ClientToScreen(clientPoint, screenPoint);
  if (SUCCEEDED(hr)) {
    screenRect.x = screenPoint.x;
    screenRect.y = screenPoint.y;
    return S_OK;
  } else {
    return E_FAIL;
  }
}

HRESULT _ClientToWindow(std::shared_ptr<ui::UIContainer> parent, ui::UIPoint &out) {
  if (parent.get() == nullptr) {
    LOG(SEVERITY_LEVEL_ERROR) << L"parent container null, " << *parent;
    return E_FAIL;
  }

  auto root = std::dynamic_pointer_cast<ui::UIRoot>(parent);
  if (root.get() != nullptr) {
    out.x += root->GetX();
    out.y += root->GetY();
  } else {
    out.x += parent->GetX();
    out.y += parent->GetY();
    auto _p = parent->GetParentContainer();
    if (_p.expired()) {
      _ClientToWindow(_p.lock(), out);
    }
  }
  return S_OK;
}

HRESULT ui::UIBase::ClientToWindow(ui::UIPoint clientPoint, ui::UIPoint &windowPoint) const {
  ui::UIPoint out;
  out.x = GetX() + clientPoint.x;
  out.y = GetY() + clientPoint.y;
  auto container = GetParentContainer();
  if (!container.expired()) {
    _ClientToWindow(container.lock(), out);
    windowPoint = out;
  }
  return S_OK;
}

HRESULT ui::UIBase::ClientToWindow(ui::UIRectangle clientRect, ui::UIRectangle &windowRect) const {
  ui::UIPoint clientPoint;
  clientPoint.x = clientRect.x;
  clientPoint.y = clientRect.y;
  ui::UIPoint screenPoint;
  HRESULT hr = ClientToWindow(clientPoint, screenPoint);
  if (SUCCEEDED(hr)) {
    windowRect.x = screenPoint.x;
    windowRect.y = screenPoint.y;
    return S_OK;
  } else {
    return E_FAIL;
  }
}

std::wostream &ui::UIBase::Dump(std::wostream &os) const {
  os << L"x=" << GetX() << L",y=" << GetY() << L",width=" << GetWidth() << L",height=" << GetHeight() << L",enable="
     << std::boolalpha << m_enable << L",focusable=" << m_focusable << L",focus=" << m_focus;
  return os;
}

HRESULT ui::UIBase::Invalidate() {
  if (!m_parentContainer.expired()) {
    return m_parentContainer.lock()->Invalidate();
  }
  return E_NOT_VALID_STATE;
}

std::wostream &ui::operator<<(std::wostream &os, ui::UIBase const &uiBase) {
  os << L"UIBase[";
  uiBase.Dump(os);
  os << L"]";
  return os;
}

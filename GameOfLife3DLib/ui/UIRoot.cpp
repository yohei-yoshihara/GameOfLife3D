#include "StdAfx.h"
#include "Common.h"
#include "graphics/D3DInteropHelper.h"
#include "ui/IUIDialog.h"
#include "ui/UIRoot.h"

#define DEBUG_EVENT

ui::UIRoot::UIRoot(void)
    : m_hWnd(0)
    , m_dialogs()
    , m_displayingDialogName()
    , m_blackBrush() {
  AddBeforeAddElementCallback([&](std::shared_ptr<UIBase> element) -> bool {
    element->SetParentContainer(shared_from_this());
    return true;
  });
  SetFocusable(false);
}

ui::UIRoot::~UIRoot(void) {
}

void ui::UIRoot::NotifyFocusObtained(const std::shared_ptr<UIBase> &uiBase) {
  for (size_t i = 0; i < GetNumberOfElements(); ++i) {
    auto child = GetElement(i);
    if (uiBase != child) {
      child->LostFocus();
    }
  }
  if (!GetParentContainer().expired()) {
    GetParentContainer().lock()->NotifyFocusObtained(shared_from_this());
  }
}

HRESULT ui::UIRoot::Initialize(graphics::D3DInteropHelper *pD3DInteropHelper) {
  CHK_WARN_HRESULT(UIContainer::Initialize(pD3DInteropHelper));
  for (auto it = m_dialogs.begin(); it != m_dialogs.end(); ++it) {
    auto dialog = it->second;
    CHK_WARN_HRESULT(dialog->Initialize(pD3DInteropHelper));
  }
  return S_OK;
}

HRESULT ui::UIRoot::CreateDeviceDependentResources(graphics::D3DInteropHelper *pD3DInteropHelper,
                                                   ID2D1RenderTarget *pRenderTarget) {
  CHK_WARN_HRESULT(UIContainer::CreateDeviceDependentResources(pD3DInteropHelper, pRenderTarget));
  for (auto it = m_dialogs.begin(); it != m_dialogs.end(); ++it) {
    auto dialog = it->second;
    CHK_WARN_HRESULT(dialog->CreateDeviceDependentResources(pD3DInteropHelper, pRenderTarget));
  }

  CHK_FATAL_HRESULT(
      pRenderTarget->CreateSolidColorBrush(D2D1::ColorF(D2D1::ColorF::Black), D2D1::BrushProperties(), &m_blackBrush));
  return S_OK;
}

HRESULT ui::UIRoot::LayoutComponents() {
  CHK_WARN_HRESULT(UIContainer::LayoutComponents());
  for (auto it = m_dialogs.begin(); it != m_dialogs.end(); ++it) {
    auto dialog = it->second;
    CHK_WARN_HRESULT(dialog->LayoutComponents());
  }
  return S_OK;
}

HRESULT ui::UIRoot::Render(graphics::D3DInteropHelper *pD3DInteropHelper, ID2D1RenderTarget *pRenderTarget) {
  HRESULT hr = S_OK;
  if (m_displayingDialogName.length() == 0 || m_dialogs.count(m_displayingDialogName) == 0) {
    hr = UIContainer::Render(pD3DInteropHelper, pRenderTarget);
  } else {
    // internal dialog displaying
    if (m_dialogs.count(m_displayingDialogName) == 0) {
      SPDLOG_ERROR(L"could not found the internal dialog '{}'",
                   m_displayingDialogName);
      return E_FAIL;
    }
    auto dialog = m_dialogs.at(m_displayingDialogName);

    // draw background
    CComPtr<ID2D1Layer> layer = nullptr;
    CHK_FATAL_HRESULT(pRenderTarget->CreateLayer(&layer));
    pRenderTarget->PushLayer(D2D1::LayerParameters(D2D1::InfiniteRect(), nullptr, D2D1_ANTIALIAS_MODE_PER_PRIMITIVE,
                                                   D2D1::IdentityMatrix(), 0.5f, m_blackBrush, D2D1_LAYER_OPTIONS_NONE),
                             layer);
    hr = UIContainer::Render(pD3DInteropHelper, pRenderTarget);
    pRenderTarget->PopLayer();

    // draw the internal dialog
    FLOAT x = (GetWidth() - dialog->GetWidth()) / 2.0f + 0.5f;
    FLOAT y = (GetHeight() - dialog->GetHeight()) / 2.0f + 0.5f;
    dialog->SetBounds(x, y, dialog->GetWidth(), dialog->GetHeight());
    D2D1_MATRIX_3X2_F origMatrix;
    pRenderTarget->GetTransform(&origMatrix);
    D2D1_MATRIX_3X2_F matrix = origMatrix * D2D1::Matrix3x2F::Translation(x, y);
    pRenderTarget->SetTransform(matrix);
    CHK_WARN_HRESULT(dialog->Render(pD3DInteropHelper, pRenderTarget));
    pRenderTarget->SetTransform(origMatrix);
  }
  return hr;
}

void ui::UIRoot::AddDialog(const std::wstring &name, const std::shared_ptr<ui::IUIDialog> &dialog) {
  dialog->SetParentContainer(shared_from_this());
  m_dialogs.insert(std::pair<std::wstring, std::shared_ptr<ui::IUIDialog>>(name, dialog));
}

void ui::UIRoot::ShowDialog(const std::wstring &name) {
  if (m_dialogs.count(name) != 0) {
    m_displayingDialogName = name;
    auto dialog = m_dialogs.at(name);
    dialog->ShowDialog();
    Invalidate();
  }
}

void ui::UIRoot::NotifyDialogClosed() {
  m_displayingDialogName.clear();
}

void ui::UIRoot::DiscardDeviceDependentResources() {
  UIContainer::DiscardDeviceDependentResources();
  for (auto it = m_dialogs.begin(); it != m_dialogs.end(); ++it) {
    auto dialog = it->second;
    dialog->DiscardDeviceDependentResources();
  }
}

void ui::UIRoot::OnSetFocus(HWND hWnd, WPARAM wParam, LPARAM lParam, ULONGLONG timestampInMilliSeconds,
                            OUT bool *eaten) {
  if (m_displayingDialogName.length() == 0 || m_dialogs.count(m_displayingDialogName) == 0) {
    UIContainer::OnSetFocus(hWnd, wParam, lParam, timestampInMilliSeconds, eaten);
  } else {
    if (m_dialogs.count(m_displayingDialogName) == 0) {
      SPDLOG_ERROR(L"could not found the internal dialog '{}'",
                   m_displayingDialogName);
      return;
    }
    auto dialog = m_dialogs.at(m_displayingDialogName);
    dialog->OnSetFocus(hWnd, wParam, lParam, timestampInMilliSeconds, eaten);
  }
}

void ui::UIRoot::OnKillFocus(HWND hWnd, WPARAM wParam, LPARAM lParam, ULONGLONG timestampInMilliSeconds,
                             OUT bool *eaten) {
  if (m_displayingDialogName.length() == 0 || m_dialogs.count(m_displayingDialogName) == 0) {
    UIContainer::OnKillFocus(hWnd, wParam, lParam, timestampInMilliSeconds, eaten);
  } else {
    if (m_dialogs.count(m_displayingDialogName) == 0) {
      SPDLOG_ERROR(L"could not found the internal dialog '{}'",
                   m_displayingDialogName);
      return;
    }
    auto dialog = m_dialogs.at(m_displayingDialogName);
    dialog->OnKillFocus(hWnd, wParam, lParam, timestampInMilliSeconds, eaten);
  }
}

void ui::UIRoot::OnChar(HWND hWnd, WPARAM wParam, LPARAM lParam, CHAR_TYPE charType, wchar_t c,
                        ULONGLONG timestampInMilliSeconds, OUT bool *eaten) {
#ifdef DEBUG_EVENT
  SPDLOG_DEBUG(L"ENTER {}: char = {}", __func__, c);
#endif
  if (m_displayingDialogName.length() == 0 || m_dialogs.count(m_displayingDialogName) == 0) {
    UIContainer::OnChar(hWnd, wParam, lParam, charType, c, timestampInMilliSeconds, eaten);
  } else {
    if (m_dialogs.count(m_displayingDialogName) == 0) {
      SPDLOG_ERROR(L"could not found the internal dialog '{}'",
                   m_displayingDialogName);
      return;
    }
    auto dialog = m_dialogs.at(m_displayingDialogName);
    dialog->OnChar(hWnd, wParam, lParam, charType, c, timestampInMilliSeconds, eaten);
  }
#ifdef DEBUG_EVENT
  SPDLOG_DEBUG(L"LEAVE {}", __func__);
#endif
}

void ui::UIRoot::OnLeftMouseDown(HWND hWnd, WPARAM wParam, LPARAM lParam, UIPoint clientPoint,
                                 ULONGLONG timestampInMilliSeconds, OUT bool *eaten) {
  if (m_displayingDialogName.length() == 0 || m_dialogs.count(m_displayingDialogName) == 0) {
    UIContainer::OnLeftMouseDown(hWnd, wParam, lParam, clientPoint, timestampInMilliSeconds, eaten);
  } else {
    if (m_dialogs.count(m_displayingDialogName) == 0) {
      SPDLOG_ERROR(L"could not found the internal dialog '{}'",
                   m_displayingDialogName);
      return;
    }
    auto dialog = m_dialogs.at(m_displayingDialogName);
    dialog->OnLeftMouseDown(hWnd, wParam, lParam, dialog->Translate(clientPoint, GetInsets().left, GetInsets().top),
                            timestampInMilliSeconds, eaten);
  }
}

void ui::UIRoot::OnLeftMouseUp(HWND hWnd, WPARAM wParam, LPARAM lParam, UIPoint clientPoint,
                               ULONGLONG timestampInMilliSeconds, OUT bool *eaten) {
  if (m_displayingDialogName.length() == 0 || m_dialogs.count(m_displayingDialogName) == 0) {
    UIContainer::OnLeftMouseUp(hWnd, wParam, lParam, clientPoint, timestampInMilliSeconds, eaten);
  } else {
    if (m_dialogs.count(m_displayingDialogName) == 0) {
      SPDLOG_ERROR(L"could not found the internal dialog '{}'",
                   m_displayingDialogName);
      return;
    }
    auto dialog = m_dialogs.at(m_displayingDialogName);
    dialog->OnLeftMouseUp(hWnd, wParam, lParam, dialog->Translate(clientPoint, GetInsets().left, GetInsets().top),
                          timestampInMilliSeconds, eaten);
  }
}

void ui::UIRoot::OnSingleClick(HWND hWnd, WPARAM wParam, LPARAM lParam, UIPoint clientPoint,
                               ULONGLONG timestampInMilliSeconds, OUT bool *eaten) {
  if (m_displayingDialogName.length() == 0 || m_dialogs.count(m_displayingDialogName) == 0) {
    UIContainer::OnSingleClick(hWnd, wParam, lParam, clientPoint, timestampInMilliSeconds, eaten);
  } else {
    if (m_dialogs.count(m_displayingDialogName) == 0) {
      SPDLOG_ERROR(L"could not found the internal dialog '{}'",
                   m_displayingDialogName);
      return;
    }
    auto dialog = m_dialogs.at(m_displayingDialogName);
    dialog->OnSingleClick(hWnd, wParam, lParam, dialog->Translate(clientPoint, GetInsets().left, GetInsets().top),
                          timestampInMilliSeconds, eaten);
  }
}

void ui::UIRoot::OnDoubleClick(HWND hWnd, WPARAM wParam, LPARAM lParam, UIPoint clientPoint,
                               ULONGLONG timestampInMilliSeconds, OUT bool *eaten) {
  if (m_displayingDialogName.length() == 0 || m_dialogs.count(m_displayingDialogName) == 0) {
    UIContainer::OnDoubleClick(hWnd, wParam, lParam, clientPoint, timestampInMilliSeconds, eaten);
  } else {
    if (m_dialogs.count(m_displayingDialogName) == 0) {
      SPDLOG_ERROR(L"could not found the internal dialog '{}'",
                   m_displayingDialogName);
      return;
    }
    auto dialog = m_dialogs.at(m_displayingDialogName);
    dialog->OnDoubleClick(hWnd, wParam, lParam, dialog->Translate(clientPoint, GetInsets().left, GetInsets().top),
                          timestampInMilliSeconds, eaten);
  }
}

void ui::UIRoot::OnDraggingStart(HWND hWnd, WPARAM wParam, LPARAM lParam, UI2Points clientDragRect, UIDelta delta,
                                 ULONGLONG timestampInMilliSeconds, OUT bool *eaten) {
  if (m_displayingDialogName.length() == 0 || m_dialogs.count(m_displayingDialogName) == 0) {
    UIContainer::OnDraggingStart(hWnd, wParam, lParam, clientDragRect, delta, timestampInMilliSeconds, eaten);
  } else {
    if (m_dialogs.count(m_displayingDialogName) == 0) {
      SPDLOG_ERROR(L"could not found the internal dialog '{}'",
                   m_displayingDialogName);
      return;
    }
    auto dialog = m_dialogs.at(m_displayingDialogName);
    dialog->OnDraggingStart(hWnd, wParam, lParam, dialog->Translate(clientDragRect, GetInsets().left, GetInsets().top),
                            delta, timestampInMilliSeconds, eaten);
  }
}

void ui::UIRoot::OnDragging(HWND hWnd, WPARAM wParam, LPARAM lParam, UI2Points clientDragRect, UIDelta delta,
                            ULONGLONG timestampInMilliSeconds, OUT bool *eaten) {
  if (m_displayingDialogName.length() == 0 || m_dialogs.count(m_displayingDialogName) == 0) {
    UIContainer::OnDragging(hWnd, wParam, lParam, clientDragRect, delta, timestampInMilliSeconds, eaten);
  } else {
    if (m_dialogs.count(m_displayingDialogName) == 0) {
      SPDLOG_ERROR(L"could not found the internal dialog '{}'",
                   m_displayingDialogName);
      return;
    }
    auto dialog = m_dialogs.at(m_displayingDialogName);
    dialog->OnDragging(hWnd, wParam, lParam, dialog->Translate(clientDragRect, GetInsets().left, GetInsets().top),
                       delta, timestampInMilliSeconds, eaten);
  }
}

void ui::UIRoot::OnDraggingEnd(HWND hWnd, WPARAM wParam, LPARAM lParam, UI2Points clientDragRect, UIDelta delta,
                               ULONGLONG timestampInMilliSeconds, OUT bool *eaten) {
  if (m_displayingDialogName.length() == 0 || m_dialogs.count(m_displayingDialogName) == 0) {
    UIContainer::OnDraggingEnd(hWnd, wParam, lParam, clientDragRect, delta, timestampInMilliSeconds, eaten);
  } else {
    if (m_dialogs.count(m_displayingDialogName) == 0) {
      SPDLOG_ERROR(L"could not found the internal dialog '{}'",
                   m_displayingDialogName);
      return;
    }
    auto dialog = m_dialogs.at(m_displayingDialogName);
    dialog->OnDraggingEnd(hWnd, wParam, lParam, dialog->Translate(clientDragRect, GetInsets().left, GetInsets().top),
                          delta, timestampInMilliSeconds, eaten);
  }
}

void ui::UIRoot::OnMouseOver(HWND hWnd, WPARAM wParam, LPARAM lParam, UIPoint clientPoint, UIDelta delta,
                             ULONGLONG timestampInMilliSeconds, OUT bool *eaten) {
  if (m_displayingDialogName.length() == 0 || m_dialogs.count(m_displayingDialogName) == 0) {
    UIContainer::OnMouseOver(hWnd, wParam, lParam, clientPoint, delta, timestampInMilliSeconds, eaten);
  } else {
    if (m_dialogs.count(m_displayingDialogName) == 0) {
      SPDLOG_ERROR(L"could not found the internal dialog '{}'",
                   m_displayingDialogName);
      return;
    }
    auto dialog = m_dialogs.at(m_displayingDialogName);
    dialog->OnMouseOver(hWnd, wParam, lParam, dialog->Translate(clientPoint, GetInsets().left, GetInsets().top), delta,
                        timestampInMilliSeconds, eaten);
  }
}

void ui::UIRoot::OnMouseOut(HWND hWnd, WPARAM wParam, LPARAM lParam, UIPoint clientPoint, UIDelta delta,
                            ULONGLONG timestampInMilliSeconds, OUT bool *eaten) {
  if (m_displayingDialogName.length() == 0 || m_dialogs.count(m_displayingDialogName) == 0) {
    UIContainer::OnMouseOut(hWnd, wParam, lParam, clientPoint, delta, timestampInMilliSeconds, eaten);
  } else {
    if (m_dialogs.count(m_displayingDialogName) == 0) {
      SPDLOG_ERROR(L"could not found the internal dialog '{}'",
                   m_displayingDialogName);
      return;
    }
    auto dialog = m_dialogs.at(m_displayingDialogName);
    dialog->OnMouseOut(hWnd, wParam, lParam, dialog->Translate(clientPoint, GetInsets().left, GetInsets().top), delta,
                       timestampInMilliSeconds, eaten);
  }
}

void ui::UIRoot::OnRightSingleClick(HWND hWnd, WPARAM wParam, LPARAM lParam, UIPoint clientPoint,
                                    ULONGLONG timestampInMilliSeconds, OUT bool *eaten) {
  if (m_displayingDialogName.length() == 0 || m_dialogs.count(m_displayingDialogName) == 0) {
    UIContainer::OnRightSingleClick(hWnd, wParam, lParam, clientPoint, timestampInMilliSeconds, eaten);
  } else {
    if (m_dialogs.count(m_displayingDialogName) == 0) {
      SPDLOG_ERROR(L"could not found the internal dialog '{}'",
                   m_displayingDialogName);
      return;
    }
    auto dialog = m_dialogs.at(m_displayingDialogName);
    dialog->OnRightSingleClick(hWnd, wParam, lParam, dialog->Translate(clientPoint, GetInsets().left, GetInsets().top),
                               timestampInMilliSeconds, eaten);
  }
}

void ui::UIRoot::OnGestureBegin(HWND hWnd, WPARAM wParam, LPARAM lParam, UIPoint clientPoint, PGESTUREINFO pGestureInfo,
                                ULONGLONG timestampInMilliSeconds, OUT bool *eaten) {
  if (m_displayingDialogName.length() == 0 || m_dialogs.count(m_displayingDialogName) == 0) {
    UIContainer::OnGestureBegin(hWnd, wParam, lParam, clientPoint, pGestureInfo, timestampInMilliSeconds, eaten);
  } else {
    if (m_dialogs.count(m_displayingDialogName) == 0) {
      SPDLOG_ERROR(L"could not found the internal dialog '{}'",
                   m_displayingDialogName);
      return;
    }
    auto dialog = m_dialogs.at(m_displayingDialogName);
    dialog->OnGestureBegin(hWnd, wParam, lParam, dialog->Translate(clientPoint, GetInsets().left, GetInsets().top),
                           pGestureInfo, timestampInMilliSeconds, eaten);
  }
}

void ui::UIRoot::OnGestureEnd(HWND hWnd, WPARAM wParam, LPARAM lParam, UIPoint clientPoint, PGESTUREINFO pGestureInfo,
                              ULONGLONG timestampInMilliSeconds, OUT bool *eaten) {
  if (m_displayingDialogName.length() == 0 || m_dialogs.count(m_displayingDialogName) == 0) {
    UIContainer::OnGestureEnd(hWnd, wParam, lParam, clientPoint, pGestureInfo, timestampInMilliSeconds, eaten);
  } else {
    if (m_dialogs.count(m_displayingDialogName) == 0) {
      SPDLOG_ERROR(L"could not found the internal dialog '{}'",
                   m_displayingDialogName);
      return;
    }
    auto dialog = m_dialogs.at(m_displayingDialogName);
    dialog->OnGestureEnd(hWnd, wParam, lParam, dialog->Translate(clientPoint, GetInsets().left, GetInsets().top),
                         pGestureInfo, timestampInMilliSeconds, eaten);
  }
}

void ui::UIRoot::OnZoom(HWND hWnd, WPARAM wParam, LPARAM lParam, UIPoint clientPoint, PGESTUREINFO pGestureInfo,
                        UIPoint zoomCenterPoint, FLOAT zoomFactor, ULONGLONG timestampInMilliSeconds, OUT bool *eaten) {
  if (m_displayingDialogName.length() == 0 || m_dialogs.count(m_displayingDialogName) == 0) {
    UIContainer::OnZoom(hWnd, wParam, lParam, clientPoint, pGestureInfo, zoomCenterPoint, zoomFactor,
                        timestampInMilliSeconds, eaten);
  } else {
    if (m_dialogs.count(m_displayingDialogName) == 0) {
      SPDLOG_ERROR(L"could not found the internal dialog '{}'",
                   m_displayingDialogName);
      return;
    }
    auto dialog = m_dialogs.at(m_displayingDialogName);
    dialog->OnZoom(hWnd, wParam, lParam, dialog->Translate(clientPoint, GetInsets().left, GetInsets().top),
                   pGestureInfo, dialog->Translate(zoomCenterPoint, GetInsets().left, GetInsets().top), zoomFactor,
                   timestampInMilliSeconds, eaten);
  }
}

void ui::UIRoot::OnPan(HWND hWnd, WPARAM wParam, LPARAM lParam, UIPoint clientPoint, PGESTUREINFO pGestureInfo,
                       UIDelta delta, ULONGLONG timestampInMilliSeconds, OUT bool *eaten) {
  if (m_displayingDialogName.length() == 0 || m_dialogs.count(m_displayingDialogName) == 0) {
    UIContainer::OnPan(hWnd, wParam, lParam, clientPoint, pGestureInfo, delta, timestampInMilliSeconds, eaten);
  } else {
    if (m_dialogs.count(m_displayingDialogName) == 0) {
      SPDLOG_ERROR(L"could not found the internal dialog '{}'",
                   m_displayingDialogName);
      return;
    }
    auto dialog = m_dialogs.at(m_displayingDialogName);
    dialog->OnPan(hWnd, wParam, lParam, dialog->Translate(clientPoint, GetInsets().left, GetInsets().top), pGestureInfo,
                  delta, timestampInMilliSeconds, eaten);
  }
}

void ui::UIRoot::OnRotate(HWND hWnd, WPARAM wParam, LPARAM lParam, UIPoint clientPoint, PGESTUREINFO pGestureInfo,
                          UIPoint rotateCenterPoint, FLOAT rotateAngle, ULONGLONG timestampInMilliSeconds,
                          OUT bool *eaten) {
  if (m_displayingDialogName.length() == 0 || m_dialogs.count(m_displayingDialogName) == 0) {
    UIContainer::OnRotate(hWnd, wParam, lParam, clientPoint, pGestureInfo, rotateCenterPoint, rotateAngle,
                          timestampInMilliSeconds, eaten);
  } else {
    if (m_dialogs.count(m_displayingDialogName) == 0) {
      SPDLOG_ERROR(L"could not found the internal dialog '{}'",
                   m_displayingDialogName);
      return;
    }
    auto dialog = m_dialogs.at(m_displayingDialogName);
    dialog->OnRotate(hWnd, wParam, lParam, dialog->Translate(clientPoint, GetInsets().left, GetInsets().top),
                     pGestureInfo, dialog->Translate(rotateCenterPoint, GetInsets().left, GetInsets().top), rotateAngle,
                     timestampInMilliSeconds, eaten);
  }
}

void ui::UIRoot::OnTwoFingerTap(HWND hWnd, WPARAM wParam, LPARAM lParam, UIPoint clientPoint, PGESTUREINFO pGestureInfo,
                                ULONGLONG timestampInMilliSeconds, OUT bool *eaten) {
  if (m_displayingDialogName.length() == 0 || m_dialogs.count(m_displayingDialogName) == 0) {
    UIContainer::OnTwoFingerTap(hWnd, wParam, lParam, clientPoint, pGestureInfo, timestampInMilliSeconds, eaten);
  } else {
    if (m_dialogs.count(m_displayingDialogName) == 0) {
      SPDLOG_ERROR(L"could not found the internal dialog '{}'",
                   m_displayingDialogName);
      return;
    }
    auto dialog = m_dialogs.at(m_displayingDialogName);
    dialog->OnTwoFingerTap(hWnd, wParam, lParam, dialog->Translate(clientPoint, GetInsets().left, GetInsets().top),
                           pGestureInfo, timestampInMilliSeconds, eaten);
  }
}

void ui::UIRoot::OnPressAndTap(HWND hWnd, WPARAM wParam, LPARAM lParam, UIPoint clientPoint, PGESTUREINFO pGestureInfo,
                               ULONGLONG timestampInMilliSeconds, OUT bool *eaten) {
  if (m_displayingDialogName.length() == 0 || m_dialogs.count(m_displayingDialogName) == 0) {
    UIContainer::OnPressAndTap(hWnd, wParam, lParam, clientPoint, pGestureInfo, timestampInMilliSeconds, eaten);
  } else {
    if (m_dialogs.count(m_displayingDialogName) == 0) {
      SPDLOG_ERROR(L"could not found the internal dialog '{}'",
                   m_displayingDialogName);
      return;
    }
    auto dialog = m_dialogs.at(m_displayingDialogName);
    dialog->OnPressAndTap(hWnd, wParam, lParam, dialog->Translate(clientPoint, GetInsets().left, GetInsets().top),
                          pGestureInfo, timestampInMilliSeconds, eaten);
  }
}

HRESULT ui::UIRoot::Invalidate() {
  BOOL bResult = ::InvalidateRect(m_hWnd, nullptr, TRUE);
  HRESULT hr = bResult ? S_OK : E_FAIL;
  return hr;
}

void ui::UIRoot::OnResize(UISize size) {
  SetBounds(GetX(), GetY(), size.width, size.height);
}

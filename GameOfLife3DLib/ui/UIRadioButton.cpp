#include "StdAfx.h"
#include "Common.h"
#include "graphics/D3DInteropHelper.h"
#include "graphics/brush/LinearGradientBrush.h"
#include "graphics/color/ColorUtil.h"
#include "ui/UIRadioButtonGroup.h"
#include "ui/UIRadioButton.h"

using namespace ui;
using namespace graphics;

const FLOAT MARGIN_BOX_TEXT = 4.0f;

ui::UIRadioButton::UIRadioButton(void)
    : m_index(0)
    , m_radioButtonGroup()
    , m_selected(false)
    , m_text()
    , m_fontAttribute()
    , m_textFormat()
    , m_textLayout()
    , m_textMetrics()
    , m_outerShineBrush(std::make_shared<graphics::brush::LinearGradientBrush>())
    , m_innerShadowBrush(std::make_shared<graphics::brush::LinearGradientBrush>())
    , m_innerShineBrush(std::make_shared<graphics::brush::LinearGradientBrush>())
    , m_bodyBrush(std::make_shared<graphics::brush::LinearGradientBrush>())
    , m_selectedMarkBrush(std::make_shared<graphics::brush::LinearGradientBrush>())
    , m_foregroundBrush(std::make_shared<graphics::brush::LinearGradientBrush>()) {
  SetFocusable(true);

  m_outerShineBrush->AddGradientStop(D2D1::GradientStop(0.0, graphics::color::ColorUtil::HsvToRgb(0, 0, 0.21f, 1.0f)));
  m_outerShineBrush->AddGradientStop(D2D1::GradientStop(1.0, graphics::color::ColorUtil::HsvToRgb(0, 0, 0.21f, 1.0f)));

  m_innerShadowBrush->AddGradientStop(D2D1::GradientStop(0.0, graphics::color::ColorUtil::HsvToRgb(0, 0, 0.05f, 1.0f)));
  m_innerShadowBrush->AddGradientStop(D2D1::GradientStop(1.0, graphics::color::ColorUtil::HsvToRgb(0, 0, 0.05f, 1.0f)));

  m_innerShineBrush->AddGradientStop(D2D1::GradientStop(0.0, graphics::color::ColorUtil::HsvToRgb(0, 0, 0.25f, 1.0f)));
  m_innerShineBrush->AddGradientStop(D2D1::GradientStop(1.0, graphics::color::ColorUtil::HsvToRgb(0, 0, 0.13f, 1.0f)));

  m_bodyBrush->AddGradientStop(D2D1::GradientStop(0.0, graphics::color::ColorUtil::HsvToRgb(0, 0, 0.13f, 1.0f)));
  m_bodyBrush->AddGradientStop(D2D1::GradientStop(1.0, graphics::color::ColorUtil::HsvToRgb(0, 0, 0.13f, 1.0f)));

  m_selectedMarkBrush->AddGradientStop(D2D1::GradientStop(0.0, D2D1::ColorF(D2D1::ColorF::White, 1.0f)));
  m_selectedMarkBrush->AddGradientStop(D2D1::GradientStop(1.0, D2D1::ColorF(D2D1::ColorF::White, 1.0f)));

  m_foregroundBrush->AddGradientStop(D2D1::GradientStop(0.0, D2D1::ColorF(D2D1::ColorF::White, 1.0f)));
  m_foregroundBrush->AddGradientStop(D2D1::GradientStop(1.0, D2D1::ColorF(D2D1::ColorF::White, 1.0f)));
}

ui::UIRadioButton::~UIRadioButton(void) {
}

HRESULT ui::UIRadioButton::Initialize(graphics::D3DInteropHelper *pD3DInteropHelper) {
  UNREFERENCED_PARAMETER(pD3DInteropHelper);
  return S_OK;
}

HRESULT ui::UIRadioButton::CreateDeviceDependentResources(graphics::D3DInteropHelper *pD3DInteropHelper,
                                                          ID2D1RenderTarget *pRenderTarget) {
  CHK_FATAL_HRESULT(m_outerShineBrush->CreateDeviceDependentResource(
      pRenderTarget, D2D1::RectF(0.0f, 0.0f, GetWidth(), GetHeight()), 1.0f));
  CHK_FATAL_HRESULT(m_innerShadowBrush->CreateDeviceDependentResource(
      pRenderTarget, D2D1::RectF(0.0f, 0.0f, GetWidth(), GetHeight()), 1.0f));
  CHK_FATAL_HRESULT(m_innerShineBrush->CreateDeviceDependentResource(
      pRenderTarget, D2D1::RectF(0.0f, 0.0f, GetWidth(), GetHeight()), 1.0f));
  CHK_FATAL_HRESULT(m_bodyBrush->CreateDeviceDependentResource(pRenderTarget,
                                                               D2D1::RectF(0.0f, 0.0f, GetWidth(), GetHeight()), 1.0f));
  CHK_FATAL_HRESULT(m_selectedMarkBrush->CreateDeviceDependentResource(
      pRenderTarget, D2D1::RectF(0.0f, 0.0f, GetWidth(), GetHeight()), 1.0f));
  CHK_FATAL_HRESULT(m_foregroundBrush->CreateDeviceDependentResource(
      pRenderTarget, D2D1::RectF(0.0f, 0.0f, GetWidth(), GetHeight()), 1.0f));

  m_textFormat = nullptr;
  CHK_FATAL_HRESULT(pD3DInteropHelper->GetDWriteFactory()->CreateTextFormat(
      m_fontAttribute.fontFamilyName.c_str(), nullptr, m_fontAttribute.GetFontWeight(), m_fontAttribute.GetFontStyle(),
      m_fontAttribute.GetFontStretch(), m_fontAttribute.fontSize, L"", &m_textFormat));

  m_textLayout = nullptr;
  CHK_FATAL_HRESULT(pD3DInteropHelper->GetDWriteFactory()->CreateTextLayout(
      m_text.c_str(), static_cast<UINT>(m_text.size()), m_textFormat, FLT_MAX, FLT_MAX, &m_textLayout));

  CHK_FATAL_HRESULT(m_textLayout->GetMetrics(&m_textMetrics));

  return S_OK;
}

void ui::UIRadioButton::DiscardDeviceDependentResources() {
  m_outerShineBrush->DiscardDeviceDependentResource();
  m_innerShadowBrush->DiscardDeviceDependentResource();
  m_innerShineBrush->DiscardDeviceDependentResource();
  m_bodyBrush->DiscardDeviceDependentResource();
  m_selectedMarkBrush->DiscardDeviceDependentResource();
  m_foregroundBrush->DiscardDeviceDependentResource();
}

HRESULT ui::UIRadioButton::Render(graphics::D3DInteropHelper *pD3DInteropHelper, ID2D1RenderTarget *pRenderTarget) {
  UNREFERENCED_PARAMETER(pD3DInteropHelper);
  FLOAT textHeight = m_textMetrics.height;

  pRenderTarget->FillRectangle(D2D1::RectF(0.0f, 0.0f, textHeight, textHeight), m_outerShineBrush->GetBrush());
  pRenderTarget->FillRectangle(D2D1::RectF(1.0f, 1.0f, textHeight - 1.0f, textHeight - 1.0f),
                               m_innerShadowBrush->GetBrush());
  pRenderTarget->FillRectangle(D2D1::RectF(2.0f, 2.0f, textHeight - 2.0f, textHeight - 2.0f),
                               m_innerShineBrush->GetBrush());
  pRenderTarget->FillRectangle(D2D1::RectF(3.0f, 3.0f, textHeight - 3.0f, textHeight - 3.0f), m_bodyBrush->GetBrush());

  if (m_selected) {
    pRenderTarget->FillRectangle(D2D1::RectF(5.0f, 5.0f, textHeight - 5.0f, textHeight - 5.0f),
                                 m_selectedMarkBrush->GetBrush());
  }
  pRenderTarget->DrawText(m_text.c_str(), static_cast<UINT>(m_text.size()), m_textFormat,
                          D2D1::RectF(textHeight + MARGIN_BOX_TEXT, 0.0f, FLT_MAX, FLT_MAX),
                          m_foregroundBrush->GetBrush());
  return S_OK;
}

void ui::UIRadioButton::ClearResources() {
  m_textFormat = nullptr;
  m_textLayout = nullptr;
}

ui::UISize ui::UIRadioButton::GetPreferredSize() const {
  UISize size;
  size.width = MARGIN_BOX_TEXT + m_textMetrics.height + m_textMetrics.widthIncludingTrailingWhitespace;
  size.height = m_textMetrics.height;
  return size;
}

void ui::UIRadioButton::SetSelected(bool selected) {
  m_selected = selected;
  if (m_selected && !m_radioButtonGroup.expired()) {
    m_radioButtonGroup.lock()->NotifySelectionChange(m_index);
  }
}

void ui::UIRadioButton::OnLeftMouseDown(HWND hWnd, WPARAM wParam, LPARAM lParam, UIPoint clientPoint,
                                        ULONGLONG timestampInMilliSeconds, OUT bool *eaten) {
  UNREFERENCED_PARAMETER(hWnd);
  UNREFERENCED_PARAMETER(wParam);
  UNREFERENCED_PARAMETER(lParam);
  UNREFERENCED_PARAMETER(clientPoint);
  UNREFERENCED_PARAMETER(timestampInMilliSeconds);
  SetSelected(true);
  if (!HasFocus()) {
    RequestFocus();
    if (!GetParentContainer().expired()) {
      GetParentContainer().lock()->NotifyFocusObtained(shared_from_this());
    }
  }
  *eaten = true;
}

void ui::UIRadioButton::SetRadioButtonGroup(std::weak_ptr<UIRadioButtonGroup> radioButtonGroup) {
  m_radioButtonGroup = radioButtonGroup;
  SetParentContainer(radioButtonGroup);
}

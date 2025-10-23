#include "StdAfx.h"
#include "Common.h"
#include "graphics/D3DInteropHelper.h"
#include "graphics/brush/LinearGradientBrush.h"
#include "graphics/color/ColorUtil.h"
#include "ui/UIRoot.h"
#include "ui/UIInternalDialog.h"

// #define DEBUG_UIINTERNALDIALOG

const static FLOAT TEXT_MARGIN = 2.0f;

ui::UIInternalDialog::UIInternalDialog(void)
    : m_dialogStatus(DIALOG_STATUS_CLOSE)
    , m_dialogResult(DIALOG_RESULT_OK)
    , m_title()
    , m_fontAttribute()
    , m_outerShineBrush(std::make_shared<graphics::brush::LinearGradientBrush>())
    , m_innerShadowBrush(std::make_shared<graphics::brush::LinearGradientBrush>())
    , m_backgroundBrush(std::make_shared<graphics::brush::LinearGradientBrush>())
    , m_titleForegroundBrush(std::make_shared<graphics::brush::LinearGradientBrush>())
    , m_titleBackgroundBrush(std::make_shared<graphics::brush::LinearGradientBrush>()) {
  AddBeforeAddElementCallback([&](std::shared_ptr<UIBase> element) -> bool {
    element->SetParentContainer(shared_from_this());
    return true;
  });

  SetFocusable(false);

  m_outerShineBrush->AddGradientStop(D2D1::GradientStop(0.0, graphics::color::ColorUtil::HsvToRgb(0, 0, 0.35f, 1.0f)));
  m_outerShineBrush->AddGradientStop(D2D1::GradientStop(1.0, graphics::color::ColorUtil::HsvToRgb(0, 0, 0.35f, 1.0f)));

  m_innerShadowBrush->AddGradientStop(D2D1::GradientStop(0.0, graphics::color::ColorUtil::HsvToRgb(0, 0, 0.18f, 1.0f)));
  m_innerShadowBrush->AddGradientStop(D2D1::GradientStop(1.0, graphics::color::ColorUtil::HsvToRgb(0, 0, 0.18f, 1.0f)));

  m_backgroundBrush->AddGradientStop(D2D1::GradientStop(0.0, graphics::color::ColorUtil::HsvToRgb(0, 0, 0.25f, 1.0f)));
  m_backgroundBrush->AddGradientStop(D2D1::GradientStop(1.0, graphics::color::ColorUtil::HsvToRgb(0, 0, 0.25f, 1.0f)));

  m_titleForegroundBrush->AddGradientStop(D2D1::GradientStop(0.0, D2D1::ColorF(D2D1::ColorF::White)));
  m_titleForegroundBrush->AddGradientStop(D2D1::GradientStop(1.0, D2D1::ColorF(D2D1::ColorF::White)));

  m_titleBackgroundBrush->AddGradientStop(
      D2D1::GradientStop(0.0, graphics::color::ColorUtil::HsvToRgb(0, 0, 0.19f, 1.0f)));
  m_titleBackgroundBrush->AddGradientStop(
      D2D1::GradientStop(1.0, graphics::color::ColorUtil::HsvToRgb(0, 0, 0.19f, 1.0f)));
}

ui::UIInternalDialog::~UIInternalDialog(void) {
}

HRESULT ui::UIInternalDialog::Initialize(graphics::D3DInteropHelper *pD3DInteropHelper) {
  CHK_WARN_HRESULT(UIContainer::Initialize(pD3DInteropHelper));

  // text format
  m_textFormat = nullptr;
  CHK_FATAL_HRESULT(pD3DInteropHelper->GetDWriteFactory()->CreateTextFormat(
      m_fontAttribute.fontFamilyName.c_str(), nullptr, m_fontAttribute.GetFontWeight(), m_fontAttribute.GetFontStyle(),
      m_fontAttribute.GetFontStretch(), m_fontAttribute.fontSize, L"", &m_textFormat));

  // text layout
  m_textLayout = nullptr;
  CHK_FATAL_HRESULT(pD3DInteropHelper->GetDWriteFactory()->CreateTextLayout(
      m_title.c_str(), static_cast<UINT>(m_title.size()), m_textFormat, FLT_MAX, FLT_MAX, &m_textLayout));

  // text metrics
  CHK_FATAL_HRESULT(m_textLayout->GetMetrics(&m_textMetrics));

  UIInsets insets(2.0f, m_textMetrics.height + 2.0f + TEXT_MARGIN, 2.0f, 2.0f);
  SetInsets(insets);
#ifdef DEBUG_UIINTERNALDIALOG
  LOG(SEVERITY_LEVEL_DEBUG) << L"insets = " << insets;
#endif
  return S_OK;
}

HRESULT ui::UIInternalDialog::CreateDeviceDependentResources(graphics::D3DInteropHelper *pD3DInteropHelper,
                                                             ID2D1RenderTarget *pRenderTarget) {
  CHK_WARN_HRESULT(UIContainer::CreateDeviceDependentResources(pD3DInteropHelper, pRenderTarget));
  CHK_FATAL_HRESULT(m_outerShineBrush->CreateDeviceDependentResource(
      pRenderTarget, D2D1::RectF(0.0f, 0.0f, GetWidth(), GetHeight()), 1.0f));
  CHK_FATAL_HRESULT(m_innerShadowBrush->CreateDeviceDependentResource(
      pRenderTarget, D2D1::RectF(0.0f, 0.0f, GetWidth(), GetHeight()), 1.0f));
  CHK_FATAL_HRESULT(m_backgroundBrush->CreateDeviceDependentResource(
      pRenderTarget, D2D1::RectF(0.0f, 0.0f, GetWidth(), GetHeight()), 1.0f));
  CHK_FATAL_HRESULT(m_titleForegroundBrush->CreateDeviceDependentResource(
      pRenderTarget, D2D1::RectF(0.0f, 0.0f, GetWidth(), GetHeight()), 1.0f));
  CHK_FATAL_HRESULT(m_titleBackgroundBrush->CreateDeviceDependentResource(
      pRenderTarget, D2D1::RectF(0.0f, 0.0f, GetWidth(), GetHeight()), 1.0f));

  return S_OK;
}

HRESULT ui::UIInternalDialog::Render(graphics::D3DInteropHelper *pD3DInteropHelper, ID2D1RenderTarget *pRenderTarget) {
  pRenderTarget->FillRectangle(D2D1::RectF(0, 0, GetWidth(), GetHeight()), m_outerShineBrush->GetBrush());
  pRenderTarget->FillRectangle(D2D1::RectF(1.0f, 1.0f, GetWidth() - 1.0f, GetHeight() - 1.0f),
                               m_innerShadowBrush->GetBrush());
  pRenderTarget->FillRectangle(D2D1::RectF(2.0f, 2.0f, GetWidth() - 2.0f, GetHeight() - 2.0f),
                               m_backgroundBrush->GetBrush());

  pRenderTarget->FillRectangle(D2D1::RectF(2.0f, 2.0f, GetWidth() - 2.0f, m_textMetrics.height + 2.0f + TEXT_MARGIN),
                               m_titleBackgroundBrush->GetBrush());
  pRenderTarget->DrawTextLayout(D2D1::Point2F(2.0f + TEXT_MARGIN, 2.0f), m_textLayout,
                                m_titleForegroundBrush->GetBrush());

  D2D1_MATRIX_3X2_F origMatrix;
  pRenderTarget->GetTransform(&origMatrix);
  D2D1_MATRIX_3X2_F matrix
      = origMatrix * D2D1::Matrix3x2F::Translation(0.0f, m_textMetrics.height + 2.0f + TEXT_MARGIN);
  pRenderTarget->SetTransform(matrix);
  for (size_t i = 0; i < GetNumberOfElements(); ++i) {
    LOG(SEVERITY_LEVEL_DEBUG) << GetElement(i)->GetBounds();
  }
  UIContainer::Render(pD3DInteropHelper, pRenderTarget);
  pRenderTarget->SetTransform(origMatrix);

  return S_OK;
}

void ui::UIInternalDialog::NotifyFocusObtained(const std::shared_ptr<UIBase> &uiBase) {
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

void ui::UIInternalDialog::DiscardDeviceDependentResources() {
  m_outerShineBrush->DiscardDeviceDependentResource();
  m_innerShadowBrush->DiscardDeviceDependentResource();
  m_backgroundBrush->DiscardDeviceDependentResource();
  m_titleForegroundBrush->DiscardDeviceDependentResource();
  m_titleBackgroundBrush->DiscardDeviceDependentResource();
}

void ui::UIInternalDialog::CloseDialog() {
  auto container = GetParentContainer();
  if (!container.expired()) {
    auto root = std::dynamic_pointer_cast<UIRoot>(container.lock());
    if (root.get() != nullptr) {
      root->NotifyDialogClosed();
    }
  }
  m_dialogStatus = DIALOG_STATUS_CLOSE;
}

/* static */ void ui::UIInternalDialog::CloseDialog(ui::event::UIActionEvent event) {
  auto uiBaseWeak = event.GetSource();
  if (!uiBaseWeak.expired()) {
    auto uiContainerWeak = uiBaseWeak.lock()->GetParentContainer();
    if (!uiContainerWeak.expired()) {
      auto dialog = std::dynamic_pointer_cast<UIInternalDialog>(uiContainerWeak.lock());
      if (dialog.get() != nullptr) {
        dialog->CloseDialog();
      }
    }
  }
}

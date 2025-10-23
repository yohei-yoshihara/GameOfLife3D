#include "StdAfx.h"
#include "Common.h"
#include "graphics/D3DInteropHelper.h"
#include "graphics/brush/LinearGradientBrush.h"
#include "ui/UILabel.h"

// #define DEBUG_UILABEL

ui::UILabel::UILabel(void)
    : m_text()
    , m_fontAttribute()
    , m_foregroundColorBrush(std::make_shared<graphics::brush::LinearGradientBrush>())
    , m_textFormat()
    , m_textLayout()
    , m_textMetrics() {
  SetFocusable(false);
  m_foregroundColorBrush->AddGradientStop(D2D1::GradientStop(0.0, D2D1::ColorF(D2D1::ColorF::White, 1.0f)));
  m_foregroundColorBrush->AddGradientStop(D2D1::GradientStop(1.0, D2D1::ColorF(D2D1::ColorF::White, 1.0f)));
  m_foregroundColorBrush->SetGradientDirection(graphics::GRADIENT_DIRECTION_TOP_BOTTOM);
}

ui::UILabel::~UILabel(void) {
}

HRESULT ui::UILabel::Initialize(graphics::D3DInteropHelper *pD3DInteropHelper) {
  m_textFormat = nullptr;
  m_textLayout = nullptr;

  // text format
  CHK_FATAL_HRESULT(pD3DInteropHelper->GetDWriteFactory()->CreateTextFormat(
      m_fontAttribute.fontFamilyName.c_str(), nullptr, m_fontAttribute.GetFontWeight(), m_fontAttribute.GetFontStyle(),
      m_fontAttribute.GetFontStretch(), m_fontAttribute.fontSize, L"", &m_textFormat));

  // text layout
  CHK_FATAL_HRESULT(pD3DInteropHelper->GetDWriteFactory()->CreateTextLayout(
      m_text.c_str(), static_cast<UINT>(m_text.size()), m_textFormat, GetWidth(), GetHeight(), &m_textLayout));

  IDWriteInlineObject *inlineObject = NULL;
  CHK_FATAL_HRESULT(pD3DInteropHelper->GetDWriteFactory()->CreateEllipsisTrimmingSign(m_textLayout, &inlineObject));
  const static DWRITE_TRIMMING trimming = {DWRITE_TRIMMING_GRANULARITY_CHARACTER, 0, 0};
  m_textLayout->SetTrimming(&trimming, inlineObject);
  SafeRelease(&inlineObject);

#ifdef DEBUG_UILABEL
  IDWriteInlineObject *inline3;
  DWRITE_TRIMMING trimming3;
  m_textLayout->GetTrimming(&trimming3, &inline3);
  LOG(SEVERITY_LEVEL_DEBUG) << L"trimming " << trimming3.granularity << L", " << trimming3.delimiter << L", "
                            << trimming3.delimiterCount;
  SafeRelease(&inline3);
#endif

  // metrics
  CHK_FATAL_HRESULT(m_textLayout->GetMetrics(&m_textMetrics));
  return S_OK;
}

HRESULT ui::UILabel::CreateDeviceDependentResources(graphics::D3DInteropHelper *pD3DInteropHelper,
                                                    ID2D1RenderTarget *pRenderTarget) {
  UNREFERENCED_PARAMETER(pD3DInteropHelper);
  CHK_FATAL_HRESULT(m_foregroundColorBrush->CreateDeviceDependentResource(
      pRenderTarget, D2D1::RectF(0.0f, 0.0f, GetWidth(), GetHeight()), 1.0f));
  return S_OK;
}

void ui::UILabel::DiscardDeviceDependentResources() {
  m_foregroundColorBrush->DiscardDeviceDependentResource();
}

void ui::UILabel::ClearResources() {
  m_textFormat = nullptr;
  m_textLayout = nullptr;
}

void ui::UILabel::SetText(const std::wstring &text) {
  m_text = text;
  ClearResources();
}

void ui::UILabel::SetForegroundColor(const std::vector<D2D1_GRADIENT_STOP> &foregroundColors,
                                     graphics::GRADIENT_DIRECTION fgGradientDirection) {
  m_foregroundColorBrush->SetGradientStops(foregroundColors);
  m_foregroundColorBrush->SetGradientDirection(fgGradientDirection);
}

HRESULT ui::UILabel::Render(graphics::D3DInteropHelper *pD3DInteropHelper, ID2D1RenderTarget *pRenderTarget) {
  UNREFERENCED_PARAMETER(pD3DInteropHelper);
  if (m_textFormat == nullptr || m_textLayout == nullptr) {
    HRESULT hr = Initialize(pD3DInteropHelper);
    if (FAILED(hr)) {
      LOG(SEVERITY_LEVEL_ERROR) << L"CreateDeviceDependentResources failed, hr = " << hr;
      return hr;
    }
  }
  pRenderTarget->DrawTextLayout(D2D1::Point2F(0.0f, 0.0f), m_textLayout, m_foregroundColorBrush->GetBrush(),
                                D2D1_DRAW_TEXT_OPTIONS_CLIP);
  return S_OK;
}

void ui::UILabel::SetFontAttribute(const graphics::FontAttribute &fontAttribute) {
  m_fontAttribute = fontAttribute;
  ClearResources();
}

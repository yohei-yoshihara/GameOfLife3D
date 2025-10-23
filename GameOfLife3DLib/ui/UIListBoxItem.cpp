#include "StdAfx.h"
#include "Common.h"
#include "graphics/DirectWriteUtil.h"
#include "graphics/D3DInteropHelper.h"
#include "graphics/brush/BrushSet.h"
#include "graphics/color/ColorSet.h"
#include "graphics/color/SolidColor.h"
#include "graphics/color/LinearGradientColor.h"
#include "graphics/decorator/FrameDecorator.h"
#include "graphics/figure/RectangleFigure.h"
#include "ui/UIScrollBar.h"
#include "ui/UIListBoxItem.h"

// #define DEBUG_UILISTBOXITEM

template <typename T>
T max3(T v1, T v2, T v3) {
  return std::max(v1, std::max(v2, v3));
}

//const wchar_t *UILISTBOXITEM_DEFAULT_TEXT_COLORSET = L"solid[mainTextBrush](rgba(#ffffff,1.0)),"
//                                                     L"solid[subText1Brush](rgba(#ffffff,1.0)),"
//                                                     L"solid[subText2Brush](rgba(#ffffff,1.0)),"
//                                                     L"solid[backgroundBrush](rgba(#000000,1.0))";

//const wchar_t *UILISTBOXITEM_DEFAULT_BODY_COLORSET
//    = L"linear[normal](TOP_BOTTOM,rgba(#404040,1.0)@0.0,rgba(#2a2a2a,1.0)@1.0),"
//      L"linear[mouseover](TOP_BOTTOM,rgba(#505050,1.0)@0.0,rgba(#3a3a3a,1.0)@1.0)";

//const wchar_t *UILISTBOXITEM_DEFAULT_FRAME_COLORSET = L"solid(rgba(#353535,1.0))";

// コンストラクタ
ui::UIListBoxItem::UIListBoxItem(void)
    : m_layoutDirection(LAYOUT_DIRECTION_VERTICAL)
    , m_preferredTextAreaSize(0.0f, 0.0f)
    , m_preferredBitmapSize(64.0f, 64.0f)
    , m_marginBetweenBitmapAndText(4.0f)
    , m_textColorSet(std::make_shared<graphics::color::ColorSet>(/*UILISTBOXITEM_DEFAULT_TEXT_COLORSET*/))
    , m_textBrushSet(std::make_unique<graphics::brush::BrushSet>())
    , m_bodyColorSet(std::make_shared<graphics::color::ColorSet>(/*UILISTBOXITEM_DEFAULT_BODY_COLORSET*/))
    , m_bodyBrushSet(std::make_unique<graphics::brush::BrushSet>())
    , m_bodyRect(std::make_unique<graphics::figure::RectangleFigure>())
    , m_mainText()
    , m_mainTextFontAttribute(12.0f)
    , m_mainTextFormat()
    , m_mainTextLayout()
    , m_mainTextMetrics()
    , m_mainTextLayoutRect()
    , m_subText1()
    , m_subText1FontAttribute(10.0f)
    , m_subText1Format()
    , m_subText1Layout()
    , m_subText1Metrics()
    , m_subText2()
    , m_subText2FontAttribute(10.0f)
    , m_subText2Format()
    , m_subText2Layout()
    , m_subText2Metrics()
    , m_bitmap(std::make_shared<graphics::Bitmap>())
    , m_bitmapLayoutRect(0.0f, 0.0f, 64.0f, 64.0f)
    , m_textLayoutRect(0.0f, 0.0f, 128.0f, 0.0f)
    , m_textChanged(false)
    , m_resourceChanged(false)
    , m_mainTextNumberOfLines(2)
    , m_scrollBar(std::unique_ptr<ui::UIScrollBar>()) {
#ifdef DEBUG_UILISTBOXITEM
  LOG_ENTER(SEVERITY_LEVEL_DEBUG);
#endif

  // L"solid[mainTextBrush](rgba(#ffffff,1.0)),"
  // L"solid[subText1Brush](rgba(#ffffff,1.0)),"
  // L"solid[subText2Brush](rgba(#ffffff,1.0)),"
  // L"solid[backgroundBrush](rgba(#000000,1.0))";
  m_textColorSet->AddColor(std::make_shared<graphics::color::SolidColor>(graphics::color::ColorValue(graphics::color::ColorValue::COLOR_TYPE_RGBA, 0xffffff, 1.0), L"mainTextBrush"));
  m_textColorSet->AddColor(std::make_shared<graphics::color::SolidColor>(graphics::color::ColorValue(graphics::color::ColorValue::COLOR_TYPE_RGBA, 0xffffff, 1.0), L"subText1Brush"));
  m_textColorSet->AddColor(std::make_shared<graphics::color::SolidColor>(graphics::color::ColorValue(graphics::color::ColorValue::COLOR_TYPE_RGBA, 0xffffff, 1.0), L"subText2Brush"));
  m_textColorSet->AddColor(std::make_shared<graphics::color::SolidColor>(graphics::color::ColorValue(graphics::color::ColorValue::COLOR_TYPE_RGBA, 0x000000, 1.0), L"backgroundBrush"));

  // L"linear[normal](TOP_BOTTOM,rgba(#404040,1.0)@0.0,rgba(#2a2a2a,1.0)@1.0),"
  // L"linear[mouseover](TOP_BOTTOM,rgba(#505050,1.0)@0.0,rgba(#3a3a3a,1.0)@1.0)";
  graphics::color::GradientStopCollection normalGradientStops;
  normalGradientStops.AddGradientStop(graphics::color::GradientStop(0.0, graphics::color::ColorValue(graphics::color::ColorValue::COLOR_TYPE_RGBA, 0x404040, 1.0)));
  normalGradientStops.AddGradientStop(graphics::color::GradientStop(1.0, graphics::color::ColorValue(graphics::color::ColorValue::COLOR_TYPE_RGBA, 0x2a2a2a, 1.0)));
  auto normalGradient = std::make_shared<graphics::color::LinearGradientColor>();
  normalGradient->SetGradientDirection(graphics::GRADIENT_DIRECTION_TOP_BOTTOM);
  normalGradient->SetColor(normalGradientStops);
  normalGradient->SetLabel(L"normal");
  m_bodyColorSet->AddColor(normalGradient);

  graphics::color::GradientStopCollection mouseoverGradientStops;
  mouseoverGradientStops.AddGradientStop(graphics::color::GradientStop(0.0, graphics::color::ColorValue(graphics::color::ColorValue::COLOR_TYPE_RGBA, 0x505050, 1.0)));
  mouseoverGradientStops.AddGradientStop(graphics::color::GradientStop(1.0, graphics::color::ColorValue(graphics::color::ColorValue::COLOR_TYPE_RGBA, 0x3a3a3a, 1.0)));
  auto mouseoverGradient = std::make_shared<graphics::color::LinearGradientColor>();
  mouseoverGradient->SetGradientDirection(graphics::GRADIENT_DIRECTION_TOP_BOTTOM);
  mouseoverGradient->SetColor(normalGradientStops);
  mouseoverGradient->SetLabel(L"mouseover");
  m_bodyColorSet->AddColor(mouseoverGradient);

  SetFocusable(true);

#ifdef DEBUG_UILISTBOXITEM
  LOG_LEAVE(SEVERITY_LEVEL_DEBUG);
#endif
}

// デストラクタ
ui::UIListBoxItem::~UIListBoxItem(void) {
#ifdef DEBUG_UILISTBOXITEM
  LOG_ENTER(SEVERITY_LEVEL_DEBUG);
  LOG_LEAVE(SEVERITY_LEVEL_DEBUG);
#endif
}

// 初期化
HRESULT ui::UIListBoxItem::Initialize(graphics::D3DInteropHelper *pD3DInteropHelper) {
#ifdef DEBUG_UILISTBOXITEM
  LOG_ENTER(SEVERITY_LEVEL_DEBUG);
#endif
  // ブラシセットの初期化
  m_textBrushSet->SetColorSet(m_textColorSet);
  // メインテキストの初期化
  CHK_WARN_HRESULT(__InitializeMainText(pD3DInteropHelper));
  // サブテキスト1の初期化
  CHK_WARN_HRESULT(__InitializeSubText(pD3DInteropHelper, m_subText1, m_subText1FontAttribute, m_subText1Format,
                                       m_subText1Layout, m_subText1Metrics, m_subText1LayoutRect));
  // サブテキスト2の初期化
  CHK_WARN_HRESULT(__InitializeSubText(pD3DInteropHelper, m_subText2, m_subText2FontAttribute, m_subText2Format,
                                       m_subText2Layout, m_subText2Metrics, m_subText2LayoutRect));
#ifdef DEBUG_UILISTBOXITEM
  SPDLOG_DEBUG( << L"m_mainTextLayout->GetMaxWidth() = " << m_mainTextLayout->GetMaxWidth();
  SPDLOG_DEBUG( << L"m_mainTextLayout->GetMaxHeight() = " << m_mainTextLayout->GetMaxHeight();
  SPDLOG_DEBUG( << L"m_subText1Layout->GetMaxWidth() = " << m_subText1Layout->GetMaxWidth();
  SPDLOG_DEBUG( << L"m_subText1Layout->GetMaxHeight() = " << m_subText1Layout->GetMaxHeight();
  SPDLOG_DEBUG( << L"m_subText2Layout->GetMaxWidth() = " << m_subText2Layout->GetMaxWidth();
  SPDLOG_DEBUG( << L"m_subText2Layout->GetMaxHeight() = " << m_subText2Layout->GetMaxHeight();
  SPDLOG_DEBUG( << L"m_mainTextMetrics = " << m_mainTextMetrics;
  SPDLOG_DEBUG( << L"m_subText1Metrics = " << m_subText1Metrics;
  SPDLOG_DEBUG( << L"m_subText2Metrics = " << m_subText2Metrics;
#endif

  __CalculateLayoutRect();

  // ボディ
  m_bodyBrushSet->SetColorSet(m_bodyColorSet);
  m_bodyRect->SetX(0.0f);
  m_bodyRect->SetY(0.0f);
  m_bodyRect->SetWidth(GetWidth());
  m_bodyRect->SetHeight(GetHeight());
  auto frameDecorator = std::make_shared<graphics::decorator::FrameDecorator>();
  m_bodyRect->Initialize(pD3DInteropHelper);

#ifdef DEBUG_UILISTBOXITEM
  LOG_LEAVE(SEVERITY_LEVEL_DEBUG);
#endif
  return S_OK;
}

// メインテキストの初期化（テキストフォーマット、テキストレイアウト、テキストメトリックス）
HRESULT ui::UIListBoxItem::__InitializeMainText(graphics::D3DInteropHelper *pD3DInteropHelper) {
#ifdef DEBUG_UILISTBOXITEM
  LOG_ENTER(SEVERITY_LEVEL_DEBUG);
#endif

  // text format
  m_mainTextFormat = nullptr;
  CHK_FATAL_HRESULT(pD3DInteropHelper->GetDWriteFactory()->CreateTextFormat(
      m_mainTextFontAttribute.fontFamilyName.c_str(), nullptr, m_mainTextFontAttribute.GetFontWeight(),
      m_mainTextFontAttribute.GetFontStyle(), m_mainTextFontAttribute.GetFontStretch(),
      m_mainTextFontAttribute.fontSize, L"", &m_mainTextFormat));

  // text layout
  m_mainTextLayout = nullptr;
  CHK_FATAL_HRESULT(graphics::DirectWriteUtil::CreateTextLayoutWithTrimming(
      pD3DInteropHelper->GetDWriteFactory(), m_mainTextFormat, m_mainText.c_str(), m_preferredTextAreaSize.width,
      m_mainTextNumberOfLines, &m_mainTextLayout));
  // metrics
  CHK_FATAL_HRESULT(m_mainTextLayout->GetMetrics(&m_mainTextMetrics));

#ifdef DEBUG_UILISTBOXITEM
  graphics::DirectWriteUtil::DumpMetrics(SEVERITY_LEVEL_DEBUG, L"m_mainTextLayout", m_mainTextLayout);
#endif
  m_mainTextLayoutRect.width = m_textLayoutRect.width;
  m_mainTextLayoutRect.height = m_mainTextMetrics.height;

#ifdef DEBUG_UILISTBOXITEM
  LOG_LEAVE(SEVERITY_LEVEL_DEBUG);
#endif
  return S_OK;
}

// サブテキストの初期化（テキストフォーマット、テキストレイアウト、テキストメトリックス）
HRESULT ui::UIListBoxItem::__InitializeSubText(graphics::D3DInteropHelper *pD3DInteropHelper, const std::wstring &text,
                                               const graphics::FontAttribute &fontAttribute,
                                               CComPtr<IDWriteTextFormat> &textFormat,
                                               CComPtr<IDWriteTextLayout> &textLayout, DWRITE_TEXT_METRICS &textMetrics,
                                               UIRectangle &textRect) {
#ifdef DEBUG_UILISTBOXITEM
  LOG_ENTER(SEVERITY_LEVEL_DEBUG);
#endif

  // text format
  textFormat = nullptr;
  CHK_FATAL_HRESULT(pD3DInteropHelper->GetDWriteFactory()->CreateTextFormat(
      fontAttribute.fontFamilyName.c_str(), nullptr, fontAttribute.GetFontWeight(), fontAttribute.GetFontStyle(),
      fontAttribute.GetFontStretch(), fontAttribute.fontSize, L"", &textFormat));

  textRect.width = m_textLayoutRect.width;

  // text layout
  textLayout = nullptr;
  CHK_FATAL_HRESULT(pD3DInteropHelper->GetDWriteFactory()->CreateTextLayout(
      text.c_str(), static_cast<UINT>(text.size()), textFormat, m_preferredTextAreaSize.width,
      std::numeric_limits<FLOAT>::max(), &textLayout));

  // trimming sign
  CComPtr<IDWriteInlineObject> inlineObject;
  CHK_FATAL_HRESULT(pD3DInteropHelper->GetDWriteFactory()->CreateEllipsisTrimmingSign(textLayout, &inlineObject));
  DWRITE_TRIMMING trimming = {DWRITE_TRIMMING_GRANULARITY_CHARACTER, 0, 0};
  textLayout->SetTrimming(&trimming, inlineObject);
  textLayout->SetWordWrapping(DWRITE_WORD_WRAPPING_NO_WRAP);

  // text metrics
  CHK_FATAL_HRESULT(textLayout->GetMetrics(&textMetrics));

#ifdef DEBUG_UILISTBOXITEM
  graphics::DirectWriteUtil::DumpMetrics(SEVERITY_LEVEL_DEBUG, L"m_subTextLayout", textLayout);
  LOG_LEAVE(SEVERITY_LEVEL_DEBUG);
#endif
  return S_OK;
}

// レイアウト領域の計算
// 実行される前に、テキストレイアウトの生成とメトリックスの取得が行われていなければならない。
void ui::UIListBoxItem::__CalculateLayoutRect() {
#ifdef DEBUG_UILISTBOXITEM
  LOG_ENTER(SEVERITY_LEVEL_DEBUG);
#endif
  if (m_layoutDirection == LAYOUT_DIRECTION_HORIZONTAL) {
    // ビットマップ
    m_bitmapLayoutRect.x = 0.0f;
    m_bitmapLayoutRect.y = 0.0f;
    m_bitmapLayoutRect.width = m_preferredBitmapSize.width;
    m_bitmapLayoutRect.height = m_preferredBitmapSize.height;

    // テキストレイアウト (x, y)
    m_textLayoutRect.x = m_bitmapLayoutRect.x + m_bitmapLayoutRect.width + m_marginBetweenBitmapAndText;
    m_textLayoutRect.y = m_bitmapLayoutRect.y;

    // メインテキストレイアウト
    m_mainTextLayoutRect.x = m_textLayoutRect.x;
    m_mainTextLayoutRect.y = m_textLayoutRect.y;
    m_mainTextLayoutRect.width = m_mainTextMetrics.widthIncludingTrailingWhitespace;
    m_mainTextLayoutRect.height = m_mainTextMetrics.height;

    // サブテキスト1レイアウト
    m_subText1LayoutRect.x = m_mainTextLayoutRect.x;
    m_subText1LayoutRect.y = m_mainTextLayoutRect.y + m_mainTextLayoutRect.height;
    m_subText1LayoutRect.width = m_subText1Metrics.widthIncludingTrailingWhitespace;
    m_subText1LayoutRect.height = m_subText1Metrics.height;

    // サブテキスト2レイアウト
    m_subText2LayoutRect.x = m_mainTextLayoutRect.x;
    m_subText2LayoutRect.y = m_subText1LayoutRect.y + m_subText1Metrics.height;
    m_subText2LayoutRect.width = m_subText2Metrics.widthIncludingTrailingWhitespace;
    m_subText2LayoutRect.height = m_subText2Metrics.height;

    // テキストレイアウト (width, height)
    m_textLayoutRect.width = max3(m_mainTextLayoutRect.width, m_subText1LayoutRect.width, m_subText2LayoutRect.width);
    m_textLayoutRect.height = m_mainTextLayoutRect.height + m_subText1LayoutRect.height + m_subText2LayoutRect.height;

//         UIRectangle bounds = GetBounds();
//         bounds.width = m_bitmapLayoutRect.width +
//                        m_textLayoutRect.width +
//                        m_marginBetweenBitmapAndText;
//         bounds.height = std::max(m_bitmapLayoutRect.height,
//                                  m_textLayoutRect.height);
//         SetBounds(bounds);
#ifdef DEBUG_UILISTBOXITEM
    SPDLOG_DEBUG( << L"bounds = " << bounds;
#endif
  } else {
    // ビットマップ
    m_bitmapLayoutRect.x = 0.0f;
    m_bitmapLayoutRect.y = 0.0f;
    m_bitmapLayoutRect.width = m_preferredBitmapSize.width;
    m_bitmapLayoutRect.height = m_preferredBitmapSize.height;

    // テキストレイアウト (x, y)
    m_textLayoutRect.x = m_bitmapLayoutRect.x;
    m_textLayoutRect.y = m_bitmapLayoutRect.y + m_bitmapLayoutRect.height + m_marginBetweenBitmapAndText;

    // メインテキストレイアウト
    m_mainTextLayoutRect.x = m_textLayoutRect.x;
    m_mainTextLayoutRect.y = m_textLayoutRect.y;
    m_mainTextLayoutRect.width = m_mainTextMetrics.widthIncludingTrailingWhitespace;
    m_mainTextLayoutRect.height = m_mainTextMetrics.height;

    // サブテキスト1レイアウト
    m_subText1LayoutRect.x = m_mainTextLayoutRect.x;
    m_subText1LayoutRect.y = m_mainTextLayoutRect.y + m_mainTextLayoutRect.height;
    m_subText1LayoutRect.width = m_subText1Metrics.widthIncludingTrailingWhitespace;
    m_subText1LayoutRect.height = m_subText1Metrics.height;

    // サブテキスト2レイアウト
    m_subText2LayoutRect.x = m_mainTextLayoutRect.x;
    m_subText2LayoutRect.y = m_subText1LayoutRect.y + m_subText1Metrics.height;
    m_subText2LayoutRect.width = m_subText2Metrics.widthIncludingTrailingWhitespace;
    m_subText2LayoutRect.height = m_subText2Metrics.height;

    // テキストレイアウト (width, height)
    m_textLayoutRect.width = max3(m_mainTextLayoutRect.width, m_subText1LayoutRect.width, m_subText2LayoutRect.width);
    m_textLayoutRect.height = m_mainTextLayoutRect.height + m_subText1LayoutRect.height + m_subText2LayoutRect.height;

//         UIRectangle bounds = GetBounds();
//         bounds.width = std::max(m_bitmapLayoutRect.width,
//                                 m_textLayoutRect.width);
//         bounds.height = m_bitmapLayoutRect.height +
//                         m_textLayoutRect.height +
//                         m_marginBetweenBitmapAndText;
//         SetBounds(bounds);
#ifdef DEBUG_UILISTBOXITEM
    SPDLOG_DEBUG( << L"bounds = " << bounds;
#endif
  }

#ifdef DEBUG_UILISTBOXITEM
  LOG_LEAVE(SEVERITY_LEVEL_DEBUG);
#endif
}

HRESULT ui::UIListBoxItem::CreateDeviceDependentResources(graphics::D3DInteropHelper *pD3DInteropHelper,
                                                          ID2D1RenderTarget *pRenderTarget) {
#ifdef DEBUG_UILISTBOXITEM
  LOG_ENTER(SEVERITY_LEVEL_DEBUG);
#endif

  CHK_FATAL_HRESULT(m_textBrushSet->CreateDeviceDependentResource(pRenderTarget, D2D1::RectF(), 1.0f));
  CHK_FATAL_HRESULT(m_bodyRect->CreateDeviceDependentResources(pD3DInteropHelper, pRenderTarget));

#ifdef DEBUG_UILISTBOXITEM
  LOG_LEAVE(SEVERITY_LEVEL_DEBUG);
#endif
  return S_OK;
}

HRESULT ui::UIListBoxItem::Render(graphics::D3DInteropHelper *pD3DInteropHelper, ID2D1RenderTarget *pRenderTarget) {
#ifdef DEBUG_UILISTBOXITEM
  LOG_ENTER(SEVERITY_LEVEL_DEBUG);
#endif
  if (m_textChanged) {
    __RecreateAllTextLayout(pD3DInteropHelper, pRenderTarget);
    m_textChanged = false;
  }
  if (m_resourceChanged) {
    DiscardDeviceDependentResources();
    CreateDeviceDependentResources(pD3DInteropHelper, pRenderTarget);
    m_resourceChanged = true;
  }

  m_bodyRect->Render(pD3DInteropHelper, pRenderTarget, 1.0f);

  if (!m_bitmap->IsLoaded()) {
    m_bitmap->Load(pRenderTarget, pD3DInteropHelper->GetWICImagingFactory());
  }
  pRenderTarget->DrawBitmap(m_bitmap->GetBitmap(), m_bitmapLayoutRect.ToRectF());
  pRenderTarget->DrawTextLayout(D2D1::Point2F(m_mainTextLayoutRect.x, m_mainTextLayoutRect.y), m_mainTextLayout,
                                m_textBrushSet->Get(L"mainTextBrush")->GetBrush());
  pRenderTarget->DrawTextLayout(D2D1::Point2F(m_subText1LayoutRect.x, m_subText1LayoutRect.y), m_subText1Layout,
                                m_textBrushSet->Get(L"subText1Brush")->GetBrush());
  pRenderTarget->DrawTextLayout(D2D1::Point2F(m_subText2LayoutRect.x, m_subText2LayoutRect.y), m_subText2Layout,
                                m_textBrushSet->Get(L"subText2Brush")->GetBrush());

//     CComPtr<ID2D1SolidColorBrush> redBrush = nullptr;
//     pRenderTarget->CreateSolidColorBrush(D2D1::ColorF(D2D1::ColorF::Red), D2D1::BrushProperties(), &redBrush);
//     SPDLOG_DEBUG( << L"width = " << GetWidth() << L", height = " << GetHeight();
//     pRenderTarget->DrawRectangle(D2D1::RectF(0.0f, 0.0f, GetWidth(), GetHeight()), redBrush);

#ifdef DEBUG_UILISTBOXITEM
  LOG_LEAVE(SEVERITY_LEVEL_DEBUG);
#endif
  return S_OK;
}

void ui::UIListBoxItem::DiscardDeviceDependentResources() {
#ifdef DEBUG_UILISTBOXITEM
  LOG_ENTER(SEVERITY_LEVEL_DEBUG);
#endif

  m_textBrushSet->DiscardDeviceDependentResource();
  m_bodyBrushSet->DiscardDeviceDependentResource();

#ifdef DEBUG_UILISTBOXITEM
  LOG_LEAVE(SEVERITY_LEVEL_DEBUG);
#endif
}

HRESULT ui::UIListBoxItem::__RecreateAllTextLayout(graphics::D3DInteropHelper *pD3DInteropHelper,
                                                   ID2D1RenderTarget *pRenderTarget) {
#ifdef DEBUG_UILISTBOXITEM
  LOG_ENTER(SEVERITY_LEVEL_DEBUG);
#endif
  CHK_WARN_HRESULT(__RecreateMainTextLayout(pD3DInteropHelper, pRenderTarget, m_mainText, m_mainTextLayout,
                                            m_mainTextMetrics, m_mainTextLayoutRect, m_mainTextNumberOfLines));
  CHK_WARN_HRESULT(__RecreateSubTextLayout(pD3DInteropHelper, pRenderTarget, m_subText1, m_subText1Layout,
                                           m_subText1Metrics, m_subText1LayoutRect));
  CHK_WARN_HRESULT(__RecreateSubTextLayout(pD3DInteropHelper, pRenderTarget, m_subText2, m_subText2Layout,
                                           m_subText2Metrics, m_subText2LayoutRect));
#ifdef DEBUG_UILISTBOXITEM
  LOG_LEAVE(SEVERITY_LEVEL_DEBUG);
#endif
  return S_OK;
}

HRESULT ui::UIListBoxItem::__RecreateMainTextLayout(graphics::D3DInteropHelper *pD3DInteropHelper,
                                                    ID2D1RenderTarget *pRenderTarget, const std::wstring &text,
                                                    CComPtr<IDWriteTextLayout> &textLayout,
                                                    DWRITE_TEXT_METRICS &textMetrics, UIRectangle &textRect,
                                                    size_t numberOfLines) {
#ifdef DEBUG_UILISTBOXITEM
  LOG_ENTER(SEVERITY_LEVEL_DEBUG);
#endif
  UNREFERENCED_PARAMETER(pRenderTarget);
  CComPtr<IDWriteTextLayout> oldTextLayout = textLayout;
  textLayout = nullptr;
  CHK_FATAL_HRESULT(graphics::DirectWriteUtil::CreateTextLayoutWithTrimming(
      pD3DInteropHelper->GetDWriteFactory(), oldTextLayout, text.c_str(), textRect.width, numberOfLines, &textLayout));
  CHK_FATAL_HRESULT(textLayout->GetMetrics(&textMetrics));
#ifdef DEBUG_UILISTBOXITEM
  LOG_LEAVE(SEVERITY_LEVEL_DEBUG);
#endif
  return S_OK;
}

HRESULT ui::UIListBoxItem::__RecreateSubTextLayout(graphics::D3DInteropHelper *pD3DInteropHelper,
                                                   ID2D1RenderTarget *pRenderTarget, const std::wstring &text,
                                                   CComPtr<IDWriteTextLayout> &textLayout,
                                                   DWRITE_TEXT_METRICS &textMetrics, UIRectangle &textRect) {
#ifdef DEBUG_UILISTBOXITEM
  LOG_ENTER(SEVERITY_LEVEL_DEBUG);
#endif
  UNREFERENCED_PARAMETER(pRenderTarget);
  CComPtr<IDWriteTextLayout> oldTextLayout = textLayout;
  textLayout = nullptr;
  CHK_FATAL_HRESULT(pD3DInteropHelper->GetDWriteFactory()->CreateTextLayout(
      text.c_str(), static_cast<UINT32>(text.length()), oldTextLayout, textRect.width, textRect.height, &textLayout));
  CHK_FATAL_HRESULT(textLayout->GetMetrics(&textMetrics));
#ifdef DEBUG_UILISTBOXITEM
  LOG_LEAVE(SEVERITY_LEVEL_DEBUG);
#endif
  return S_OK;
}

void ui::UIListBoxItem::SetMainText(const std::wstring &mainText) {
  m_mainText = mainText;
  m_textChanged = true;
}

void ui::UIListBoxItem::SetSubText1(const std::wstring &subText1) {
  m_subText1 = subText1;
  m_textChanged = true;
}

void ui::UIListBoxItem::SetSubText2(const std::wstring &subText2) {
  m_subText2 = subText2;
  m_textChanged = true;
}

void ui::UIListBoxItem::SetBitmapFileName(LPCTSTR fileName) {
  m_bitmap->SetFileName(fileName);
}

void ui::UIListBoxItem::SetBitmapResource(LPCTSTR resourceName, LPCTSTR resourceType) {
  m_bitmap->SetResource(resourceName, resourceType);
}

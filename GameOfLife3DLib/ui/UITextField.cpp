#include "StdAfx.h"
#include "graphics/D3DInteropHelper.h"
#include "graphics/color/ColorUtil.h"
#include "ui/UIContainer.h"
#include "ui/UITextField.h"

// #define DEBUG_UITEXTFIELD

const FLOAT ui::UITextField::INNER_MARGIN = 4.0f;

using namespace ui;

ui::UITextField::UITextField(void)
    : m_initialized(false)
    , m_resourceCreated(false)
    , m_textDisplayAreaHPos(0.0f)
    , m_textSize(0.0f, 0.0f)
    , m_textStoreACP(nullptr)
    , m_textEditSink(nullptr)
    , m_tsfManager()
    , m_pDocMgr(nullptr)
    , m_pContext(nullptr)
    , m_editCookie(0)
    , m_fontAttribute()
    , m_presetText()
    , m_textChanged(false)
    , m_foregroundColor(D2D1::ColorF(D2D1::ColorF::White))
    , m_backgroundColor(D2D1::ColorF(0x272727, 1.0))
    , m_borderLineColor(D2D1::ColorF(0x3e3e3e, 1.0))
    , m_focusBorderLineColor(D2D1::ColorF(0x999999, 1.0))
    , m_highlightColor(D2D1::ColorF(D2D1::ColorF::Aqua))
    , m_compositionUnderlineColor(D2D1::ColorF(D2D1::ColorF::SkyBlue))
    , m_foregroundColorBrush(nullptr)
    , m_backgroundColorBrush(nullptr)
    , m_borderLineColorBrush(nullptr)
    , m_focusBorderLineColorBrush(nullptr)
    , m_highlightColorBrush(nullptr)
    , m_compositionUnderlineBrush(nullptr)
    , m_compositionSolidUnderlineStrokeStyle(nullptr)
    , m_compositionDotUnderlineStrokeStyle(nullptr)
    , m_compositionDashUnderlineStrokeStyle(nullptr)
    , m_compositionSquiggleUnderlineStrokeStyle(nullptr)
    , m_textFormat(nullptr)
    , m_textLayout(nullptr)
    , m_compositionAttributes() {
#ifdef DEBUG_UITEXTFIELD
  LOG_ENTER(SEVERITY_LEVEL_DEBUG);
#endif

  SetFocusable(true);

#ifdef DEBUG_UITEXTFIELD
  LOG_LEAVE(SEVERITY_LEVEL_DEBUG);
#endif
}

ui::UITextField::~UITextField(void) {
#ifdef DEBUG_UITEXTFIELD
  LOG_ENTER(SEVERITY_LEVEL_DEBUG);
#endif

  if (m_pDocMgr != nullptr) {
    m_pDocMgr->Pop(m_editCookie);
  }

#ifdef DEBUG_UITEXTFIELD
  LOG_LEAVE(SEVERITY_LEVEL_DEBUG);
#endif
}

HRESULT ui::UITextField::Initialize(graphics::D3DInteropHelper *pD3DInteropHelper) {
#ifdef DEBUG_UITEXTFIELD
  LOG_ENTER(SEVERITY_LEVEL_DEBUG);
#endif
  m_textStoreACP = new UITextStoreACP();
  m_textEditSink = new UITextEditSink();

  CHK_WARN_HRESULT(m_textStoreACP->Initialize(shared_from_this()));
  CHK_WARN_HRESULT(m_textEditSink->Initialize(shared_from_this()));

  CHK_FATAL_HRESULT(m_tsfManager->GetThreadMgr()->CreateDocumentMgr(&m_pDocMgr));
  CHK_FATAL_HRESULT(m_pDocMgr->CreateContext(m_tsfManager->GetClientId(), 0,
                                             static_cast<ITextStoreACP *>(m_textStoreACP), &m_pContext, &m_editCookie));
  CHK_FATAL_HRESULT(m_pDocMgr->Push(m_pContext));

  m_textEditSink->SetCategoryMgr(m_tsfManager->GetCatetoryMgr());
  m_textEditSink->SetDisplayAttributeMgr(m_tsfManager->GetDisplayAttributeMgr());
  CHK_FATAL_HRESULT(m_textEditSink->Advise(m_pContext));

  CHK_WARN_HRESULT(CreateDeviceIndependentResources(pD3DInteropHelper));

  m_initialized = true;

  SetText(m_presetText);
  m_presetText.clear();
#ifdef DEBUG_UITEXTFIELD
  LOG_LEAVE(SEVERITY_LEVEL_DEBUG);
#endif
  return S_OK;
}

HRESULT ui::UITextField::CreateDeviceIndependentResources(graphics::D3DInteropHelper *pD3DInteropHelper) {
#ifdef DEBUG_UITEXTFIELD
  LOG_ENTER(SEVERITY_LEVEL_DEBUG);
#endif
  ID2D1Factory *pD2DFactory = pD3DInteropHelper->GetD2DFactory();
  CHK_FATAL_HRESULT(pD2DFactory->CreateStrokeStyle(
      D2D1::StrokeStyleProperties(D2D1_CAP_STYLE_FLAT, D2D1_CAP_STYLE_FLAT, D2D1_CAP_STYLE_FLAT, D2D1_LINE_JOIN_MITER,
                                  10.0f, D2D1_DASH_STYLE_SOLID, 0.0f),
      nullptr, 0, &m_compositionSolidUnderlineStrokeStyle));

  CHK_FATAL_HRESULT(pD2DFactory->CreateStrokeStyle(
      D2D1::StrokeStyleProperties(D2D1_CAP_STYLE_ROUND, D2D1_CAP_STYLE_ROUND, D2D1_CAP_STYLE_ROUND,
                                  D2D1_LINE_JOIN_MITER, 10.0f, D2D1_DASH_STYLE_DOT, 0.0f),
      nullptr, 0, &m_compositionDotUnderlineStrokeStyle));

  CHK_FATAL_HRESULT(pD2DFactory->CreateStrokeStyle(
      D2D1::StrokeStyleProperties(D2D1_CAP_STYLE_FLAT, D2D1_CAP_STYLE_FLAT, D2D1_CAP_STYLE_FLAT, D2D1_LINE_JOIN_MITER,
                                  10.0f, D2D1_DASH_STYLE_DASH, 0.0f),
      nullptr, 0, &m_compositionDashUnderlineStrokeStyle));

  CHK_FATAL_HRESULT(pD2DFactory->CreateStrokeStyle(
      D2D1::StrokeStyleProperties(D2D1_CAP_STYLE_FLAT, D2D1_CAP_STYLE_FLAT, D2D1_CAP_STYLE_FLAT, D2D1_LINE_JOIN_MITER,
                                  10.0f, D2D1_DASH_STYLE_DASH_DOT, 0.0f),
      nullptr, 0, &m_compositionSquiggleUnderlineStrokeStyle));

  CHK_FATAL_HRESULT(pD3DInteropHelper->GetDWriteFactory()->CreateTextFormat(
      m_fontAttribute.fontFamilyName.c_str(), nullptr, m_fontAttribute.GetFontWeight(), m_fontAttribute.GetFontStyle(),
      m_fontAttribute.GetFontStretch(), m_fontAttribute.fontSize, L"", &m_textFormat));

  const std::wstring &text = GetText();
  CHK_FATAL_HRESULT(pD3DInteropHelper->GetDWriteFactory()->CreateTextLayout(
      text.c_str(), static_cast<UINT32>(text.size()), m_textFormat, GetWidth(), GetHeight(), &m_textLayout));

  CHK_FATAL_HRESULT(m_textLayout->GetMetrics(&m_textMetrics));
  m_textSize.width = m_textMetrics.widthIncludingTrailingWhitespace;
  m_textSize.height = m_textMetrics.height;

  CHK_WARN_HRESULT(UpdateDisplayAreaPosition());

#ifdef DEBUG_UITEXTFIELD
  LOG_LEAVE(SEVERITY_LEVEL_DEBUG);
#endif
  return S_OK;
}

HRESULT ui::UITextField::CreateDeviceDependentResources(graphics::D3DInteropHelper *pD3DInteropHelper,
                                                        ID2D1RenderTarget *pRenderTarget) {
#ifdef DEBUG_UITEXTFIELD
  LOG_ENTER(SEVERITY_LEVEL_DEBUG);
#endif
  UNREFERENCED_PARAMETER(pD3DInteropHelper);
  CHK_FATAL_HRESULT(pRenderTarget->CreateSolidColorBrush(m_foregroundColor, &m_foregroundColorBrush));
  CHK_FATAL_HRESULT(pRenderTarget->CreateSolidColorBrush(m_backgroundColor, &m_backgroundColorBrush));
  CHK_FATAL_HRESULT(pRenderTarget->CreateSolidColorBrush(m_borderLineColor, &m_borderLineColorBrush));
  CHK_FATAL_HRESULT(pRenderTarget->CreateSolidColorBrush(m_focusBorderLineColor, &m_focusBorderLineColorBrush));
  CHK_FATAL_HRESULT(pRenderTarget->CreateSolidColorBrush(m_highlightColor, &m_highlightColorBrush));
  CHK_FATAL_HRESULT(pRenderTarget->CreateSolidColorBrush(m_compositionUnderlineColor, &m_compositionUnderlineBrush));
  m_resourceCreated = true;
#ifdef DEBUG_UITEXTFIELD
  LOG_LEAVE(SEVERITY_LEVEL_DEBUG);
#endif
  return S_OK;
}

void ui::UITextField::DiscardDeviceDependentResources() {
#ifdef DEBUG_UITEXTFIELD
  LOG_ENTER(SEVERITY_LEVEL_DEBUG);
#endif
  m_foregroundColorBrush = nullptr;
  m_backgroundColorBrush = nullptr;
  m_borderLineColorBrush = nullptr;
  m_focusBorderLineColorBrush = nullptr;
  m_highlightColorBrush = nullptr;
  m_compositionUnderlineBrush = nullptr;
#ifdef DEBUG_UITEXTFIELD
  LOG_LEAVE(SEVERITY_LEVEL_DEBUG);
#endif
}

HRESULT ui::UITextField::_RecreateTextLayout(graphics::D3DInteropHelper *pD3DInteropHelper,
                                             ID2D1RenderTarget *pRenderTarget) {
#ifdef DEBUG_UITEXTFIELD
  LOG_ENTER(SEVERITY_LEVEL_DEBUG);
#endif
  UNREFERENCED_PARAMETER(pRenderTarget);
  CComPtr<IDWriteTextLayout> oldTextLayout = m_textLayout;
  m_textLayout = nullptr;
  const std::wstring &text = GetText();
  CHK_FATAL_HRESULT(pD3DInteropHelper->GetDWriteFactory()->CreateTextLayout(
      text.c_str(), static_cast<UINT32>(text.length()), oldTextLayout, FLT_MAX, FLT_MAX, &m_textLayout));
  CHK_FATAL_HRESULT(m_textLayout->GetMetrics(&m_textMetrics));
  m_textSize.width = m_textMetrics.widthIncludingTrailingWhitespace;
  m_textSize.height = m_textMetrics.height;

#ifdef DEBUG_UITEXTFIELD
  LOG_LEAVE(SEVERITY_LEVEL_DEBUG);
#endif
  return S_OK;
}

const std::wstring &ui::UITextField::GetText() const {
  if (!m_initialized) {
    return m_presetText;
  } else {
    return m_textStoreACP->GetText();
  }
}

void ui::UITextField::_RenderBackground(graphics::D3DInteropHelper *pD3DInteropHelper,
                                        ID2D1RenderTarget *pRenderTarget) {
#ifdef DEBUG_UITEXTFIELD
  LOG_ENTER(SEVERITY_LEVEL_DEBUG);
#endif
  UNREFERENCED_PARAMETER(pD3DInteropHelper);
  pRenderTarget->FillRectangle(D2D1::RectF(1.0f, 1.0f, GetWidth() - 1.0f, GetHeight() - 1.0f), m_backgroundColorBrush);
#ifdef DEBUG_UITEXTFIELD
  LOG_LEAVE(SEVERITY_LEVEL_DEBUG);
#endif
}

void ui::UITextField::_RenderBox(graphics::D3DInteropHelper *pD3DInteropHelper, ID2D1RenderTarget *pRenderTarget) {
#ifdef DEBUG_UITEXTFIELD
  LOG_ENTER(SEVERITY_LEVEL_DEBUG);
#endif
  UNREFERENCED_PARAMETER(pD3DInteropHelper);
  pRenderTarget->DrawRectangle(D2D1::RectF(1.0f, 1.0f, GetWidth() - 1.0f, GetHeight() - 1.0f), m_borderLineColorBrush);
  if (HasFocus()) {
    pRenderTarget->DrawRectangle(D2D1::RectF(0.0f, 0.0f, GetWidth(), GetHeight()), m_focusBorderLineColorBrush);
  }
#ifdef DEBUG_UITEXTFIELD
  LOG_LEAVE(SEVERITY_LEVEL_DEBUG);
#endif
}

HRESULT ui::UITextField::_RenderText(graphics::D3DInteropHelper *pD3DInteropHelper, ID2D1RenderTarget *pRenderTarget,
                                     UIRectangle textRect) {
#ifdef DEBUG_UITEXTFIELD
  LOG_ENTER(SEVERITY_LEVEL_DEBUG);
#endif
  D2D1_POINT_2F point = D2D1::Point2F(textRect.x - m_textDisplayAreaHPos, textRect.y);

  const std::wstring &text = GetText();
  CComPtr<IDWriteTextLayout> pTextLayout;
  CHK_FATAL_HRESULT(pD3DInteropHelper->GetDWriteFactory()->CreateTextLayout(
      text.c_str(), static_cast<UINT32>(text.length()), m_textLayout, FLT_MAX, FLT_MAX, &pTextLayout));

  for (auto it = m_compositionAttributes.begin(); it != m_compositionAttributes.end(); ++it) {
    CompositionAttribute &compositionAttribute = *it;
    if (compositionAttribute.startPos >= 0 && compositionAttribute.endPos >= 0
        && compositionAttribute.startPos != compositionAttribute.endPos) {
      UINT32 actualHitTextCount = 0;
      m_textLayout->HitTestTextRange(compositionAttribute.startPos,
                                     compositionAttribute.endPos - compositionAttribute.startPos, point.x, point.y,
                                     nullptr, 0, &actualHitTextCount);
      if (actualHitTextCount > 0) {
        std::vector<DWRITE_HIT_TEST_METRICS> hitTestMetrics(actualHitTextCount);

        CHK_FATAL_HRESULT(m_textLayout->HitTestTextRange(
            compositionAttribute.startPos, compositionAttribute.endPos - compositionAttribute.startPos, point.x,
            point.y, hitTestMetrics.data(), actualHitTextCount, &actualHitTextCount));
        for (size_t i = 0; i < actualHitTextCount; i++) {
          D2D1_POINT_2F startPoint
              = D2D1::Point2F(hitTestMetrics[i].left, hitTestMetrics[i].top + hitTestMetrics[i].height * 0.9f);
          D2D1_POINT_2F endPoint = D2D1::Point2F(hitTestMetrics[i].left + hitTestMetrics[i].width,
                                                 hitTestMetrics[i].top + hitTestMetrics[i].height * 0.9f);
          // line color
          CComPtr<ID2D1SolidColorBrush> pLineColorBrush;
          if (compositionAttribute.displayAttribute.crLine.type != TF_CT_NONE) {
            COLORREF colorref;
            if (compositionAttribute.displayAttribute.crLine.type == TF_CT_SYSCOLOR) {
              colorref = GetSysColor(compositionAttribute.displayAttribute.crLine.nIndex);
            } else {
              colorref = compositionAttribute.displayAttribute.crLine.cr;
            }
            CHK_FATAL_HRESULT(pRenderTarget->CreateSolidColorBrush(
                graphics::color::ColorUtil::ColorrefToColorF(colorref), &pLineColorBrush));
          }

          // text color
          if (compositionAttribute.displayAttribute.crText.type != TF_CT_NONE) {
            COLORREF colorref;
            if (compositionAttribute.displayAttribute.crText.type == TF_CT_SYSCOLOR) {
              colorref = GetSysColor(compositionAttribute.displayAttribute.crText.nIndex);
            } else {
              colorref = compositionAttribute.displayAttribute.crText.cr;
            }
            CComPtr<ID2D1SolidColorBrush> pTextColorBrush;
            CHK_FATAL_HRESULT(pRenderTarget->CreateSolidColorBrush(
                graphics::color::ColorUtil::ColorrefToColorF(colorref), &pTextColorBrush));
            DWRITE_TEXT_RANGE textRange;
            textRange.startPosition = compositionAttribute.startPos;
            textRange.length = compositionAttribute.endPos - compositionAttribute.startPos;
            pTextLayout->SetDrawingEffect(pTextColorBrush, textRange);
          }

          // background color
          CComPtr<ID2D1SolidColorBrush> pBackgroundColorBrush;
          if (compositionAttribute.displayAttribute.crBk.type != TF_CT_NONE) {
            COLORREF colorref;
            if (compositionAttribute.displayAttribute.crText.type == TF_CT_SYSCOLOR) {
              colorref = GetSysColor(compositionAttribute.displayAttribute.crBk.nIndex);
            } else {
              colorref = compositionAttribute.displayAttribute.crBk.cr;
            }
            CHK_FATAL_HRESULT(pRenderTarget->CreateSolidColorBrush(
                graphics::color::ColorUtil::ColorrefToColorF(colorref), &pBackgroundColorBrush));
          }

          // stroke style
          ID2D1StrokeStyle *strokeStyle = nullptr;
          switch (compositionAttribute.displayAttribute.lsStyle) {
          case TF_LS_NONE:
            strokeStyle = nullptr;
            break;
          case TF_LS_SOLID:
            strokeStyle = m_compositionSolidUnderlineStrokeStyle;
            break;
          case TF_LS_DOT:
            strokeStyle = m_compositionDotUnderlineStrokeStyle;
            break;
          case TF_LS_DASH:
            strokeStyle = m_compositionDashUnderlineStrokeStyle;
            break;
          case TF_LS_SQUIGGLE:
            strokeStyle = m_compositionSquiggleUnderlineStrokeStyle;
            break;
          default:
            strokeStyle = m_compositionSolidUnderlineStrokeStyle;
          }

          // stroke width
          FLOAT strokeWidth = compositionAttribute.displayAttribute.fBoldLine ? 2.0f : 1.0f;

          if (pBackgroundColorBrush != nullptr) {
            pRenderTarget->FillRectangle(D2D1::RectF(hitTestMetrics[i].left, hitTestMetrics[i].top,
                                                     hitTestMetrics[i].left + hitTestMetrics[i].width,
                                                     hitTestMetrics[i].top + hitTestMetrics[i].height),
                                         pBackgroundColorBrush);
          }
          if (strokeStyle != nullptr) {
            if (pLineColorBrush != nullptr) {
              pRenderTarget->DrawLine(startPoint, endPoint, pLineColorBrush, strokeWidth, strokeStyle);
            } else {
              pRenderTarget->DrawLine(startPoint, endPoint, m_compositionUnderlineBrush, strokeWidth, strokeStyle);
            }
          }
        }
      }
    }
  }

  pRenderTarget->DrawTextLayout(point, pTextLayout, m_foregroundColorBrush, D2D1_DRAW_TEXT_OPTIONS_NONE);
#ifdef DEBUG_UITEXTFIELD
  LOG_LEAVE(SEVERITY_LEVEL_DEBUG);
#endif
  return S_OK;
}

void ui::UITextField::_RenderHighlight(graphics::D3DInteropHelper *pD3DInteropHelper, ID2D1RenderTarget *pRenderTarget,
                                       UIRectangle textRect) {
#ifdef DEBUG_UITEXTFIELD
  LOG_ENTER(SEVERITY_LEVEL_DEBUG);
#endif
  UNREFERENCED_PARAMETER(pD3DInteropHelper);
  LONG acpStart = m_textStoreACP->GetACPStart();
  LONG acpEnd = m_textStoreACP->GetACPEnd();
  if (HasFocus() && acpStart != acpEnd && m_highlightColorBrush != nullptr) {
    FLOAT x = textRect.x;
    FLOAT y = textRect.y;

    UINT32 actualHitTextCount;
    m_textLayout->HitTestTextRange(acpStart, acpEnd - acpStart, x, y, nullptr, 0, &actualHitTextCount);
    if (actualHitTextCount > 0) {
      std::vector<DWRITE_HIT_TEST_METRICS> hitTestMetrics(actualHitTextCount);

      m_textLayout->HitTestTextRange(acpStart, acpEnd - acpStart, x, y, hitTestMetrics.data(), actualHitTextCount,
                                     &actualHitTextCount);

      for (size_t i = 0; i < actualHitTextCount; i++) {
        D2D1_RECT_F highlightRect = D2D1::RectF(hitTestMetrics[i].left, hitTestMetrics[i].top,
                                                hitTestMetrics[i].left + hitTestMetrics[i].width,
                                                hitTestMetrics[i].top + hitTestMetrics[i].height);
        pRenderTarget->FillRectangle(highlightRect, m_highlightColorBrush);
      }
    }
  }
#ifdef DEBUG_UITEXTFIELD
  LOG_LEAVE(SEVERITY_LEVEL_DEBUG);
#endif
}

HRESULT ui::UITextField::Render(graphics::D3DInteropHelper *pD3DInteropHelper, ID2D1RenderTarget *pRenderTarget) {
#ifdef DEBUG_UITEXTFIELD
  LOG_ENTER(SEVERITY_LEVEL_DEBUG);
#endif
  HRESULT hr = S_OK;
  if (m_textChanged) {
    _RecreateTextLayout(pD3DInteropHelper, pRenderTarget);
    UpdateDisplayAreaPosition();
    UpdateCaret();
    m_textChanged = false;
  }

  pRenderTarget->PushAxisAlignedClip(D2D1::RectF(0.0f, 0.0f, GetWidth(), GetHeight()),
                                     D2D1_ANTIALIAS_MODE_PER_PRIMITIVE);

  UISize textSize;
  CHK_WARN_HRESULT(_CalculateTextSize(textSize));
  DWRITE_HIT_TEST_METRICS hitTestMetrics;
  UIPoint caretPos;
  CHK_WARN_HRESULT(_CalculateCaretSizeAndPosition(caretPos, &hitTestMetrics));
  UISize textDisplaySize(GetWidth() - INNER_MARGIN * 2.0f, GetHeight() - INNER_MARGIN * 2.0f);

  UIRectangle textRect(INNER_MARGIN, INNER_MARGIN, GetWidth() - INNER_MARGIN * 2, GetHeight() - INNER_MARGIN * 2);
  _RenderBackground(pD3DInteropHelper, pRenderTarget);
  _RenderHighlight(pD3DInteropHelper, pRenderTarget, textRect);
  CHK_WARN_HRESULT(_RenderText(pD3DInteropHelper, pRenderTarget, textRect));
  _RenderBox(pD3DInteropHelper, pRenderTarget);

#ifdef UI_UITEXTFIELD_SOFTWARE_CARET
  if (HasFocus()) {
    pRenderTarget->DrawLine(D2D1::Point2F(m_caretPosInTextCoordinate.x, m_caretPosInTextCoordinate.y),
                            D2D1::Point2F(m_caretPosInTextCoordinate.x, m_caretPosInTextCoordinate.y + m_caretHeight),
                            m_foregroundColorBrush);
  }
#endif

  pRenderTarget->PopAxisAlignedClip();
#ifdef DEBUG_UITEXTFIELD
  LOG_LEAVE(SEVERITY_LEVEL_DEBUG);
#endif
  return hr;
}

HWND ui::UITextField::GetHWnd() const {
  auto container = GetParentContainer();
  if (!container.expired()) {
    return container.lock()->GetHWnd();
  } else {
    throw std::runtime_error("container expired");
  }
}

void ui::UITextField::AddCompositionRenderInfo(LONG startPos, LONG endPos,
                                               const TF_DISPLAYATTRIBUTE *pDisplayAttribute) {
#ifdef DEBUG_UITEXTFIELD
  LOG_ENTER(SEVERITY_LEVEL_DEBUG);
#endif
  CompositionAttribute compositionAttribute;
  compositionAttribute.startPos = startPos;
  compositionAttribute.endPos = endPos;
  compositionAttribute.displayAttribute = *pDisplayAttribute;
  m_compositionAttributes.push_back(compositionAttribute);
#ifdef DEBUG_UITEXTFIELD
  LOG_LEAVE(SEVERITY_LEVEL_DEBUG);
#endif
}

void ui::UITextField::ClearCompositionRenderInfo() {
  m_compositionAttributes.clear();
}

void ui::UITextField::NotifyTextReplace(LONG oldTotalLength, LONG startPos, LONG replacedTextLength,
                                        LONG textToInsertLength) {
#ifdef DEBUG_UITEXTFIELD
  LOG_ENTER(SEVERITY_LEVEL_DEBUG);
#endif
  UNREFERENCED_PARAMETER(oldTotalLength);
  UNREFERENCED_PARAMETER(startPos);
  UNREFERENCED_PARAMETER(replacedTextLength);
  UNREFERENCED_PARAMETER(textToInsertLength);
  m_textChanged = true;
  Invalidate();
#ifdef DEBUG_UITEXTFIELD
  LOG_LEAVE(SEVERITY_LEVEL_DEBUG);
#endif
}

void ui::UITextField::NotifyTextRemove(LONG oldTotalLength, LONG startPos, LONG removedTextLength) {
#ifdef DEBUG_UITEXTFIELD
  LOG_ENTER(SEVERITY_LEVEL_DEBUG);
#endif
  UNREFERENCED_PARAMETER(oldTotalLength);
  UNREFERENCED_PARAMETER(startPos);
  UNREFERENCED_PARAMETER(removedTextLength);
  m_textChanged = true;
  Invalidate();
#ifdef DEBUG_UITEXTFIELD
  LOG_LEAVE(SEVERITY_LEVEL_DEBUG);
#endif
}

void ui::UITextField::NotifyTextInsert(LONG oldTotalLength, LONG startPos, LONG textToInsertLength) {
#ifdef DEBUG_UITEXTFIELD
  LOG_ENTER(SEVERITY_LEVEL_DEBUG);
#endif
  UNREFERENCED_PARAMETER(oldTotalLength);
  UNREFERENCED_PARAMETER(startPos);
  UNREFERENCED_PARAMETER(textToInsertLength);
  m_textChanged = true;
  Invalidate();
#ifdef DEBUG_UITEXTFIELD
  LOG_LEAVE(SEVERITY_LEVEL_DEBUG);
#endif
}

void ui::UITextField::NotifySelectionHasChanged() {
#ifdef DEBUG_UITEXTFIELD
  LOG_ENTER(SEVERITY_LEVEL_DEBUG);
#endif
  m_textChanged = true;
  Invalidate();
#ifdef DEBUG_UITEXTFIELD
  LOG_LEAVE(SEVERITY_LEVEL_DEBUG);
#endif
}

HRESULT ui::UITextField::GetTextExt(LONG acpStart, LONG acpEnd, RECT *prc, BOOL *pfClipped) {
#ifdef DEBUG_UITEXTFIELD
  LOG_ENTER(SEVERITY_LEVEL_DEBUG) << L"acpStart=" << acpStart << L",acpEnd=" << acpEnd;
#endif
  if (nullptr == prc || nullptr == pfClipped) {
    return E_INVALIDARG;
  }

  *pfClipped = FALSE;
  ZeroMemory(prc, sizeof(RECT));

  UINT32 actualCount;
  if (m_textLayout->HitTestTextRange(acpStart, acpEnd - acpStart + 1, INNER_MARGIN, INNER_MARGIN, nullptr, 0,
                                     &actualCount)
      != E_NOT_SUFFICIENT_BUFFER) {
    return E_INVALIDARG;
  }

  std::vector<DWRITE_HIT_TEST_METRICS> hitTestMetrics(actualCount);
  m_textLayout->HitTestTextRange(acpStart, acpEnd - acpStart + 1, INNER_MARGIN, INNER_MARGIN, hitTestMetrics.data(),
                                 actualCount, &actualCount);

  if (actualCount > 0) {
    UIPoint p1;
    p1.x = hitTestMetrics[0].left;
    p1.y = hitTestMetrics[0].top;
    UIPoint p2;
    p2.x = (hitTestMetrics[0].left + hitTestMetrics[0].width);
    p2.y = (hitTestMetrics[0].top + hitTestMetrics[0].height);

    UIPoint sp1;
    ClientToScreen(p1, sp1);
    UIPoint sp2;
    ClientToScreen(p2, sp2);

    prc->left = static_cast<LONG>(sp1.x);
    prc->top = static_cast<LONG>(sp1.y);
    prc->right = static_cast<LONG>(sp2.x);
    prc->bottom = static_cast<LONG>(sp2.y);
    pfClipped = FALSE;
  }

#ifdef DEBUG_UITEXTFIELD
  LOG_LEAVE(SEVERITY_LEVEL_DEBUG) << L"rect=" << prc->left << L"," << prc->top << L"," << prc->right << L","
                                  << prc->bottom;
#endif
  return S_OK;
}

LRESULT CALLBACK ui::UITextField::WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) {
  UNREFERENCED_PARAMETER(hWnd);
  UNREFERENCED_PARAMETER(message);
  UNREFERENCED_PARAMETER(wParam);
  UNREFERENCED_PARAMETER(lParam);
  return 0;
}

void ui::UITextField::SetText(const std::wstring &text) {
#ifdef DEBUG_UITEXTFIELD
  LOG_ENTER(SEVERITY_LEVEL_DEBUG) << L"text = " << text;
#endif
  if (!m_initialized) {
    m_presetText = text;
  } else {
    m_textStoreACP->SetText(text, true);
    m_textChanged = true;
  }
#ifdef DEBUG_UITEXTFIELD
  LOG_LEAVE(SEVERITY_LEVEL_DEBUG);
#endif
}

HRESULT ui::UITextField::_CalculateCaretSizeAndPosition(OUT UIPoint &caretPosInTextCoordinate,
                                                        OUT DWRITE_HIT_TEST_METRICS *pHitTestMetrics) {
#ifdef DEBUG_UITEXTFIELD
  LOG_ENTER(SEVERITY_LEVEL_DEBUG);
#endif
  TsActiveSelEnd activeSelEnd = m_textStoreACP->GetActiveSelEnd();
  LONG acpStart = m_textStoreACP->GetACPStart();
  LONG acpEnd = m_textStoreACP->GetACPEnd();
  LONG caretPosition;
  if (activeSelEnd == TS_AE_END) {
    caretPosition = acpEnd - 1;
  } else {
    caretPosition = acpStart;
  }
  CHK_FATAL_HRESULT(m_textLayout->HitTestTextPosition(caretPosition, activeSelEnd == TS_AE_END,
                                                      &(caretPosInTextCoordinate.x), &(caretPosInTextCoordinate.y),
                                                      pHitTestMetrics));
#ifdef DEBUG_UITEXTFIELD
  LOG_LEAVE(SEVERITY_LEVEL_DEBUG) << L"caretPosition = " << caretPosition << L", isTrailingHit = " << std::boolalpha
                                  << (activeSelEnd == TS_AE_END) << ", caretPosInModel = " << caretPosInTextCoordinate;
#endif
  return S_OK;
}

HRESULT ui::UITextField::_CalculateTextSize(OUT UISize &textSize) {
#ifdef DEBUG_UITEXTFIELD
  LOG_ENTER(SEVERITY_LEVEL_DEBUG);
#endif

  DWRITE_TEXT_METRICS textMetrics;
  CHK_FATAL_HRESULT(m_textLayout->GetMetrics(&textMetrics));
  textSize.width = textMetrics.widthIncludingTrailingWhitespace;
  textSize.height = textMetrics.height;

#ifdef DEBUG_UITEXTFIELD
  LOG_LEAVE(SEVERITY_LEVEL_DEBUG);
#endif
  return S_OK;
}

// <summary>Update the caret position</summary>
// <param name="updateTextSize">If the text height may be different by a position of ACP,
// this parameter must be set true.</param>
void ui::UITextField::UpdateCaret() {
#ifdef DEBUG_UITEXTFIELD
  LOG_ENTER(SEVERITY_LEVEL_DEBUG) << L"hwnd = " << std::hex << GetHWnd();
#endif
  if (!HasFocus()) {
#ifdef DEBUG_UITEXTFIELD
    LOG_LEAVE(SEVERITY_LEVEL_DEBUG) << L"no focus, skip";
#endif
    return;
  }
  UIPoint caretPosInTextCoordinate;
  DWRITE_HIT_TEST_METRICS hitTestMetrics;
  _CalculateCaretSizeAndPosition(caretPosInTextCoordinate, &hitTestMetrics);
  caretPosInTextCoordinate.x += (INNER_MARGIN - m_textDisplayAreaHPos);
  caretPosInTextCoordinate.y += INNER_MARGIN;

#ifdef UI_UITEXTFIELD_SOFTWARE_CARET
  m_caretPosInTextCoordinate = caretPosInTextCoordinate;
  m_caretHeight = hitTestMetrics.height;
#else
  CHK_WARN_BOOL(::CreateCaret(GetHWnd(), (HBITMAP) nullptr, 0, (int)hitTestMetrics.height));
  UIPoint caretPosInWindow;
  ClientToWindow(caretPosInTextCoordinate, caretPosInWindow);
  CHK_WARN_BOOL(::SetCaretPos(static_cast<int>(caretPosInWindow.x), static_cast<int>(caretPosInWindow.y)));
  CHK_WARN_BOOL(::ShowCaret(GetHWnd()));
#endif

#ifdef DEBUG_UITEXTFIELD
  LOG_LEAVE(SEVERITY_LEVEL_DEBUG) << L"caretPosInWindow = " << caretPosInWindow;
#endif
}

void ui::UITextField::OnChar(HWND hWnd, WPARAM wParam, LPARAM lParam, CHAR_TYPE charType, wchar_t c,
                             ULONGLONG timestampInMilliSeconds, OUT bool *eaten) {
  UNREFERENCED_PARAMETER(timestampInMilliSeconds);
#ifdef DEBUG_UITEXTFIELD
  ATLTRACE2("UITextField::OnChar\n");
  LOG_ENTER(SEVERITY_LEVEL_DEBUG) << L"char = " << c;
#endif
  if (c == L'\n' || c == L'\r') {
    *eaten = false;
    return;
  }

  bool isShiftKeyDown = (GetKeyState(VK_SHIFT) & 0x80) == 0x80;
  m_textStoreACP->OnChar(hWnd, wParam, lParam, charType, c, isShiftKeyDown);
  m_textStoreACP->GetTextStoreACPSink()->OnLayoutChange(TS_LC_CHANGE, EDIT_VIEW_COOKIE);
  m_textChanged = true;
  Invalidate();
  *eaten = true;
#ifdef DEBUG_UITEXTFIELD
  LOG_LEAVE(SEVERITY_LEVEL_DEBUG);
#endif
}

void ui::UITextField::NotifyCompositionRenderInfoChange() {
  Invalidate();
}

HRESULT ui::UITextField::UpdateDisplayAreaPosition() {
#ifdef DEBUG_UITEXTFIELD
  LOG_ENTER(SEVERITY_LEVEL_DEBUG) << L"m_textDisplayAreaHPos(old) = " << m_textDisplayAreaHPos;
#endif
  UIPoint caretPosInTextCoordinate;
  DWRITE_HIT_TEST_METRICS hitTestMetrics;
  HRESULT hr = _CalculateCaretSizeAndPosition(caretPosInTextCoordinate, &hitTestMetrics);
  if (FAILED(hr)) {
    LOG(SEVERITY_LEVEL_ERROR) << L"_CalculateCaretSizeAndPosition failed, hr = " << hr;
    return hr;
  }
#ifdef DEBUG_UITEXTFIELD
  LOG_LEAVE(SEVERITY_LEVEL_DEBUG) << L"caretPosInTextCoordinate = " << caretPosInTextCoordinate;
  LOG_LEAVE(SEVERITY_LEVEL_DEBUG) << L"GetTextDisplayAreaSize() = " << GetTextDisplayAreaSize();
#endif

  if (caretPosInTextCoordinate.x < m_textDisplayAreaHPos) {
    m_textDisplayAreaHPos = caretPosInTextCoordinate.x;
  } else if (caretPosInTextCoordinate.x > (m_textDisplayAreaHPos + GetTextDisplayAreaSize().width)) {
    m_textDisplayAreaHPos = caretPosInTextCoordinate.x - GetTextDisplayAreaSize().width;
  }
#ifdef DEBUG_UITEXTFIELD
  LOG_LEAVE(SEVERITY_LEVEL_DEBUG) << L"m_textDisplayAreaHPos(new) = " << m_textDisplayAreaHPos;
  ;
#endif
  return hr;
}

void ui::UITextField::CalculateSelectionFromDragRect(FLOAT startX, FLOAT startY, FLOAT endX, FLOAT endY,
                                                     OUT bool *inside, OUT LONG *acpStart, OUT LONG *acpEnd,
                                                     OUT TsActiveSelEnd *activeSelEnd) {
#ifdef DEBUG_UITEXTFIELD
  LOG_ENTER(SEVERITY_LEVEL_DEBUG);
#endif
  BOOL isTrailingHit;
  DWRITE_HIT_TEST_METRICS caretMetrics;

  BOOL isStartPosInside;
  m_textLayout->HitTestPoint(startX, startY, &isTrailingHit, &isStartPosInside, &caretMetrics);
  if (!isStartPosInside) {
    UISize textSize;
    HRESULT hr = _CalculateTextSize(textSize);
    if (FAILED(hr)) {
      LOG(SEVERITY_LEVEL_ERROR) << L"_CalculateTextSize failed, hr = " << hr;
      *inside = false;
      return;
    }
    if (textSize.width <= startX && startX <= m_textDisplayAreaHPos + GetTextDisplayAreaSize().width && 0.0f <= startY
        && startY <= GetTextDisplayAreaSize().height) {
      // click the right side of the text
      caretMetrics.textPosition = static_cast<UINT32>(m_textStoreACP->GetTextLength());
    } else {
      *inside = false;
      return;
    }
  }
  *acpStart = caretMetrics.textPosition;

  BOOL isEndPosInside;
  m_textLayout->HitTestPoint(endX, endY, &isTrailingHit, &isEndPosInside, &caretMetrics);
  if (!isEndPosInside) {
    UISize textSize;
    HRESULT hr = _CalculateTextSize(textSize);
    if (FAILED(hr)) {
      LOG(SEVERITY_LEVEL_ERROR) << L"_CalculateTextSize failed, hr = " << hr;
      *inside = false;
      return;
    }
    if (textSize.width <= endX && endX <= m_textDisplayAreaHPos + GetTextDisplayAreaSize().width && 0.0f <= endY
        && endY <= GetTextDisplayAreaSize().height) {
      // click the right side of the text
      caretMetrics.textPosition = static_cast<UINT32>(m_textStoreACP->GetTextLength());
    } else {
      *inside = false;
      return;
    }
  }
  *acpEnd = caretMetrics.textPosition;

  if (*acpStart < *acpEnd) {
    *activeSelEnd = TS_AE_END;
  } else {
    *activeSelEnd = TS_AE_START;
    std::swap(*acpStart, *acpEnd);
  }
  *inside = true;
#ifdef DEBUG_UITEXTFIELD
  LOG_LEAVE(SEVERITY_LEVEL_DEBUG);
#endif
}

void ui::UITextField::OnLeftMouseDown(HWND hWnd, WPARAM wParam, LPARAM lParam, UIPoint clientPoint,
                                      ULONGLONG timestampInMilliSeconds, OUT bool *eaten) {
#ifdef DEBUG_UITEXTFIELD
  LOG_ENTER(SEVERITY_LEVEL_DEBUG);
#endif
  UNREFERENCED_PARAMETER(hWnd);
  UNREFERENCED_PARAMETER(wParam);
  UNREFERENCED_PARAMETER(lParam);
  UNREFERENCED_PARAMETER(timestampInMilliSeconds);
  if (!HasFocus()) {
    RequestFocus();
    if (!GetParentContainer().expired()) {
      GetParentContainer().lock()->NotifyFocusObtained(shared_from_this());
    }
  } else {
    m_textStoreACP->OnMouseEvent(clientPoint.x - INNER_MARGIN, clientPoint.y - INNER_MARGIN,
                                 clientPoint.x - INNER_MARGIN, clientPoint.y - INNER_MARGIN);
  }
  UpdateCaret();
  *eaten = true;
#ifdef DEBUG_UITEXTFIELD
  LOG_LEAVE(SEVERITY_LEVEL_DEBUG);
#endif
}

void ui::UITextField::OnDraggingStart(HWND hWnd, WPARAM wParam, LPARAM lParam, UI2Points clientDragRect, UIDelta delta,
                                      ULONGLONG timestampInMilliSeconds, OUT bool *eaten) {
#ifdef DEBUG_UITEXTFIELD
  LOG_ENTER(SEVERITY_LEVEL_DEBUG);
#endif
  UNREFERENCED_PARAMETER(hWnd);
  UNREFERENCED_PARAMETER(wParam);
  UNREFERENCED_PARAMETER(lParam);
  UNREFERENCED_PARAMETER(delta);
  UNREFERENCED_PARAMETER(timestampInMilliSeconds);

  m_textStoreACP->OnMouseEvent(clientDragRect.start.x - INNER_MARGIN, clientDragRect.start.y - INNER_MARGIN,
                               clientDragRect.end.x - INNER_MARGIN, clientDragRect.end.y - INNER_MARGIN);
  *eaten = true;
#ifdef DEBUG_UITEXTFIELD
  LOG_LEAVE(SEVERITY_LEVEL_DEBUG);
#endif
}

void ui::UITextField::OnDragging(HWND hWnd, WPARAM wParam, LPARAM lParam, UI2Points clientDragRect, UIDelta delta,
                                 ULONGLONG timestampInMilliSeconds, OUT bool *eaten) {
#ifdef DEBUG_UITEXTFIELD
  LOG_ENTER(SEVERITY_LEVEL_DEBUG);
#endif
  UNREFERENCED_PARAMETER(hWnd);
  UNREFERENCED_PARAMETER(wParam);
  UNREFERENCED_PARAMETER(lParam);
  UNREFERENCED_PARAMETER(delta);
  UNREFERENCED_PARAMETER(timestampInMilliSeconds);

  m_textStoreACP->OnMouseEvent(clientDragRect.start.x - INNER_MARGIN, clientDragRect.start.y - INNER_MARGIN,
                               clientDragRect.end.x - INNER_MARGIN, clientDragRect.end.y - INNER_MARGIN);
  *eaten = true;
#ifdef DEBUG_UITEXTFIELD
  LOG_LEAVE(SEVERITY_LEVEL_DEBUG);
#endif
}

void ui::UITextField::OnDraggingEnd(HWND hWnd, WPARAM wParam, LPARAM lParam, UI2Points clientDragRect, UIDelta delta,
                                    ULONGLONG timestampInMilliSeconds, OUT bool *eaten) {
#ifdef DEBUG_UITEXTFIELD
  LOG_ENTER(SEVERITY_LEVEL_DEBUG);
#endif
  UNREFERENCED_PARAMETER(hWnd);
  UNREFERENCED_PARAMETER(wParam);
  UNREFERENCED_PARAMETER(lParam);
  UNREFERENCED_PARAMETER(delta);
  UNREFERENCED_PARAMETER(timestampInMilliSeconds);

  m_textStoreACP->OnMouseEvent(clientDragRect.start.x - INNER_MARGIN, clientDragRect.start.y - INNER_MARGIN,
                               clientDragRect.end.x - INNER_MARGIN, clientDragRect.end.y - INNER_MARGIN);
  *eaten = true;
#ifdef DEBUG_UITEXTFIELD
  LOG_LEAVE(SEVERITY_LEVEL_DEBUG);
#endif
}

void ui::UITextField::_SetFocus() {
#ifdef DEBUG_UITEXTFIELD
  LOG_ENTER(SEVERITY_LEVEL_DEBUG);
#endif
  CHK_WARN_HRESULT(m_tsfManager->GetThreadMgr()->SetFocus(m_pDocMgr));
  UpdateCaret();
#ifdef DEBUG_UITEXTFIELD
  LOG_LEAVE(SEVERITY_LEVEL_DEBUG);
#endif
}

void ui::UITextField::_KillFocus() {
#ifdef DEBUG_UITEXTFIELD
  LOG_ENTER(SEVERITY_LEVEL_DEBUG);
#endif
  m_textStoreACP->TerminateAllCompositions();
  ClearCompositionRenderInfo();
  CHK_WARN_BOOL(::DestroyCaret());
#ifdef DEBUG_UITEXTFIELD
  LOG_LEAVE(SEVERITY_LEVEL_DEBUG);
#endif
}

void ui::UITextField::RequestFocus() {
#ifdef DEBUG_UITEXTFIELD
  LOG_ENTER(SEVERITY_LEVEL_DEBUG);
#endif
  UIBase::RequestFocus();
  if (HasFocus()) {
    _SetFocus();
  }
#ifdef DEBUG_UITEXTFIELD
  LOG_LEAVE(SEVERITY_LEVEL_DEBUG);
#endif
}

void ui::UITextField::LostFocus() {
#ifdef DEBUG_UITEXTFIELD
  LOG_ENTER(SEVERITY_LEVEL_DEBUG);
#endif
  if (HasFocus()) {
    _KillFocus();
  }
  UIBase::LostFocus();
#ifdef DEBUG_UITEXTFIELD
  LOG_LEAVE(SEVERITY_LEVEL_DEBUG);
#endif
}

void ui::UITextField::OnSetFocus(HWND hWnd, WPARAM wParam, LPARAM lParam, ULONGLONG timestampInMilliSeconds,
                                 OUT bool *eaten) {
#ifdef DEBUG_UITEXTFIELD
  LOG_ENTER(SEVERITY_LEVEL_DEBUG);
#endif
  UNREFERENCED_PARAMETER(hWnd);
  UNREFERENCED_PARAMETER(wParam);
  UNREFERENCED_PARAMETER(lParam);
  UNREFERENCED_PARAMETER(timestampInMilliSeconds);
  if (HasFocus()) {
    _SetFocus();
    *eaten = true;
  }
#ifdef DEBUG_UITEXTFIELD
  LOG_LEAVE(SEVERITY_LEVEL_DEBUG);
#endif
}

void ui::UITextField::OnKillFocus(HWND hWnd, WPARAM wParam, LPARAM lParam, ULONGLONG timestampInMilliSeconds,
                                  OUT bool *eaten) {
#ifdef DEBUG_UITEXTFIELD
  LOG_ENTER(SEVERITY_LEVEL_DEBUG);
#endif
  UNREFERENCED_PARAMETER(hWnd);
  UNREFERENCED_PARAMETER(wParam);
  UNREFERENCED_PARAMETER(lParam);
  UNREFERENCED_PARAMETER(timestampInMilliSeconds);
  if (HasFocus()) {
    _KillFocus();
    *eaten = true;
  }
#ifdef DEBUG_UITEXTFIELD
  LOG_LEAVE(SEVERITY_LEVEL_DEBUG);
#endif
}

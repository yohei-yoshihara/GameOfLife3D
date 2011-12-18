#include "StdAfx.h"
#include "Common.h"
#include "graphics/D3DInteropHelper.h"
#include "graphics/brush/LinearGradientBrush.h"
#include "graphics/color/ColorUtil.h"
#include "ui/UIContainer.h"
#include "ui/UIButton.h"

//#define DEBUG_EVENT

using namespace ui;
using namespace graphics;

ui::UIButton::UIButton(void) :
    m_text(),
    m_fontAttribute(),
    m_fgGradientBrush(std::make_shared<graphics::brush::LinearGradientBrush>()),
    m_bgGradientBrush(std::make_shared<graphics::brush::LinearGradientBrush>()),
    m_bgDownGradientBrush(std::make_shared<graphics::brush::LinearGradientBrush>()),
    m_bgMouseOverGradientBrush(std::make_shared<graphics::brush::LinearGradientBrush>()),
    m_textFormat(),
    m_textLayout(),
    m_textMetrics(),
    m_shineColorBrush(),
    m_shadowColorBrush(),
    m_actionListeners(),
    m_status(BUTTON_STATUS_NORMAL)
{
    SetFocusable(true);

    m_fgGradientBrush->AddGradientStop(
        D2D1::GradientStop(0.0, D2D1::ColorF(D2D1::ColorF::White, 1.0f)));
    m_fgGradientBrush->AddGradientStop(
        D2D1::GradientStop(1.0, D2D1::ColorF(D2D1::ColorF::White, 1.0f)));

    m_bgGradientBrush->AddGradientStop(
        D2D1::GradientStop(0.0, graphics::color::ColorUtil::HsvToRgb(0, 0, 0.35f, 1.0f)));
    m_bgGradientBrush->AddGradientStop(
        D2D1::GradientStop(1.0, graphics::color::ColorUtil::HsvToRgb(0, 0, 0.10f, 1.0f)));

    m_bgDownGradientBrush->AddGradientStop(
        D2D1::GradientStop(0.0, graphics::color::ColorUtil::HsvToRgb(0, 0, 0.25f, 1.0f)));
    m_bgDownGradientBrush->AddGradientStop(
        D2D1::GradientStop(1.0, graphics::color::ColorUtil::HsvToRgb(0, 0, 0.00f, 1.0f)));

    m_bgMouseOverGradientBrush->AddGradientStop(
        D2D1::GradientStop(0.0, graphics::color::ColorUtil::HsvToRgb(0, 0, 0.55f, 1.0f)));
    m_bgMouseOverGradientBrush->AddGradientStop(
        D2D1::GradientStop(1.0, graphics::color::ColorUtil::HsvToRgb(0, 0, 0.30f, 1.0f)));

    m_shineColor = graphics::color::ColorUtil::HsvToRgb(0, 0, 0.7f, 1.0f);
    m_shadowColor = D2D1::ColorF(D2D1::ColorF::Black);
}

ui::UIButton::~UIButton(void)
{
}

HRESULT ui::UIButton::Initialize(
    graphics::D3DInteropHelper *pD3DInteropHelper )
{
    UNREFERENCED_PARAMETER(pD3DInteropHelper);
    return S_OK;
}

HRESULT ui::UIButton::CreateDeviceDependentResources(
    graphics::D3DInteropHelper *pD3DInteropHelper,
    ID2D1RenderTarget *pRenderTarget )
{
    CHK_FATAL_HRESULT(m_fgGradientBrush->CreateDeviceDependentResource(pRenderTarget, D2D1::RectF(0.0f, 0.0f, GetWidth(), GetHeight()), 1.0f));
    CHK_FATAL_HRESULT(m_bgGradientBrush->CreateDeviceDependentResource(pRenderTarget, D2D1::RectF(0.0f, 0.0f, GetWidth(), GetHeight()), 1.0f));
    CHK_FATAL_HRESULT(m_bgDownGradientBrush->CreateDeviceDependentResource(pRenderTarget, D2D1::RectF(0.0f, 0.0f, GetWidth(), GetHeight()), 1.0f));
    CHK_FATAL_HRESULT(m_bgMouseOverGradientBrush->CreateDeviceDependentResource(pRenderTarget, D2D1::RectF(0.0f, 0.0f, GetWidth(), GetHeight()), 1.0f));

    m_textFormat = nullptr;
    CHK_FATAL_HRESULT(pD3DInteropHelper->GetDWriteFactory()->CreateTextFormat(
                          m_fontAttribute.fontFamilyName.c_str(),
                          nullptr,
                          m_fontAttribute.GetFontWeight(),
                          m_fontAttribute.GetFontStyle(),
                          m_fontAttribute.GetFontStretch(),
                          m_fontAttribute.fontSize,
                          L"",
                          &m_textFormat));

    m_textLayout = nullptr;
    CHK_FATAL_HRESULT(pD3DInteropHelper->GetDWriteFactory()->CreateTextLayout(
                          m_text.c_str(),
                          static_cast<UINT>(m_text.size()),
                          m_textFormat,
                          GetWidth(),
                          GetHeight(),
                          &m_textLayout
                      ));

    CHK_FATAL_HRESULT(m_textLayout->GetMetrics(&m_textMetrics));

    m_shineColorBrush = nullptr;
    CHK_FATAL_HRESULT(pRenderTarget->CreateSolidColorBrush(m_shineColor, &m_shineColorBrush));

    m_shadowColorBrush = nullptr;
    CHK_FATAL_HRESULT(pRenderTarget->CreateSolidColorBrush(m_shadowColor, &m_shadowColorBrush));

    return S_OK;
}

void ui::UIButton::DiscardDeviceDependentResources()
{
}

void ui::UIButton::ClearResources()
{
    m_textFormat = nullptr;
    m_textLayout = nullptr;
}

void ui::UIButton::SetText( const std::wstring &text )
{
    m_text = text;
    ClearResources();
}

void ui::UIButton::SetForegroundColor(
    const std::vector<D2D1_GRADIENT_STOP> &foregroundColors,
    GRADIENT_DIRECTION fgGradientDirection )
{
    m_fgGradientBrush->SetGradientStops(foregroundColors);
    m_fgGradientBrush->SetGradientDirection(fgGradientDirection);
    ClearResources();
}

void ui::UIButton::SetBackgroundColor(
    const std::vector<D2D1_GRADIENT_STOP> &backgroundColors,
    GRADIENT_DIRECTION bgGradientDirection )
{
    m_bgGradientBrush->SetGradientStops(backgroundColors);
    m_bgGradientBrush->SetGradientDirection(bgGradientDirection);
    ClearResources();
}

void ui::UIButton::SetFontAttribute( const graphics::FontAttribute &fontAttribute )
{
    m_fontAttribute = fontAttribute;
    ClearResources();
}

HRESULT ui::UIButton::Render(
    graphics::D3DInteropHelper *pD3DInteropHelper,
    ID2D1RenderTarget *pRenderTarget )
{
#ifdef DEBUG_EVENT
    LOG_ENTER(SEVERITY_LEVEL_DEBUG) << *this;
#endif
    if (m_textFormat == nullptr || m_textLayout == nullptr) {
        HRESULT hr = CreateDeviceDependentResources(pD3DInteropHelper, pRenderTarget);
        if (FAILED(hr)) {
            LOG(SEVERITY_LEVEL_ERROR) << L"CreateResources failed, hr = " << hr;
            return hr;
        }
    }

    FLOAT textWidth = m_textMetrics.widthIncludingTrailingWhitespace;
    FLOAT textHeight = m_textMetrics.height;

    D2D1_POINT_2F p1 = D2D1::Point2F(0.0f, 0.0f);
    D2D1_POINT_2F p2 = D2D1::Point2F(GetWidth(), 0.0f);
    D2D1_POINT_2F p3 = D2D1::Point2F(0.0f, GetHeight());
    D2D1_POINT_2F p4 = D2D1::Point2F(GetWidth(), GetHeight());

    switch (m_status) {
    case BUTTON_STATUS_DOWN:
        pRenderTarget->FillRectangle(
            D2D1::RectF(0.0f, 0.0f, GetWidth(), GetHeight()),
            m_bgDownGradientBrush->GetBrush());
        pRenderTarget->DrawLine(p2, p4, m_shineColorBrush);
        pRenderTarget->DrawLine(p3, p4, m_shineColorBrush);
        pRenderTarget->DrawLine(p1, p2, m_shadowColorBrush);
        pRenderTarget->DrawLine(p1, p3, m_shadowColorBrush);
        break;
    case BUTTON_STATUS_MOUSEOVER:
        pRenderTarget->FillRectangle(
            D2D1::RectF(0.0f, 0.0f, GetWidth(), GetHeight()),
            m_bgMouseOverGradientBrush->GetBrush());
        pRenderTarget->DrawLine(p2, p4, m_shadowColorBrush);
        pRenderTarget->DrawLine(p3, p4, m_shadowColorBrush);
        pRenderTarget->DrawLine(p1, p2, m_shineColorBrush);
        pRenderTarget->DrawLine(p1, p3, m_shineColorBrush);
        break;
    case BUTTON_STATUS_NORMAL:
    default:
        pRenderTarget->FillRectangle(
            D2D1::RectF(0.0f, 0.0f, GetWidth(), GetHeight()),
            m_bgGradientBrush->GetBrush());
        pRenderTarget->DrawLine(p2, p4, m_shadowColorBrush);
        pRenderTarget->DrawLine(p3, p4, m_shadowColorBrush);
        pRenderTarget->DrawLine(p1, p2, m_shineColorBrush);
        pRenderTarget->DrawLine(p1, p3, m_shineColorBrush);
        break;
    }

    pRenderTarget->DrawText(
        m_text.c_str(),
        static_cast<UINT>(m_text.size()),
        m_textFormat,
        D2D1::RectF(
            (GetWidth() - textWidth) / 2.0f,
            (GetHeight() - textHeight) / 2.0f,
            GetWidth(),
            GetHeight()),
        m_fgGradientBrush->GetBrush());

#ifdef DEBUG_EVENT
    LOG_LEAVE(SEVERITY_LEVEL_DEBUG);
#endif
    return S_OK;
}

void ui::UIButton::OnLeftMouseDown(
    HWND hWnd,
    WPARAM wParam,
    LPARAM lParam,
    UIPoint clientPoint,
    ULONGLONG timestampInMilliSeconds,
    OUT bool* eaten )
{
    UNREFERENCED_PARAMETER(hWnd);
    UNREFERENCED_PARAMETER(wParam);
    UNREFERENCED_PARAMETER(lParam);
    UNREFERENCED_PARAMETER(timestampInMilliSeconds);
#ifdef DEBUG_EVENT
    LOG_ENTER(SEVERITY_LEVEL_DEBUG) << L"text = " << m_text << L", clientPoint = " << clientPoint;
#endif
    if (0.0f <= clientPoint.x && clientPoint.x <= GetWidth() &&
            0.0f <= clientPoint.y && clientPoint.y <= GetHeight()) {
        m_status = BUTTON_STATUS_DOWN;
        *eaten = true;
    }
#ifdef DEBUG_EVENT
    LOG_LEAVE(SEVERITY_LEVEL_DEBUG);
#endif
}

void ui::UIButton::OnLeftMouseUp(
    HWND hWnd,
    WPARAM wParam,
    LPARAM lParam,
    UIPoint clientPoint,
    ULONGLONG timestampInMilliSeconds,
    OUT bool* eaten )
{
    UNREFERENCED_PARAMETER(hWnd);
    UNREFERENCED_PARAMETER(wParam);
    UNREFERENCED_PARAMETER(lParam);
    UNREFERENCED_PARAMETER(timestampInMilliSeconds);
#ifdef DEBUG_EVENT
    LOG_ENTER(SEVERITY_LEVEL_DEBUG) << L"text = " << m_text << L", clientPoint = " << clientPoint;
#endif
    if (0.0f <= clientPoint.x && clientPoint.x <= GetWidth() &&
            0.0f <= clientPoint.y && clientPoint.y <= GetHeight()) {
        if (!HasFocus()) {
            RequestFocus();
            if (!GetParentContainer().expired()) {
                GetParentContainer().lock()->NotifyFocusObtained(shared_from_this());
            }
        }
        m_actionListeners.FireEvent(ui::event::UIActionEvent(shared_from_this(), m_text));
    }
    m_status = BUTTON_STATUS_NORMAL;
    *eaten = true;
#ifdef DEBUG_EVENT
    LOG_LEAVE(SEVERITY_LEVEL_DEBUG);
#endif
}

void ui::UIButton::OnMouseOver(
    HWND hWnd,
    WPARAM wParam,
    LPARAM lParam,
    UIPoint clientPoint,
    UIDelta delta,
    ULONGLONG timestampInMilliSeconds,
    OUT bool* eaten )
{
    UNREFERENCED_PARAMETER(hWnd);
    UNREFERENCED_PARAMETER(wParam);
    UNREFERENCED_PARAMETER(lParam);
    UNREFERENCED_PARAMETER(delta);
    UNREFERENCED_PARAMETER(timestampInMilliSeconds);
#ifdef DEBUG_EVENT
    LOG_ENTER(SEVERITY_LEVEL_DEBUG)
            << L"text = " << m_text
            << L", clientPoint = " << clientPoint
            << L", delta = " << delta;
#endif
    if (0.0f <= clientPoint.x && clientPoint.x <= GetWidth() &&
            0.0f <= clientPoint.y && clientPoint.y <= GetHeight()) {
        m_status = BUTTON_STATUS_MOUSEOVER;
        *eaten = true;
#ifdef DEBUG_EVENT
        LOG(SEVERITY_LEVEL_DEBUG) << L"status -> MOUSEOVER";
#endif
    }
#ifdef DEBUG_EVENT
    LOG_LEAVE(SEVERITY_LEVEL_DEBUG);
#endif
}

void ui::UIButton::OnMouseOut(
    HWND hWnd,
    WPARAM wParam,
    LPARAM lParam,
    UIPoint clientPoint,
    UIDelta delta,
    ULONGLONG timestampInMilliSeconds,
    OUT bool* eaten )
{
    UNREFERENCED_PARAMETER(hWnd);
    UNREFERENCED_PARAMETER(wParam);
    UNREFERENCED_PARAMETER(lParam);
    UNREFERENCED_PARAMETER(clientPoint);
    UNREFERENCED_PARAMETER(delta);
    UNREFERENCED_PARAMETER(timestampInMilliSeconds);
#ifdef DEBUG_EVENT
    LOG_ENTER(SEVERITY_LEVEL_DEBUG)
            << L"text = " << m_text
            << L", clientPoint = " << clientPoint
            << L", delta = " << delta;
#endif
    m_status = BUTTON_STATUS_NORMAL;
    *eaten = true;
#ifdef DEBUG_EVENT
    LOG_LEAVE(SEVERITY_LEVEL_DEBUG);
#endif
}

std::wostream& ui::UIButton::Dump( std::wostream& os ) const
{
    UIBase::Dump(os);
    os << L",text=" << m_text
       << L",status=";
    switch (m_status) {
    case UIButton::BUTTON_STATUS_NORMAL:
        os << L"NORMAL";
        break;
    case UIButton::BUTTON_STATUS_DOWN:
        os << L"DOWN";
        break;
    case UIButton::BUTTON_STATUS_MOUSEOVER:
        os << L"MOUSEOVER";
        break;
    }
    return os;
}

std::wostream& ui::operator<<( std::wostream& os, ui::UIButton const& button )
{
    os << L"UIButton[";
    button.Dump(os);
    os << L"]";
    return os;
}

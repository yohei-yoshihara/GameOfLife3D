#include "StdAfx.h"
#include "Common.h"
#include "graphics/DirectWriteUtil.h"

// #define DEBUG_DIRECTWRITEUTIL_

std::wostream& operator<< (std::wostream& os, DWRITE_TEXT_METRICS const& metrics)
{
    os << L"DWRITE_TEXT_METRICS[left=" << metrics.left
       << L",top=" << metrics.top
       << L",width=" << metrics.width
       << L",widthIncludingTrailingWhitespace=" << metrics.widthIncludingTrailingWhitespace
       << L",height=" << metrics.height
       << L",layoutWidth=" << metrics.layoutWidth
       << L",layoutHeight=" << metrics.layoutHeight
       << L",maxBidiReorderingDepth=" << metrics.maxBidiReorderingDepth
       << L"]";
    return os;
}

std::wostream& operator<< (std::wostream& os, DWRITE_LINE_METRICS const& metrics)
{
    os << L"DWRITE_LINE_METRICS[length=" << metrics.length
       << L",trailingWhitespaceLength=" << metrics.trailingWhitespaceLength
       << L",newlineLength=" << metrics.newlineLength
       << L",height=" << metrics.height
       << L",baseline=" << metrics.baseline
       << L",isTrimmed=" << metrics.isTrimmed
       << L"]";
    return os;
}

std::wostream& operator<< (std::wostream& os, DWRITE_OVERHANG_METRICS const& metrics)
{
    os << L"DWRITE_OVERHANG_METRICS[left=" << metrics.left
       << L",top=" << metrics.top
       << L",right=" << metrics.right
       << L",bottom=" << metrics.bottom
       << L"]";
    return os;
}

std::wostream& operator<< (std::wostream& os, DWRITE_CLUSTER_METRICS const& metrics)
{
    os << L"DWRITE_CLUSTER_METRICS[width=" << metrics.width
       << L",length=" << metrics.length
       << L",canWrapLineAfter=" << metrics.canWrapLineAfter
       << L",isWhitespace=" << metrics.isWhitespace
       << L",isNewline=" << metrics.isNewline
       << L",isSoftHyphen=" << metrics.isSoftHyphen
       << L",isRightToLeft" << metrics.isRightToLeft
       << L",padding=" << metrics.padding
       << L"]";
    return os;
}

graphics::DirectWriteUtil::DirectWriteUtil(void)
{
}

graphics::DirectWriteUtil::~DirectWriteUtil(void)
{
}

void graphics::DirectWriteUtil::GetLineMetrics(
    IN IDWriteTextLayout *pTextLayout,
    OUT std::vector<DWRITE_LINE_METRICS>& lineMetrics )
{
#ifdef DEBUG_DIRECTWRITEUTIL_
    LOG_ENTER(SEVERITY_LEVEL_DEBUG);
#endif

    DWRITE_TEXT_METRICS textMetrics;
    CHK_FATAL_HRESULT(pTextLayout->GetMetrics(&textMetrics));
    lineMetrics.resize(textMetrics.lineCount);
    CHK_FATAL_HRESULT(pTextLayout->GetLineMetrics(lineMetrics.data(), textMetrics.lineCount, &textMetrics.lineCount));

#ifdef DEBUG_DIRECTWRITEUTIL_
    LOG_LEAVE(SEVERITY_LEVEL_DEBUG);
#endif
}

void graphics::DirectWriteUtil::GetClusterMetrics(
    IN IDWriteTextLayout *pTextLayout,
    OUT std::vector<DWRITE_CLUSTER_METRICS>& clusterMetrics )
{
#ifdef DEBUG_DIRECTWRITEUTIL_
    LOG_ENTER(SEVERITY_LEVEL_DEBUG);
#endif

    UINT32 actualClusterCount = 0;
    HRESULT hr = pTextLayout->GetClusterMetrics(nullptr, 0, &actualClusterCount);
    if (hr != E_NOT_SUFFICIENT_BUFFER) {
        CHK_FATAL_HRESULT(hr);
    }
    clusterMetrics.resize(actualClusterCount);
    CHK_FATAL_HRESULT(pTextLayout->GetClusterMetrics(clusterMetrics.data(), actualClusterCount, &actualClusterCount));

#ifdef DEBUG_DIRECTWRITEUTIL_
    LOG_LEAVE(SEVERITY_LEVEL_DEBUG);
#endif
}

void graphics::DirectWriteUtil::DumpMetrics(
    IN SEVERITY_LEVEL severityLevel,
    IN const std::wstring &layoutName,
    IN IDWriteTextLayout *pTextLayout )
{
#ifdef DEBUG_DIRECTWRITEUTIL_
    LOG_ENTER(SEVERITY_LEVEL_DEBUG);
#endif

    LOG(severityLevel) << layoutName << L"->GetMaxWidth() = " << pTextLayout->GetMaxWidth();
    LOG(severityLevel) << layoutName << L"->GetMaxHeight() = " << pTextLayout->GetMaxHeight();
    FLOAT minWidth = 0.0f;
    CHK_FATAL_HRESULT(pTextLayout->DetermineMinWidth(&minWidth));
    LOG(severityLevel) << layoutName << L"->DetermineMinWidth() = " << minWidth;

    DWRITE_TEXT_METRICS textMetrics;
    CHK_FATAL_HRESULT(pTextLayout->GetMetrics(&textMetrics));
    LOG(severityLevel) << layoutName << L"->GetMetrics() = " << textMetrics;

    DWRITE_OVERHANG_METRICS overhangMetrics;
    CHK_FATAL_HRESULT(pTextLayout->GetOverhangMetrics(&overhangMetrics));
    LOG(severityLevel) << layoutName << L"->GetOverhangMetrics() = " << overhangMetrics;

    std::vector<DWRITE_LINE_METRICS> lineMetrics;
    graphics::DirectWriteUtil::GetLineMetrics(pTextLayout, lineMetrics);
    for (size_t i = 0; i < lineMetrics.size(); ++i) {
        LOG(severityLevel) << L"line[" << i << L"] " << lineMetrics.at(i);
    }

    std::vector<DWRITE_CLUSTER_METRICS> clusterMetrics;
    graphics::DirectWriteUtil::GetClusterMetrics(pTextLayout, clusterMetrics);
    for (size_t i = 0; i < clusterMetrics.size(); ++i) {
        LOG(severityLevel) << L"cluster[" << i << L"] " << clusterMetrics.at(i);
    }

#ifdef DEBUG_DIRECTWRITEUTIL_
    LOG_LEAVE(SEVERITY_LEVEL_DEBUG);
#endif
}

HRESULT graphics::DirectWriteUtil::CreateTextLayoutWithTrimming(
    IN IDWriteFactory *factory,
    IN IDWriteTextFormat *textFormat,
    IN const std::wstring &text,
    IN FLOAT width,
    IN size_t numberOfLines,
    OUT IDWriteTextLayout **textLayout )
{
#ifdef DEBUG_DIRECTWRITEUTIL_
    LOG_ENTER(SEVERITY_LEVEL_DEBUG) << L"text=" << text
                                    << L",width=" << width
                                    << L",numberOfLines=" << numberOfLines;
#endif
    assert(width > 0.0f);

    HRESULT hr = S_OK;
    CHK_ERROR_HRESULT(hr, factory->CreateTextLayout(
                          text.c_str(),
                          static_cast<UINT32>(text.length()),
                          textFormat,
                          width,
                          std::numeric_limits<FLOAT>::max(),
                          textLayout));
    std::vector<DWRITE_LINE_METRICS> lineMetrics;
    GetLineMetrics(*textLayout, lineMetrics);
    if (SUCCEEDED(hr) && lineMetrics.size() > numberOfLines) {
        size_t n = 0;
        GetLineMetrics(*textLayout, lineMetrics);
        do {
            size_t len = 0;
            for (size_t i = 0; i < std::min(lineMetrics.size(), numberOfLines); ++i) {
                len += lineMetrics[i].length;
            }
            std::wstring _text = text.substr(0, len /* - 3 */ - n);
            _text.append(L"...");

            SafeRelease(textLayout);
            CHK_ERROR_HRESULT(hr, factory->CreateTextLayout(
                                  _text.c_str(),
                                  static_cast<UINT32>(_text.length()),
                                  textFormat,
                                  width,
                                  std::numeric_limits<FLOAT>::max(),
                                  textLayout));
            GetLineMetrics(*textLayout, lineMetrics);
            ++n;
        } while (lineMetrics.size() > numberOfLines);
    }
#ifdef DEBUG_DIRECTWRITEUTIL_
    LOG_LEAVE(SEVERITY_LEVEL_DEBUG);
#endif
    return hr;
}

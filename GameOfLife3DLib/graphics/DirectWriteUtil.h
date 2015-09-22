#ifndef GRAPHICS_DIRECTWRITEUTIL_H
#define GRAPHICS_DIRECTWRITEUTIL_H

#include "graphics/GraphicsCommon.h"

std::wostream &operator<<(std::wostream &os, DWRITE_TEXT_METRICS const &metrics);
std::wostream &operator<<(std::wostream &os, DWRITE_LINE_METRICS const &metrics);
std::wostream &operator<<(std::wostream &os, DWRITE_OVERHANG_METRICS const &metrics);
std::wostream &operator<<(std::wostream &os, DWRITE_CLUSTER_METRICS const &metrics);

namespace graphics {
  class DirectWriteUtil {
  private:
    DirectWriteUtil(void);
    ~DirectWriteUtil(void);

  public:
    static void GetLineMetrics(IN IDWriteTextLayout *pTextLayout, OUT std::vector<DWRITE_LINE_METRICS> &lineMetrics);
    static void GetClusterMetrics(IN IDWriteTextLayout *pTextLayout,
                                  OUT std::vector<DWRITE_CLUSTER_METRICS> &clusterMetrics);
    static void DumpMetrics(IN SEVERITY_LEVEL severityLevel, IN const std::wstring &layoutName,
                            IN IDWriteTextLayout *pTextLayout);
    static HRESULT CreateTextLayoutWithTrimming(IN IDWriteFactory *factory, IN IDWriteTextFormat *textFormat,
                                                IN const std::wstring &text, IN FLOAT width, IN size_t numberOfLines,
                                                OUT IDWriteTextLayout **textLayout);
  };
}

#endif // GRAPHICS_DIRECTWRITEUTIL_H
#ifndef GRAPHICS_FIGURE_FILTERRESOURCE_H_
#define GRAPHICS_FIGURE_FILTERRESOURCE_H_

#include "graphics/GraphicsCommon.h"
#include "graphics/figure/IFigure.h"

namespace graphics {
  namespace figure {
    class FilterResource {
    private:
      // フィルター名
      std::wstring m_filterName;
      // フィルターの表示順序
      FILTER_DISPLAY_ORDER m_filterDisplayOrder;
      // フィルターの色
      graphics::color::ColorValue m_colorValue;
      // フィルターを適用したビットマップ
      CComPtr<ID2D1Bitmap> m_bitmap;
      // フィルターの領域
      D2D1_RECT_F m_bounds;
      // フィルタ－のブラシ
      std::shared_ptr<graphics::brush::SolidColorBrush> m_brush;

    public:
      FilterResource()
          : m_filterName()
          , m_filterDisplayOrder(FILTER_DISPLAY_ORDER_BEFORE_FIGURE)
          , m_colorValue()
          , m_bitmap(nullptr)
          , m_bounds()
          , m_brush() {
      }
      ~FilterResource() {
      }
      void SetFilterName(const std::wstring &filterName) {
        m_filterName = filterName;
      }
      const std::wstring &GetFilterName() const {
        return m_filterName;
      }
      void SetFilterDisplayOrder(FILTER_DISPLAY_ORDER filterDisplayOrder) {
        m_filterDisplayOrder = filterDisplayOrder;
      }
      FILTER_DISPLAY_ORDER GetFilterDisplayOrder() const {
        return m_filterDisplayOrder;
      }
      void SetColor(const graphics::color::ColorValue &colorValue) {
        m_colorValue = colorValue;
      }
      const graphics::color::ColorValue &GetColor() const {
        return m_colorValue;
      }
    };
  }
}

#endif
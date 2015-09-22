#ifndef GRAPHICS_SOLIDCOLORBRUSH_H_
#define GRAPHICS_SOLIDCOLORBRUSH_H_

#include "graphics/brush/IBrush.h"
#include "graphics/color/ColorValue.h"

namespace graphics {
  namespace brush {
    class SolidColorBrush : public graphics::brush::IBrush {
    private:
      std::wstring m_label;
      D2D1_COLOR_F m_color;
      CComPtr<ID2D1SolidColorBrush> m_brush;
      bool m_deviceDependentResourceCreated;

    public:
      SolidColorBrush(void);
      SolidColorBrush(D2D1_COLOR_F color);
      SolidColorBrush(const graphics::color::ColorValue &color);
      virtual ~SolidColorBrush(void);
      void SetLabel(const std::wstring &label) {
        m_label = label;
      }
      virtual const std::wstring &GetLabel() const {
        return m_label;
      }
      void SetColor(D2D1_COLOR_F color) {
        m_color = color;
      }
      void SetColor(const graphics::color::ColorValue &color) {
        m_color = color.ToD2D1_COLOR_F();
      }
      D2D1_COLOR_F GetColor() const {
        return m_color;
      }
      virtual HRESULT CreateDeviceDependentResource(ID2D1RenderTarget *pRenderTarget, const D2D1_RECT_F &rect,
                                                    FLOAT opacity);
      virtual ID2D1Brush *GetBrush(const D2D1_RECT_F &rect, FLOAT opacity);
      virtual ID2D1Brush *GetBrush(FLOAT opacity);
      virtual ID2D1Brush *GetBrush();
      virtual void DiscardDeviceDependentResource();
      virtual bool IsDeviceDependentResourceCreated() const {
        return m_deviceDependentResourceCreated;
      }
      virtual void Clear();
    };
  }
}
#endif // GRAPHICS_SOLIDCOLORBRUSH_H_
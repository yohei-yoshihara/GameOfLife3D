#ifndef GRAPHICS_BRUSH_RADIALCOLORBRUSH_H_
#define GRAPHICS_BRUSH_RADIALCOLORBRUSH_H_

#include "graphics/GraphicsCommon.h"
#include "graphics/brush/IBrush.h"

namespace graphics {
  namespace brush {
    class RadialGradientBrush : public graphics::brush::IBrush {
    private:
      std::wstring m_label;
      std::vector<D2D1_GRADIENT_STOP> m_gradientStops;
      D2D1_SIZE_F m_gradientOriginOffsetRate;
      CComPtr<ID2D1RadialGradientBrush> m_brush;
      bool m_deviceDependentResourceCreated;

    protected:
      void _CalculateParameters(IN const D2D1_RECT_F &rect, OUT D2D1_POINT_2F &center, OUT D2D1_POINT_2F &offset,
                                OUT FLOAT &radiusX, OUT FLOAT &radiusY);

    public:
      RadialGradientBrush(void);
      virtual ~RadialGradientBrush(void);
      void SetLabel(const std::wstring &label) {
        m_label = label;
      }
      virtual const std::wstring &GetLabel() const {
        return m_label;
      }
      size_t GetNumberOfFradientStops() const {
        return m_gradientStops.size();
      }
      void AddGradientStop(D2D1_GRADIENT_STOP gradientStop) {
        m_gradientStops.push_back(gradientStop);
      }
      void SetGradientStops(const std::vector<D2D1_GRADIENT_STOP> &gradientStops) {
        m_gradientStops = gradientStops;
      }
      void SetGradientStops(const graphics::color::GradientStopCollection &collection);
      void SetGradientOriginOffsetRate(FLOAT horizontal, FLOAT vertical) {
        m_gradientOriginOffsetRate = D2D1::SizeF(horizontal, vertical);
      }
      void SetGradientOriginOffsetRate(D2D1_SIZE_F offset) {
        m_gradientOriginOffsetRate = offset;
      }
      D2D1_SIZE_F GetGradientOriginOffsetRate() const {
        return m_gradientOriginOffsetRate;
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

#endif // GRAPHICS_BRUSH_RADIALCOLORBRUSH_H_

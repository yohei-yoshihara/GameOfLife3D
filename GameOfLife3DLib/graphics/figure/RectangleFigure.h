#ifndef GRAPHICS_RECTANGLEFIGURE_H_
#define GRAPHICS_RECTANGLEFIGURE_H_

#include "graphics/GraphicsCommon.h"
#include "graphics/color/IColor.h"
#include "graphics/color/ColorUtil.h"
#include "graphics/color/ColorValue.h"
#include "graphics/color/ColorPatternUtil.h"
#include "graphics/figure/IFigure.h"
#include "graphics/decorator/IDecorator.h"
#include "graphics/filter/IGeometryFilter.h"
#include "graphics/figure/GeometryFilterWorkSet.h"

namespace graphics {
  namespace figure {
    // 四角形（角丸四角形も含む）
    class RectangleFigure : public graphics::figure::IFigure,
                            public std::enable_shared_from_this<graphics::figure::RectangleFigure> {
    private:
      // プロパティが変更された場合true
      bool m_propertyChanged;
      // x座標
      FLOAT m_x;
      // y座標
      FLOAT m_y;
      // 幅
      FLOAT m_width;
      // 高さ
      FLOAT m_height;
      // 角丸の半径
      FLOAT m_radius;
      // 図形の色
      graphics::color::ColorValue m_color;
      // 図形の形状（基本となる形状）
      CComPtr<ID2D1RoundedRectangleGeometry> m_pGeometry;
      // 影効果がある場合true
      bool m_shadow;
      // グロー効果がある場合true
      bool m_glow;
      // カラーパターンを実現するために用いる図形の形状
      std::vector<CComPtr<ID2D1RoundedRectangleGeometry>> m_frames;
      // カラーパターンを実現するために用いる複数のブラシ
      std::unique_ptr<graphics::brush::BrushSet> m_brushes;
      // 影効果の色
      std::shared_ptr<graphics::color::IColor> m_shadowColor;
      // グロー効果の色
      std::shared_ptr<graphics::color::IColor> m_glowColor;
      // デコレータ
      std::vector<std::shared_ptr<graphics::decorator::IDecorator>> m_decorators;
      // ジオメトリフィルタ
      std::vector<graphics::figure::GeometryFilterWorkSet> m_geometryFilter;

    public:
      // コンストラクタ
      RectangleFigure(void);
      // デストラクタ
      virtual ~RectangleFigure(void);

      // インターフェイス graphics::figure::IFigure (開始)
      virtual void SetX(FLOAT x) {
        m_x = x;
        m_propertyChanged = true;
      }
      virtual FLOAT GetX() const {
        return m_x;
      }
      virtual void SetY(FLOAT y) {
        m_y = y;
        m_propertyChanged = true;
      }
      virtual FLOAT GetY() const {
        return m_y;
      }
      virtual void SetWidth(FLOAT width) {
        m_width = width;
        m_propertyChanged = true;
      }
      virtual FLOAT GetWidth() const {
        return m_width;
      }
      virtual void SetHeight(FLOAT height) {
        m_height = height;
        m_propertyChanged = true;
      }
      virtual FLOAT GetHeight() const {
        return m_height;
      }
      virtual void SetColor(const graphics::color::ColorValue &colorValue,
                            graphics::color::COLOR_PATTERN colorPattern = graphics::color::COLOR_PATTERN_FLAT);
      virtual HRESULT Initialize(graphics::D3DInteropHelper *pD3DInteropHelper);
      virtual HRESULT CreateDeviceDependentResources(graphics::D3DInteropHelper *pD3DInteropHelper,
                                                     ID2D1RenderTarget *pRenderTarget);
      virtual HRESULT Render(graphics::D3DInteropHelper *pD3DInteropHelper, ID2D1RenderTarget *pRenderTarget,
                             FLOAT opacity);
      virtual void DiscardDeviceDependentResources();
      virtual bool HitTest(FLOAT x, FLOAT y) {
        return (m_x <= x && x <= (m_x + m_width)) && (m_y <= y && y <= (m_y + m_height));
      }
      virtual void AddDecorator(const std::shared_ptr<graphics::decorator::IDecorator> &decorator) {
        m_decorators.push_back(decorator);
      }
      virtual void ClearDecorators() {
        m_decorators.clear();
      }
      // ジオメトリーフィルターの追加
      virtual void AddGeometryFilter(const std::wstring &workSetName, const std::wstring &filterName,
                                     FILTER_DISPLAY_ORDER filterDispayOrder,
                                     std::shared_ptr<graphics::color::IColor> color) {
        m_geometryFilter.push_back(
            graphics::figure::GeometryFilterWorkSet(workSetName, filterName, filterDispayOrder, color));
      }
      // フィルターが存在するかどうかのテスト
      virtual bool HasGeometryFilter(const std::wstring &workSetName) {
        return std::find_if(
                   m_geometryFilter.begin(), m_geometryFilter.end(),
                   [&](graphics::figure::GeometryFilterWorkSet &e) { return e.GetWorkSetName() == workSetName; })
               != m_geometryFilter.end();
      }
      // ジオメトリーフィルターの削除
      virtual void ClearGeometryFilters() {
        m_geometryFilter.clear();
      }
      // インターフェイスgraphics::figure::IFigure (終了)

      // 角丸半径の設定
      void SetRadius(FLOAT radius) {
        m_radius = radius;
        m_propertyChanged = true;
      }
      // 角丸半径の取得
      FLOAT GetRadius() const {
        return m_radius;
      }

      // *** 影効果 ***
      // 影効果をON/OFFに設定
      void SetShadow(bool shadow) {
        m_shadow = shadow;
        m_propertyChanged = true;
      }
      // 影効果を持つかどうか
      bool HasShadow() const {
        return m_shadow;
      }
      // 影効果の色の設定
      void SetShadowColor(const std::shared_ptr<graphics::color::IColor> &shadowColor) {
        m_shadowColor = shadowColor;
        m_propertyChanged = true;
      }
      // 影効果の色の取得
      std::shared_ptr<graphics::color::IColor> GetShadowColor() const {
        return m_shadowColor;
      }

      // *** グロー効果 ***
      // グロー効果をON/OFFに設定
      void SetGlow(bool glow) {
        m_glow = glow;
        m_propertyChanged = true;
      }
      // グロー効果を持つかどうか
      bool HasGlow() const {
        return m_glow;
      }
      // グロー効果の色の設定
      void SetGlowColor(const std::shared_ptr<graphics::color::IColor> &glowColor) {
        m_glowColor = glowColor;
        m_propertyChanged = true;
      }
      // グロー効果の色の取得
      std::shared_ptr<graphics::color::IColor> GetGlowColor() const {
        return m_glowColor;
      }
    };
  }
}

#endif // GRAPHICS_RECTANGLEFIGURE_H_
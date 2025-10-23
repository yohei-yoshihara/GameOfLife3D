#ifndef GRAPHICS_FIGURE_IFIGURE_H_
#define GRAPHICS_FIGURE_IFIGURE_H_

#include "graphics/GraphicsCommon.h"
#include "graphics/decorator/IDecorator.h"

namespace graphics {
  namespace figure {
    enum FILTER_DISPLAY_ORDER {
      FILTER_DISPLAY_ORDER_BEFORE_FIGURE,
      FILTER_DISPLAY_ORDER_AFTER_FIGURE,
    };

    // 図形
    class IFigure {
    public:
      // X座標の設定
      virtual void SetX(FLOAT x) = 0;
      // X座標の取得
      virtual FLOAT GetX() const = 0;
      // Y座標の設定
      virtual void SetY(FLOAT y) = 0;
      // Y座標の取得
      virtual FLOAT GetY() const = 0;
      // 幅の設定
      virtual void SetWidth(FLOAT width) = 0;
      // 幅の取得
      virtual FLOAT GetWidth() const = 0;
      // 高さの設定
      virtual void SetHeight(FLOAT height) = 0;
      // 高さの取得
      virtual FLOAT GetHeight() const = 0;
      // 図形の色の設定
      //   カラーパターンを設定した場合、そのカラーパターンに従って表現される。具体的にどのように表現されるかは
      //   個々の図形の実装による。
      //   カラーパターンを指定しなかった場合、指定した単色で塗りつぶされる。
      virtual void SetColor(const graphics::color::ColorValue &colorValue,
                            graphics::color::COLOR_PATTERN colorPattern = graphics::color::COLOR_PATTERN_FLAT)
          = 0;
      // 初期化
      virtual HRESULT Initialize(graphics::D3DInteropHelper *pD3DInteropHelper) = 0;
      // デバイス依存リソースの生成
      virtual HRESULT CreateDeviceDependentResources(graphics::D3DInteropHelper *pD3DInteropHelper,
                                                     ID2D1RenderTarget *pRenderTarget)
          = 0;
      // 描画
      virtual HRESULT Render(graphics::D3DInteropHelper *pD3DInteropHelper, ID2D1RenderTarget *pRenderTarget,
                             FLOAT opacity)
          = 0;
      // デバイス依存リソースの破棄
      virtual void DiscardDeviceDependentResources() = 0;
      // ヒットテスト
      virtual bool HitTest(FLOAT x, FLOAT y) = 0;
      // デコレータの追加
      virtual void AddDecorator(const std::shared_ptr<graphics::decorator::IDecorator> &decorator) = 0;
      // デコレータのクリア
      virtual void ClearDecorators() = 0;
      // フィルターの追加
      virtual void AddGeometryFilter(const std::wstring &workSetName, const std::wstring &filterName,
                                     FILTER_DISPLAY_ORDER filterDispayOrder,
                                     std::shared_ptr<graphics::color::IColor> color)
          = 0;
      // フィルターが存在するかどうかのテスト
      virtual bool HasGeometryFilter(const std::wstring &workSetName) = 0;
      // フィルターの削除
      virtual void ClearGeometryFilters() = 0;
    };
  }
}

#endif // GRAPHICS_FIGURE_IFIGURE_H_

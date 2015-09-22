#ifndef UI_UILISTBOX_H_
#define UI_UILISTBOX_H_

#include "graphics/GraphicsCommon.h"
#include "ui/UICommon.h"
#include "ui/UIBase.h"
#include "ui/UIContainer.h"
#include "ui/UIScrollBar.h"

namespace ui {

  class UIListBox : public UIContainer, public std::enable_shared_from_this<UIListBox> {
  public:
    // スクロール方向
    enum SCROLL_DIRECTION { SCROLL_DIRECTION_HORIZONTAL, SCROLL_DIRECTION_VERTICAL };

  private:
    // スクロール方向
    SCROLL_DIRECTION m_scrollDirection;
    // 希望するビットマップサイズ （デフォルトは64x64）
    UISize m_preferredBitmapSize;
    // 希望するテキスト幅 (マイナスの場合、ビットマップサイズより自動計算）
    UISize m_preferredTextAreaSize;
    // ビットマップとテキストの間のマージン
    FLOAT m_marginBetweenBitmapAndText;
    // アイテム間の間隔 （デフォルトは4）
    FLOAT m_marginBetweenItems;
    // アイテムの幅
    FLOAT m_itemWidth;
    // アイテムの高さ
    FLOAT m_itemHeight;
    // スクロールバーのサイドからの間隔　（デフォルトは8）
    FLOAT m_scrollBarMarginFromSide;
    // スクロールバーの太さ（デフォルトは12）
    FLOAT m_scrollBarThick;
    // リストボックスを描画するためのフィギュア
    std::unique_ptr<graphics::figure::RectangleFigure> m_rectFigure;
    // リストボックスを描画するためのカラーセット
    std::shared_ptr<graphics::color::ColorSet> m_colorSet;
    // スクロールバー
    std::shared_ptr<ui::UIScrollBar> m_scrollBar;
    // スクロールバーの位置から計算される表示位置オフセット
    FLOAT m_displayPositionOffset;

  public:
    // コンストラクタ
    UIListBox(SCROLL_DIRECTION scrollDirection = SCROLL_DIRECTION_VERTICAL,
              UISize preferredBitmapSize = UISize(64, 64));
    // デストラクタ
    virtual ~UIListBox(void);
    // コンポーネントの初期化
    virtual HRESULT Initialize(graphics::D3DInteropHelper *pD3DInteropHelper);
    // コンポーネントのレイアウト
    virtual HRESULT LayoutComponents();
    // デバイス依存リソースの生成
    virtual HRESULT CreateDeviceDependentResources(graphics::D3DInteropHelper *pD3DInteropHelper,
                                                   ID2D1RenderTarget *pRenderTarget);
    // 描画
    virtual HRESULT Render(graphics::D3DInteropHelper *pD3DInteropHelper, ID2D1RenderTarget *pRenderTarget);
    // デバイス依存リソースの破棄
    virtual void DiscardDeviceDependentResources();

    // 希望するビットマップサイズの設定
    void SetPreferredBitmapSize(UISize preferredBitmapSize) {
      m_preferredBitmapSize = preferredBitmapSize;
    }
    // 希望するテキスト領域サイズの設定
    void SetPreferredTextAreaSize(UISize preferredTextAreaSize) {
      m_preferredTextAreaSize = preferredTextAreaSize;
    }
    // アイテム間のマージンの設定
    void SetMarginBetweenItems(FLOAT marginBetweenItems) {
      m_marginBetweenItems = marginBetweenItems;
    }
    // スクロール方向の設定
    void SetScrollDirection(SCROLL_DIRECTION scrollDirection) {
      m_scrollDirection = scrollDirection;
    }
    // フォーカス取得の通知
    virtual void NotifyFocusObtained(const std::shared_ptr<UIBase> &uiBase);

    // scroll position changed
    void NotifyScrollPositionChanged(FLOAT position);

    // events
    virtual void OnLeftMouseDown(HWND hWnd, WPARAM wParam, LPARAM lParam, UIPoint clientPoint,
                                 ULONGLONG timestampInMilliSeconds, OUT bool *eaten);
    virtual void OnDraggingStart(HWND hWnd, WPARAM wParam, LPARAM lParam, UI2Points clientDragRect, UIDelta delta,
                                 ULONGLONG timestampInMilliSeconds, OUT bool *eaten);
    virtual void OnDragging(HWND hWnd, WPARAM wParam, LPARAM lParam, UI2Points clientDragRect, UIDelta delta,
                            ULONGLONG timestampInMilliSeconds, OUT bool *eaten);
    virtual void OnDraggingEnd(HWND hWnd, WPARAM wParam, LPARAM lParam, UI2Points clientDragRect, UIDelta delta,
                               ULONGLONG timestampInMilliSeconds, OUT bool *eaten);
    virtual void OnMouseOver(HWND hWnd, WPARAM wParam, LPARAM lParam, UIPoint clientPoint, UIDelta delta,
                             ULONGLONG timestampInMilliSeconds, OUT bool *eaten);
    virtual void OnMouseOut(HWND hWnd, WPARAM wParam, LPARAM lParam, UIPoint clientPoint, UIDelta delta,
                            ULONGLONG timestampInMilliSeconds, OUT bool *eaten);
    virtual void OnRightSingleClick(HWND hWnd, WPARAM wParam, LPARAM lParam, UIPoint clientPoint,
                                    ULONGLONG timestampInMilliSeconds, OUT bool *eaten);
    virtual void OnGestureBegin(HWND hWnd, WPARAM wParam, LPARAM lParam, UIPoint clientPoint, PGESTUREINFO pGestureInfo,
                                ULONGLONG timestampInMilliSeconds, OUT bool *eaten);
    virtual void OnGestureEnd(HWND hWnd, WPARAM wParam, LPARAM lParam, UIPoint clientPoint, PGESTUREINFO pGestureInfo,
                              ULONGLONG timestampInMilliSeconds, OUT bool *eaten);
    virtual void OnPan(HWND hWnd, WPARAM wParam, LPARAM lParam, UIPoint clientPoint, PGESTUREINFO pGestureInfo,
                       UIDelta delta, ULONGLONG timestampInMilliSeconds, OUT bool *eaten);
    virtual void OnPressAndTap(HWND hWnd, WPARAM wParam, LPARAM lParam, UIPoint clientPoint, PGESTUREINFO pGestureInfo,
                               ULONGLONG timestampInMilliSeconds, OUT bool *eaten);
  };
}

#endif // UI_UILISTBOX_H_

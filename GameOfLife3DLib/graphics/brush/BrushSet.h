#ifndef GRAPHICS_BRUSH_BRUSHSET_H_
#define GRAPHICS_BRUSH_BRUSHSET_H_

#include "graphics/brush/IBrush.h"
#include "graphics/color/IColor.h"
#include "graphics/color/ColorSet.h"

namespace graphics
{
namespace brush
{
// ブラシセット - 複数のブラシを一つにまとめたクラス
class BrushSet
{
private:
    // ブラシを格納したベクタ
    std::vector<std::shared_ptr<graphics::brush::IBrush>> m_brushes;
    // ラベルとブラシのマップ
    std::unordered_map<std::wstring, std::shared_ptr<graphics::brush::IBrush>> m_labelToBrashMap;
    // デバイス依存リソースが生成されたかどうか
    bool m_deviceDependentResourceCreated;

public:
    // コンストラクタ
    BrushSet() :
        m_brushes(),
        m_labelToBrashMap(),
        m_deviceDependentResourceCreated(false)
    {}
    // コンストラクタ（指定されたカラーセットからブラシを作成）
    BrushSet(const graphics::color::ColorSet &colorSet) :
        m_brushes(colorSet.GetNumberOfColors()),
        m_labelToBrashMap(),
        m_deviceDependentResourceCreated(false) {
        SetColorSet(colorSet);
    }
    // コンストラクタ（指定されたカラーセットからブラシを作成）
    BrushSet(const std::shared_ptr<graphics::color::ColorSet> &colorSet) :
        m_brushes(colorSet->GetNumberOfColors()),
        m_labelToBrashMap(),
        m_deviceDependentResourceCreated(false) {
        SetColorSet(colorSet);
    }
    // デストラクタ
    ~BrushSet() {}
    // ブラシのデバイス依存リソースの生成
    // 指定された領域に合うようにブラシが生成される。もし領域を変更したい場合は
    // GetBrushの引数に指定する。
    // opacityはブラシの不透明度を指定する。もし不透明度を変更したい場合は
    // GetBrushの引数に指定する。
    HRESULT CreateDeviceDependentResource(
        ID2D1RenderTarget *pRenderTarget,
        const D2D1_RECT_F &rect,
        FLOAT opacity);
    // ブラシの追加
    void AddBrush(const std::shared_ptr<graphics::brush::IBrush> &brush) {
        m_brushes.push_back(brush);
    }
    // ブラシの追加
    void AddBrush(const std::shared_ptr<graphics::brush::IBrush> &brush, const std::wstring &label) {
        m_brushes.push_back(brush);
        m_labelToBrashMap.insert({ label, brush });
    }
    // ブラシのクリア（すべてのブラシが破棄される）
    void Clear() {
        m_brushes.clear();
        m_labelToBrashMap.clear();
    }
    // ブラシ数を返す
    size_t GetNumberOfBrushes() const {
        return m_brushes.size();
    }
    // 指定されたインデックスのブラシを返す
    std::shared_ptr<IBrush> Get(size_t index) const {
        return m_brushes[index];
    }
    // ラベルからブラシを返す
    std::shared_ptr<IBrush> Get(const std::wstring &label) const {
        if (m_labelToBrashMap.count(label) > 0) {
            return m_labelToBrashMap.at(label);
        } else {
            throw std::runtime_error("a brush which has the specified label is not found");
        }
    }
    // 現在保持しているすべてのブラシが破棄され、カラーセットの内容に合わせてブラシが生成される
    void SetColorSet(const graphics::color::ColorSet &colorSet);
    // 現在保持しているすべてのブラシが破棄され、カラーセットの内容に合わせてブラシが生成される
    void SetColorSet(const std::shared_ptr<graphics::color::ColorSet> &colorSet) {
        SetColorSet(*colorSet);
    }
    // すべてのデバイス依存リソースを破棄する
    void DiscardDeviceDependentResource();
    // デバイス依存リソースが生成されているかどうか
    bool IsDeviceDependentResourceCreated() {
        return m_deviceDependentResourceCreated;
    }
};
}
}

#endif // GRAPHICS_BRUSH_BRUSHSET_H_

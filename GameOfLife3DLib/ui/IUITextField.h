#ifndef UI_IUITEXTFIELD_H_
#define UI_IUITEXTFIELD_H_

namespace ui {
  class IUITextField {
  public:
    virtual HWND GetHWnd() const = 0;
    virtual ITfContext *GetContext() const = 0;
    virtual LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) = 0;
    virtual HRESULT GetTextExt(LONG acpStart, LONG acpEnd, RECT *prc, BOOL *pfClipped) = 0;
    virtual void NotifySelectionHasChanged() = 0;
    virtual void NotifyTextReplace(LONG oldTotalLength, LONG startPos, LONG replacedTextLength, LONG textToInsertLength)
        = 0;
    virtual void NotifyTextInsert(LONG oldTotalLength, LONG startPos, LONG textToInsertLength) = 0;
    virtual void NotifyTextRemove(LONG oldTotalLength, LONG startPos, LONG removedTextLength) = 0;
    virtual void AddCompositionRenderInfo(LONG startPos, LONG endPos, const TF_DISPLAYATTRIBUTE *pDisplayAttribute) = 0;
    virtual void NotifyCompositionRenderInfoChange() = 0;
    virtual void ClearCompositionRenderInfo() = 0;
    virtual void CalculateSelectionFromDragRect(FLOAT startX, FLOAT startY, FLOAT endX, FLOAT endY, OUT bool *inside,
                                                OUT LONG *acpStart, OUT LONG *acpEnd, OUT TsActiveSelEnd *activeSelEnd)
        = 0;
  };
}

#endif // UI_IUITEXTFIELD_H_
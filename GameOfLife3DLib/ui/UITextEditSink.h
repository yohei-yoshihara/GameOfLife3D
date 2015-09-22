#ifndef UI_TEXTEDITSINK_H_
#define UI_TEXTEDITSINK_H_

#include "graphics/GraphicsCommon.h"
#include "ui/UICommon.h"
#include "ui/UITextDisplayAttribute.h"
#include "ui/IUITextField.h"

namespace ui {

  class UITextDisplayAttribute;

  class UITextEditSink : public ITfTextEditSink {
  private:
    std::weak_ptr<IUITextField> m_editControl;
    ULONG m_refCount;
    UITextDisplayAttribute m_displayAttribute;
    DWORD m_editCookie;

  public:
    UITextEditSink();
    virtual ~UITextEditSink(void);
    virtual HRESULT Initialize(std::weak_ptr<IUITextField> pEditControl);
    void SetDisplayAttributeMgr(ITfDisplayAttributeMgr *pDisplayAttributeMgr);
    void SetCategoryMgr(ITfCategoryMgr *pCategoryMgr);

    // IUnknown methods.
    STDMETHOD(QueryInterface)(REFIID, LPVOID *);
    STDMETHOD_(DWORD, AddRef)();
    STDMETHOD_(DWORD, Release)();

    // ITfTextEditSink
    STDMETHODIMP OnEndEdit(ITfContext *pic, TfEditCookie ecReadOnly, ITfEditRecord *pEditRecord);

    HRESULT Advise(ITfContext *pTfContext);
    HRESULT Unadvise(ITfContext *pTfContext);
  };
}

#endif // UI_TEXTEDITSINK_H_
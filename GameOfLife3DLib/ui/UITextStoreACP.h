﻿#ifndef UI_UITEXTSTOREACP_H_
#define UI_UITEXTSTOREACP_H_

#include "ui/UICommon.h"
#include "ui/IUITextField.h"
#include "tsf/MessageQueue.h"

namespace ui {
#define EDIT_VIEW_COOKIE 0
#define NUM_SUPPORTED_ATTRS 2

#define ATTR_INDEX_TEXT_ORIENTATION 0
#define ATTR_INDEX_TEXT_VERTICALWRITING 1

#define ATTR_FLAG_NONE 0
#define ATTR_FLAG_REQUESTED 1
#define ATTR_FLAG_DEFAULT 2

  typedef struct {
    IUnknown *punkID;
    ITextStoreACPSink *pTextStoreACPSink;
    DWORD dwMask;
  } ADVISE_SINK, *PADVISE_SINK;

  typedef struct {
    DWORD dwFlags;
    const TS_ATTRID *attrid;
    VARIANT varValue;
    VARIANT varDefaultValue;
  } ATTRIBUTES, *PATTRIBUTES;

  // <summary>This class implements ITextStoreACP interface to support TSF</summary>
  class UITextStoreACP : public ITextStoreACP {
  private:
    std::weak_ptr<IUITextField> m_editControl;
    ULONG m_refCount;
    ADVISE_SINK m_AdviseSink;
    ITextStoreACPServices *m_pServices;
    BOOL m_fLocked;
    DWORD m_dwLockType;
    BOOL m_fPendingLockUpgrade;
    tsf::MessageQueue m_messageQueue;

    std::wstring m_text;
    LONG m_acpStart;
    LONG m_acpEnd;
    TsActiveSelEnd m_ActiveSelEnd;
    BOOL m_fInterimChar;
    BOOL m_fNotify;
    ATTRIBUTES m_rgAttributes[NUM_SUPPORTED_ATTRS];
    BOOL m_fLayoutChanged;
    DWORD m_dwInternalLockType;

  public:
    UITextStoreACP();
    virtual ~UITextStoreACP(void);
    HRESULT Initialize(std::weak_ptr<IUITextField> pEditControl);
    const std::wstring &GetText() const {
      return m_text;
    }
    void SetText(const std::wstring &text, bool setAcpEnd = false) {
      m_text = text;
      if (setAcpEnd) {
        m_acpStart = m_acpEnd = static_cast<LONG>(m_text.length());
        m_ActiveSelEnd = TS_AE_START;
      }
    }
    size_t GetTextLength() {
      return m_text.length();
    }
    LONG GetACPStart() {
      return m_acpStart;
    }
    LONG GetACPEnd() {
      return m_acpEnd;
    }
    TsActiveSelEnd GetActiveSelEnd() {
      return m_ActiveSelEnd;
    }
    bool IsAdvised() {
      return m_AdviseSink.pTextStoreACPSink != nullptr;
    }
    // IUnknown methods.
    STDMETHOD(QueryInterface)(REFIID, LPVOID *);
    STDMETHOD_(DWORD, AddRef)();
    STDMETHOD_(DWORD, Release)();

    // ITextStoreACP methods.
    STDMETHODIMP AdviseSink(REFIID riid, IUnknown *punk, DWORD dwMask);
    STDMETHODIMP UnadviseSink(IUnknown *punk);
    STDMETHODIMP RequestLock(DWORD dwLockFlags, HRESULT *phrSession);
    STDMETHODIMP GetStatus(TS_STATUS *pdcs);
    STDMETHODIMP QueryInsert(LONG acpTestStart, LONG acpTestEnd, ULONG cch, LONG *pacpResultStart, LONG *pacpResultEnd);
    STDMETHODIMP GetSelection(ULONG ulIndex, ULONG ulCount, TS_SELECTION_ACP *pSelection, ULONG *pcFetched);
    STDMETHODIMP SetSelection(ULONG ulCount, const TS_SELECTION_ACP *pSelection);
    STDMETHODIMP GetText(LONG acpStart, LONG acpEnd, WCHAR *pchPlain, ULONG cchPlainReq, ULONG *pcchPlainOut,
                         TS_RUNINFO *prgRunInfo, ULONG ulRunInfoReq, ULONG *pulRunInfoOut, LONG *pacpNext);
    STDMETHODIMP SetText(DWORD dwFlags, LONG acpStart, LONG acpEnd, const WCHAR *pchText, ULONG cch,
                         TS_TEXTCHANGE *pChange);
    STDMETHODIMP GetFormattedText(LONG acpStart, LONG acpEnd, IDataObject **ppDataObject);
    STDMETHODIMP GetEmbedded(LONG acpPos, REFGUID rguidService, REFIID riid, IUnknown **ppunk);
    STDMETHODIMP QueryInsertEmbedded(const GUID *pguidService, const FORMATETC *pFormatEtc, BOOL *pfInsertable);
    STDMETHODIMP InsertEmbedded(DWORD dwFlags, LONG acpStart, LONG acpEnd, IDataObject *pDataObject,
                                TS_TEXTCHANGE *pChange);
    STDMETHODIMP RequestSupportedAttrs(DWORD dwFlags, ULONG cFilterAttrs, const TS_ATTRID *paFilterAttrs);
    STDMETHODIMP RequestAttrsAtPosition(LONG acpPos, ULONG cFilterAttrs, const TS_ATTRID *paFilterAttrs, DWORD dwFlags);
    STDMETHODIMP RequestAttrsTransitioningAtPosition(LONG acpPos, ULONG cFilterAttrs, const TS_ATTRID *paFilterAttrs,
                                                     DWORD dwFlags);
    STDMETHODIMP FindNextAttrTransition(LONG acpStart, LONG acpHalt, ULONG cFilterAttrs, const TS_ATTRID *paFilterAttrs,
                                        DWORD dwFlags, LONG *pacpNext, BOOL *pfFound, LONG *plFoundOffset);
    STDMETHODIMP RetrieveRequestedAttrs(ULONG ulCount, TS_ATTRVAL *paAttrVals, ULONG *pcFetched);
    STDMETHODIMP GetEndACP(LONG *pacp);
    STDMETHODIMP GetActiveView(TsViewCookie *pvcView);
    STDMETHODIMP GetACPFromPoint(TsViewCookie vcView, const POINT *pt, DWORD dwFlags, LONG *pacp);
    STDMETHODIMP GetTextExt(TsViewCookie vcView, LONG acpStart, LONG acpEnd, RECT *prc, BOOL *pfClipped);
    STDMETHODIMP GetScreenExt(TsViewCookie vcView, RECT *prc);
    STDMETHODIMP GetWnd(TsViewCookie vcView, HWND *phwnd);
    STDMETHODIMP InsertTextAtSelection(DWORD dwFlags, const WCHAR *pchText, ULONG cch, LONG *pacpStart, LONG *pacpEnd,
                                       TS_TEXTCHANGE *pChange);
    STDMETHODIMP InsertEmbeddedAtSelection(DWORD dwFlags, IDataObject *pDataObject, LONG *pacpStart, LONG *pacpEnd,
                                           TS_TEXTCHANGE *pChange);

    // ITfContextOwnerCompositionSink
    STDMETHODIMP OnStartComposition(ITfCompositionView *pComposition, BOOL *pfOk);
    STDMETHODIMP OnUpdateComposition(ITfCompositionView *pComposition, ITfRange *pRangeNew);
    STDMETHODIMP OnEndComposition(ITfCompositionView *pComposition);

    HRESULT _ClearAdviseSink(PADVISE_SINK pAdviseSink);
    BOOL _LockDocument(DWORD dwLockFlags);
    void _UnlockDocument();
    BOOL _IsLocked(DWORD dwLockType);
    void _ClearRequestedAttributes(void);
    BOOL _GetCurrentSelection(void);
    STDMETHODIMP _TestInsert(LONG acpTestStart, LONG acpTestEnd, ULONG cch, LONG *pacpResultStart, LONG *pacpResultEnd);
    HRESULT _GetText(std::wstring &text, LPLONG pcch);

    BOOL InitializeAttributes();
    HRESULT TerminateAllCompositions(void);

    void PushWndMessage(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
    ITextStoreACPSink *GetTextStoreACPSink();
    HRESULT ChangeACPWithoutLock(LONG acpStart, LONG acpEnd, TsActiveSelEnd activeSelEnd);
    HRESULT InsertTextAtACP(std::wstring &text);
    HRESULT Backspace();
    HRESULT Delete();
    HRESULT CursorLeft(bool isShiftKeyDown);
    HRESULT CursorRight(bool isShiftKeyDown);

    HRESULT OnChar(HWND hWnd, WPARAM wParam, LPARAM lParam, CHAR_TYPE charType, wchar_t c, bool isShiftKeyDown);
    HRESULT OnMouseEvent(FLOAT startX, FLOAT startY, FLOAT endX, FLOAT endY);

    void _Internal_InsertText(LONG startPos, std::wstring &s);
    void _Internal_RemoveText(LONG startPos, LONG length);
    void _Internal_ReplaceText(LONG startPos, LONG length, std::wstring &s);
    HRESULT MoveACPToEnd();
  };
}

#endif // UI_UITEXTSTOREACP_H_
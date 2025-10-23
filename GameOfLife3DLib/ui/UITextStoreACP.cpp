﻿#include "StdAfx.h"
#include "ui/UITextStoreACP.h"

ui::UITextStoreACP::UITextStoreACP()
    : m_editControl()
    , m_refCount(0)
    , m_pServices(nullptr)
    , m_fLocked(FALSE)
    , m_fNotify(FALSE)
    , m_acpStart(0)
    , m_acpEnd(0)
    , m_dwInternalLockType(0)
    , m_fInterimChar(FALSE)
    , m_ActiveSelEnd(TS_AE_START) {
  m_AdviseSink.punkID = nullptr;
  m_AdviseSink.dwMask = 0;
  m_AdviseSink.pTextStoreACPSink = nullptr;
  InitializeAttributes();
}

ui::UITextStoreACP::~UITextStoreACP(void) {
}

HRESULT ui::UITextStoreACP::Initialize(std::weak_ptr<IUITextField> editControl) {
  HRESULT hr = S_OK;
  m_editControl = editControl;
  return hr;
}

STDMETHODIMP ui::UITextStoreACP::QueryInterface(REFIID riid, LPVOID *ppReturn) {
  *ppReturn = nullptr;

  // IUnknown
  if (IsEqualIID(riid, IID_IUnknown) || IsEqualIID(riid, IID_ITextStoreACP)) {
    *ppReturn = (ITextStoreACP *)this;
  }
  if (*ppReturn) {
    (*(LPUNKNOWN *)ppReturn)->AddRef();
    return S_OK;
  }
  LOG(SEVERITY_LEVEL_ERROR) << L"E_NOINTERFACE";
  return E_NOINTERFACE;
}

STDMETHODIMP_(DWORD) ui::UITextStoreACP::AddRef() {
  return InterlockedIncrement(&m_refCount);
}

STDMETHODIMP_(DWORD) ui::UITextStoreACP::Release() {
  long val = InterlockedDecrement(&m_refCount);
  if (val == 0) {
    delete this;
  }
  return val;
}

STDMETHODIMP ui::UITextStoreACP::AdviseSink(REFIID riid, IUnknown *pUnknown, DWORD dwMask) {
  HRESULT hr;
  IUnknown *punkID;

  hr = pUnknown->QueryInterface(IID_IUnknown, (LPVOID *)&punkID);
  if (FAILED(hr)) {
    return hr;
  }

  hr = E_INVALIDARG;

  if (punkID == m_AdviseSink.punkID) {
    m_AdviseSink.dwMask = dwMask;

    hr = S_OK;
  } else if (nullptr != m_AdviseSink.punkID) {
    hr = CONNECT_E_ADVISELIMIT;
  } else if (IsEqualIID(riid, IID_ITextStoreACPSink)) {
    m_AdviseSink.dwMask = dwMask;
    m_AdviseSink.punkID = punkID;

    punkID->AddRef();
    pUnknown->QueryInterface(IID_ITextStoreACPSink, (LPVOID *)&m_AdviseSink.pTextStoreACPSink);
    pUnknown->QueryInterface(IID_ITextStoreACPServices, (LPVOID *)&m_pServices);
    hr = S_OK;
  }

  punkID->Release();
  return hr;
}

STDMETHODIMP ui::UITextStoreACP::UnadviseSink(IUnknown *pUnknown) {
  HRESULT hr;
  IUnknown *punkID;

  hr = pUnknown->QueryInterface(IID_IUnknown, (LPVOID *)&punkID);
  if (FAILED(hr)) {
    return hr;
  }

  if (punkID == m_AdviseSink.punkID) {
    _ClearAdviseSink(&m_AdviseSink);

    if (m_pServices) {
      m_pServices->Release();
      m_pServices = nullptr;
    }

    hr = S_OK;
  } else {
    hr = CONNECT_E_NOCONNECTION;
  }

  punkID->Release();
  return hr;
}

STDMETHODIMP ui::UITextStoreACP::RequestLock(DWORD dwLockFlags, HRESULT *phrSession) {
  if (nullptr == m_AdviseSink.pTextStoreACPSink) {
    LOG(SEVERITY_LEVEL_ERROR) << L"E_UNEXPECTED";
    return E_UNEXPECTED;
  }

  if (nullptr == phrSession) {
    LOG(SEVERITY_LEVEL_ERROR) << L"E_INVALIDARG";
    return E_INVALIDARG;
  }

  *phrSession = E_FAIL;
  HRESULT hr = S_OK;

  if (m_fLocked) {
    if (dwLockFlags & TS_LF_SYNC) {
      *phrSession = TS_E_SYNCHRONOUS;
      hr = S_OK;
    } else {
      if (((m_dwLockType & TS_LF_READWRITE) == TS_LF_READ) && ((dwLockFlags & TS_LF_READWRITE) == TS_LF_READWRITE)) {
        m_fPendingLockUpgrade = TRUE;

        *phrSession = TS_S_ASYNC;

        hr = S_OK;
      } else {
        hr = E_FAIL;
      }
    }
  } else {
    _LockDocument(dwLockFlags);
    *phrSession = m_AdviseSink.pTextStoreACPSink->OnLockGranted(dwLockFlags);
    _UnlockDocument();
  }

  auto pEditControl = m_editControl.lock();
  while (!m_messageQueue.IsEmpty()) {
    tsf::Message msg;
    m_messageQueue.Pop(&msg);
    pEditControl->WndProc(msg.hWnd, msg.message, msg.wParam, msg.lParam);
  }
  return hr;
}

STDMETHODIMP ui::UITextStoreACP::GetStatus(TS_STATUS *pdcs) {
  if (nullptr == pdcs) {
    return E_INVALIDARG;
  }
  pdcs->dwDynamicFlags = 0;
  pdcs->dwStaticFlags = 0;
  return S_OK;
}

STDMETHODIMP ui::UITextStoreACP::QueryInsert(LONG acpTestStart, LONG acpTestEnd, ULONG cch, LONG *pacpResultStart,
                                             LONG *pacpResultEnd) {
  return _TestInsert(acpTestStart, acpTestEnd, cch, pacpResultStart, pacpResultEnd);
}

STDMETHODIMP ui::UITextStoreACP::GetSelection(ULONG ulIndex, ULONG ulCount, TS_SELECTION_ACP *pSelection,
                                              ULONG *pcFetched) {
  UNREFERENCED_PARAMETER(ulCount);

  if (nullptr == pSelection) {
    return E_INVALIDARG;
  }

  if (nullptr == pcFetched) {
    return E_INVALIDARG;
  }

  *pcFetched = 0;

  if (!_IsLocked(TS_LF_READ)) {
    return TS_E_NOLOCK;
  }

  if (TF_DEFAULT_SELECTION == ulIndex) {
    ulIndex = 0;
  } else if (ulIndex > 1) {
    return E_INVALIDARG;
  }

  pSelection[0].acpStart = m_acpStart;
  pSelection[0].acpEnd = m_acpEnd;
  pSelection[0].style.fInterimChar = m_fInterimChar;
  if (m_fInterimChar) {
    pSelection[0].style.ase = TS_AE_NONE;
  } else {
    pSelection[0].style.ase = m_ActiveSelEnd;
  }
  *pcFetched = 1;

  return S_OK;
}

STDMETHODIMP ui::UITextStoreACP::SetSelection(ULONG ulCount, const TS_SELECTION_ACP *pSelection) {
  if (nullptr == pSelection) {
    return E_INVALIDARG;
  }

  if (ulCount > 1) {
    return E_INVALIDARG;
  }

  if (!_IsLocked(TS_LF_READWRITE)) {
    return TS_E_NOLOCK;
  }

  m_acpStart = pSelection[0].acpStart;
  m_acpEnd = pSelection[0].acpEnd;
  m_fInterimChar = pSelection[0].style.fInterimChar;
  if (m_fInterimChar) {
    m_ActiveSelEnd = TS_AE_NONE;
  } else {
    m_ActiveSelEnd = pSelection[0].style.ase;
  }

  LONG lStart = m_acpStart;
  LONG lEnd = m_acpEnd;

  if (TS_AE_START == m_ActiveSelEnd) {
    lStart = m_acpEnd;
    lEnd = m_acpStart;
  }

  m_fNotify = TRUE;

  return S_OK;
}

STDMETHODIMP ui::UITextStoreACP::GetText(IN LONG acpStart, IN LONG acpEnd, OUT WCHAR *pchPlain, IN ULONG cchPlainReq,
                                         OUT ULONG *pcchPlainRet, OUT TS_RUNINFO *prgRunInfo, IN ULONG ulRunInfoReq,
                                         OUT ULONG *pulRunInfoOut, OUT LONG *pacpNext) {
  if (!_IsLocked(TS_LF_READ)) {
    return TS_E_NOLOCK;
  }

  BOOL fDoText = cchPlainReq > 0;
  BOOL fDoRunInfo = ulRunInfoReq > 0;
  LONG cchTotal;
  HRESULT hr = E_FAIL;

  if (pcchPlainRet) {
    *pcchPlainRet = 0;
  }

  if (fDoRunInfo) {
    *pulRunInfoOut = 0;
  }

  if (pacpNext) {
    *pacpNext = acpStart;
  }

  std::wstring text;
  _GetText(text, &cchTotal);

  if ((acpStart < 0) || (acpStart > cchTotal)) {
    hr = TS_E_INVALIDPOS;
  } else {
    if (acpStart == cchTotal) {
      hr = S_OK;
    } else {
      ULONG cchReq;

      if (acpEnd >= acpStart) {
        cchReq = acpEnd - acpStart;
      } else {
        cchReq = cchTotal - acpStart;
      }

      if (fDoText) {
        if (cchReq > cchPlainReq) {
          cchReq = cchPlainReq;
        }

        std::wstring substr = text.substr(acpStart, cchReq);

        if (pchPlain && cchPlainReq) {
          CopyMemory(pchPlain, substr.c_str(), cchReq * sizeof(WCHAR));
        }
      }

      if (pcchPlainRet) {
        *pcchPlainRet = cchReq;
      }

      if (fDoRunInfo) {
        *pulRunInfoOut = 1;
        prgRunInfo[0].type = TS_RT_PLAIN;
        prgRunInfo[0].uCount = cchReq;
      }

      if (pacpNext) {
        *pacpNext = acpStart + cchReq;
      }

      hr = S_OK;
    }
  }
  return hr;
}

STDMETHODIMP ui::UITextStoreACP::SetText(DWORD dwFlags, LONG acpStart, LONG acpEnd, const WCHAR *pchText, ULONG cch,
                                         TS_TEXTCHANGE *pChange) {
  UNREFERENCED_PARAMETER(dwFlags);

  HRESULT hr = S_OK;

  TS_SELECTION_ACP tsa;
  tsa.acpStart = acpStart;
  tsa.acpEnd = acpEnd;
  tsa.style.ase = TS_AE_START;
  tsa.style.fInterimChar = FALSE;

  hr = SetSelection(1, &tsa);

  if (SUCCEEDED(hr)) {
    hr = InsertTextAtSelection(TS_IAS_NOQUERY, pchText, cch, nullptr, nullptr, pChange);
  }
  return hr;
}

STDMETHODIMP ui::UITextStoreACP::GetFormattedText(LONG acpStart, LONG acpEnd, IDataObject **ppDataObject) {
  UNREFERENCED_PARAMETER(acpStart);
  UNREFERENCED_PARAMETER(acpEnd);
  UNREFERENCED_PARAMETER(ppDataObject);
  return E_NOTIMPL;
}

STDMETHODIMP ui::UITextStoreACP::GetEmbedded(LONG acpPos, REFGUID rguidService, REFIID riid, IUnknown **ppunk) {
  UNREFERENCED_PARAMETER(acpPos);
  UNREFERENCED_PARAMETER(rguidService);
  UNREFERENCED_PARAMETER(riid);
  UNREFERENCED_PARAMETER(ppunk);
  return E_NOTIMPL;
}

STDMETHODIMP ui::UITextStoreACP::QueryInsertEmbedded(const GUID *pguidService, const FORMATETC *pFormatEtc,
                                                     BOOL *pfInsertable) {
  UNREFERENCED_PARAMETER(pguidService);
  UNREFERENCED_PARAMETER(pFormatEtc);
  UNREFERENCED_PARAMETER(pfInsertable);

  *pfInsertable = FALSE;

  return E_NOTIMPL;
}

STDMETHODIMP ui::UITextStoreACP::InsertEmbedded(DWORD dwFlags, LONG acpStart, LONG acpEnd, IDataObject *pDataObject,
                                                TS_TEXTCHANGE *pChange) {
  UNREFERENCED_PARAMETER(dwFlags);
  UNREFERENCED_PARAMETER(acpStart);
  UNREFERENCED_PARAMETER(acpEnd);
  UNREFERENCED_PARAMETER(pDataObject);
  UNREFERENCED_PARAMETER(pChange);

  return E_NOTIMPL;
}

STDMETHODIMP ui::UITextStoreACP::RequestSupportedAttrs(DWORD dwFlags, ULONG cFilterAttrs,
                                                       const TS_ATTRID *paFilterAttrs) {
  _ClearRequestedAttributes();

  for (int i = 0; i < NUM_SUPPORTED_ATTRS; ++i) {
    for (ULONG x = 0; x < cFilterAttrs; x++) {
      if (IsEqualGUID(*m_rgAttributes[i].attrid, paFilterAttrs[x])) {
        m_rgAttributes[i].dwFlags = ATTR_FLAG_REQUESTED;
        if (dwFlags & TS_ATTR_FIND_WANT_VALUE) {
          m_rgAttributes[i].dwFlags |= ATTR_FLAG_DEFAULT;
        } else {
          VariantCopy(&m_rgAttributes[i].varValue, &m_rgAttributes[i].varDefaultValue);
        }
      }
    }
  }
  return S_OK;
}

STDMETHODIMP ui::UITextStoreACP::RequestAttrsAtPosition(LONG acpPos, ULONG cFilterAttrs, const TS_ATTRID *paFilterAttrs,
                                                        DWORD dwFlags) {
  UNREFERENCED_PARAMETER(acpPos);

  _ClearRequestedAttributes();

  for (int i = 0; i < NUM_SUPPORTED_ATTRS; i++) {
    for (ULONG x = 0; x < cFilterAttrs; x++) {
      if (IsEqualGUID(*m_rgAttributes[i].attrid, paFilterAttrs[x])) {
        m_rgAttributes[i].dwFlags = ATTR_FLAG_REQUESTED;
        if (dwFlags & TS_ATTR_FIND_WANT_VALUE) {
          m_rgAttributes[i].dwFlags |= ATTR_FLAG_DEFAULT;
        } else {
          VariantCopy(&m_rgAttributes[i].varValue, &m_rgAttributes[i].varDefaultValue);
        }
      }
    }
  }
  return S_OK;
}

STDMETHODIMP ui::UITextStoreACP::RequestAttrsTransitioningAtPosition(LONG acpPos, ULONG cFilterAttrs,
                                                                     const TS_ATTRID *paFilterAttrs, DWORD dwFlags) {
  UNREFERENCED_PARAMETER(acpPos);
  UNREFERENCED_PARAMETER(cFilterAttrs);
  UNREFERENCED_PARAMETER(paFilterAttrs);
  UNREFERENCED_PARAMETER(dwFlags);

  return E_NOTIMPL;
}

STDMETHODIMP ui::UITextStoreACP::FindNextAttrTransition(LONG acpStart, LONG acpHalt, ULONG cFilterAttrs,
                                                        const TS_ATTRID *paFilterAttrs, DWORD dwFlags, LONG *pacpNext,
                                                        BOOL *pfFound, LONG *plFoundOffset) {
  UNREFERENCED_PARAMETER(acpStart);
  UNREFERENCED_PARAMETER(acpHalt);
  UNREFERENCED_PARAMETER(cFilterAttrs);
  UNREFERENCED_PARAMETER(paFilterAttrs);
  UNREFERENCED_PARAMETER(dwFlags);
  UNREFERENCED_PARAMETER(pacpNext);
  UNREFERENCED_PARAMETER(pfFound);
  UNREFERENCED_PARAMETER(plFoundOffset);

  return E_NOTIMPL;
}

STDMETHODIMP ui::UITextStoreACP::RetrieveRequestedAttrs(ULONG ulCount, TS_ATTRVAL *paAttrVals, ULONG *pcFetched) {
  ULONG uFetched = 0;
  for (int i = 0; i < NUM_SUPPORTED_ATTRS && ulCount; ++i) {
    if (m_rgAttributes[i].dwFlags & ATTR_FLAG_REQUESTED) {
      paAttrVals->varValue.vt = VT_EMPTY;
      CopyMemory(&paAttrVals->idAttr, m_rgAttributes[i].attrid, sizeof(GUID));
      paAttrVals->dwOverlapId = 0;

      if (m_rgAttributes[i].dwFlags & ATTR_FLAG_DEFAULT) {
        VariantCopy(&paAttrVals->varValue, &m_rgAttributes[i].varDefaultValue);
      } else {
        VariantCopy(&paAttrVals->varValue, &m_rgAttributes[i].varValue);
      }

      paAttrVals++;
      uFetched++;
      ulCount--;

      VariantClear(&m_rgAttributes[i].varValue);
      m_rgAttributes[i].dwFlags = ATTR_FLAG_NONE;
    }
  }
  *pcFetched = uFetched;
  return S_OK;
}

STDMETHODIMP ui::UITextStoreACP::GetEndACP(LONG *pacp) {
  if (!_IsLocked(TS_LF_READWRITE)) {
    return TS_E_NOLOCK;
  }

  if (nullptr == pacp) {
    return E_INVALIDARG;
  }

  *pacp = static_cast<LONG>(m_text.length());
  return S_OK;
}

STDMETHODIMP ui::UITextStoreACP::GetActiveView(TsViewCookie *pvcView) {
  *pvcView = EDIT_VIEW_COOKIE;
  return S_OK;
}

STDMETHODIMP ui::UITextStoreACP::GetACPFromPoint(TsViewCookie vcView, const POINT *pt, DWORD dwFlags, LONG *pacp) {
  UNREFERENCED_PARAMETER(vcView);
  UNREFERENCED_PARAMETER(pt);
  UNREFERENCED_PARAMETER(dwFlags);
  UNREFERENCED_PARAMETER(pacp);
  return E_NOTIMPL;
}

STDMETHODIMP ui::UITextStoreACP::GetTextExt(TsViewCookie vcView, LONG acpStart, LONG acpEnd, RECT *prc,
                                            BOOL *pfClipped) {
  if (nullptr == prc || nullptr == pfClipped) {
    return E_INVALIDARG;
  }

  *pfClipped = FALSE;
  ZeroMemory(prc, sizeof(RECT));

  if (EDIT_VIEW_COOKIE != vcView) {
    return E_INVALIDARG;
  }

  if (!_IsLocked(TS_LF_READ)) {
    return TS_E_NOLOCK;
  }

  if (!m_editControl.expired()) {
    return m_editControl.lock()->GetTextExt(acpStart, acpEnd, prc, pfClipped);
  }
  return E_FAIL;
}

STDMETHODIMP ui::UITextStoreACP::GetScreenExt(TsViewCookie vcView, RECT *prc) {
  if (nullptr == prc) {
    return E_INVALIDARG;
  }

  ZeroMemory(prc, sizeof(RECT));

  if (EDIT_VIEW_COOKIE != vcView) {
    return E_INVALIDARG;
  }

  return S_OK;
}

STDMETHODIMP ui::UITextStoreACP::GetWnd(TsViewCookie vcView, HWND *phwnd) {
  if (EDIT_VIEW_COOKIE == vcView) {
    if (!m_editControl.expired()) {
      *phwnd = m_editControl.lock()->GetHWnd();
      return S_OK;
    }
  }
  return E_INVALIDARG;
}

STDMETHODIMP ui::UITextStoreACP::InsertTextAtSelection(DWORD dwFlags,
                                                       const WCHAR *pwszText, // null if cch is 0
                                                       ULONG cch, LONG *pacpStart, LONG *pacpEnd,
                                                       TS_TEXTCHANGE *pChange) {
  LONG lTemp;

  if (!_IsLocked(TS_LF_READWRITE)) {
    return TS_E_NOLOCK;
  }

  if (cch > 0L && nullptr == pwszText) {
    return E_POINTER;
  }

  if (nullptr == pacpStart) {
    pacpStart = &lTemp;
  }

  if (nullptr == pacpEnd) {
    pacpEnd = &lTemp;
  }

  LONG acpStart;
  LONG acpOldEnd;
  LONG acpNewEnd;

  _GetCurrentSelection();

  acpOldEnd = m_acpEnd;
  HRESULT hr = _TestInsert(m_acpStart, m_acpEnd, cch, &acpStart, &acpNewEnd);
  if (FAILED(hr)) {
    return hr;
  }

  if (dwFlags & TS_IAS_QUERYONLY) {
    *pacpStart = acpStart;
    *pacpEnd = acpNewEnd;
    return S_OK;
  }

  m_fNotify = FALSE;

  std::wstring s(pwszText, cch);

  if ((acpOldEnd - acpStart > 0) && (acpNewEnd - acpStart > 0)) {
    _Internal_ReplaceText(acpStart, acpOldEnd - acpStart, s);
  } else if (acpOldEnd - acpStart > 0) {
    _Internal_RemoveText(acpStart, acpOldEnd - acpStart);
  } else if (acpNewEnd - acpStart > 0) {
    _Internal_InsertText(acpStart, s);
  }

  m_fNotify = TRUE;

  _GetCurrentSelection();

  if (!(dwFlags & TS_IAS_NOQUERY)) {
    *pacpStart = acpStart;
    *pacpEnd = acpNewEnd;
  }

  pChange->acpStart = acpStart;
  pChange->acpOldEnd = acpOldEnd;
  pChange->acpNewEnd = acpNewEnd;

  m_fLayoutChanged = TRUE;

  if (!m_editControl.expired()) {
    m_editControl.lock()->NotifySelectionHasChanged();
  }
  return S_OK;
}

STDMETHODIMP ui::UITextStoreACP::InsertEmbeddedAtSelection(DWORD dwFlags, IDataObject *pDataObject, LONG *pacpStart,
                                                           LONG *pacpEnd, TS_TEXTCHANGE *pChange) {
  UNREFERENCED_PARAMETER(dwFlags);
  UNREFERENCED_PARAMETER(pDataObject);
  UNREFERENCED_PARAMETER(pacpStart);
  UNREFERENCED_PARAMETER(pacpEnd);
  UNREFERENCED_PARAMETER(pChange);

  return E_NOTIMPL;
}

HRESULT ui::UITextStoreACP::_ClearAdviseSink(PADVISE_SINK pAdviseSink) {
  if (pAdviseSink->punkID) {
    pAdviseSink->punkID->Release();
    pAdviseSink->punkID = nullptr;
  }

  if (pAdviseSink->pTextStoreACPSink) {
    pAdviseSink->pTextStoreACPSink->Release();
    pAdviseSink->pTextStoreACPSink = nullptr;
  }

  pAdviseSink->dwMask = 0;

  return S_OK;
}

BOOL ui::UITextStoreACP::_LockDocument(DWORD dwLockFlags) {
  if (m_fLocked) {
    return FALSE;
  }

  m_fLocked = TRUE;
  m_dwLockType = dwLockFlags;

  return TRUE;
}

void ui::UITextStoreACP::_UnlockDocument() {
  HRESULT hr;

  m_fLocked = FALSE;
  m_dwLockType = 0;

  if (m_fPendingLockUpgrade) {
    m_fPendingLockUpgrade = FALSE;

    RequestLock(TS_LF_READWRITE, &hr);
  }

  if (m_fLayoutChanged) {
    m_fLayoutChanged = FALSE;
    m_AdviseSink.pTextStoreACPSink->OnLayoutChange(TS_LC_CHANGE, EDIT_VIEW_COOKIE);
  }
}

BOOL ui::UITextStoreACP::_IsLocked(DWORD dwLockType) {
  if (m_dwInternalLockType) {
    return TRUE;
  }

  return m_fLocked && (m_dwLockType & dwLockType);
}

void ui::UITextStoreACP::_ClearRequestedAttributes(void) {
  int i;

  for (i = 0; i < NUM_SUPPORTED_ATTRS; i++) {
    VariantClear(&m_rgAttributes[i].varValue);
    m_rgAttributes[i].dwFlags = ATTR_FLAG_NONE;
  }
}

BOOL ui::UITextStoreACP::InitializeAttributes() {
  m_rgAttributes[ATTR_INDEX_TEXT_ORIENTATION].dwFlags = 0;
  m_rgAttributes[ATTR_INDEX_TEXT_ORIENTATION].attrid = &TSATTRID_Text_Orientation;
  VariantInit(&m_rgAttributes[ATTR_INDEX_TEXT_ORIENTATION].varValue);
  m_rgAttributes[ATTR_INDEX_TEXT_ORIENTATION].varDefaultValue.vt = VT_I4;
  m_rgAttributes[ATTR_INDEX_TEXT_ORIENTATION].varDefaultValue.lVal = 0;

  m_rgAttributes[ATTR_INDEX_TEXT_VERTICALWRITING].dwFlags = 0;
  m_rgAttributes[ATTR_INDEX_TEXT_VERTICALWRITING].attrid = &TSATTRID_Text_VerticalWriting;
  VariantInit(&m_rgAttributes[ATTR_INDEX_TEXT_VERTICALWRITING].varValue);
  m_rgAttributes[ATTR_INDEX_TEXT_VERTICALWRITING].varDefaultValue.vt = VT_BOOL;
  m_rgAttributes[ATTR_INDEX_TEXT_VERTICALWRITING].varDefaultValue.lVal = FALSE;

  return TRUE;
}

BOOL ui::UITextStoreACP::_GetCurrentSelection(void) {
  return TRUE;
}

STDMETHODIMP ui::UITextStoreACP::_TestInsert(LONG acpTestStart, LONG acpTestEnd, ULONG cch, LONG *pacpResultStart,
                                             LONG *pacpResultEnd) {
  if ((acpTestStart > acpTestEnd) || (acpTestStart > static_cast<LONG>(m_text.length()))) {
    LOG(SEVERITY_LEVEL_ERROR) << L"hr = E_INVALIDARG";
    return E_INVALIDARG;
  }

  *pacpResultStart = acpTestStart;
  *pacpResultEnd = acpTestStart + cch;
  return S_OK;
}

// HWND ui::UITextStoreACP::_GetWindow()
// {
//     return m_hWnd;
// }

HRESULT ui::UITextStoreACP::_GetText(std::wstring &text, LPLONG pcch) {
  text = m_text;
  *pcch = static_cast<LONG>(m_text.length());
  return S_OK;
}

HRESULT ui::UITextStoreACP::TerminateAllCompositions(void) {
  HRESULT hr = S_OK;
  if (!m_editControl.expired()) {
    auto editControl = m_editControl.lock();
    ITfContextOwnerCompositionServices *pCompServices = nullptr;
    hr = editControl->GetContext()->QueryInterface(IID_ITfContextOwnerCompositionServices, (LPVOID *)&pCompServices);
    if (SUCCEEDED(hr)) {
      hr = pCompServices->TerminateComposition(nullptr);
      pCompServices->Release();
    }
  }
  return hr;
}

HRESULT ui::UITextStoreACP::ChangeACPWithoutLock(LONG acpStart, LONG acpEnd, TsActiveSelEnd activeSelEnd) {
  m_acpStart = acpStart;
  m_acpEnd = acpEnd;
  m_ActiveSelEnd = activeSelEnd;

  m_AdviseSink.pTextStoreACPSink->OnSelectionChange();
  return S_OK;
}

HRESULT ui::UITextStoreACP::MoveACPToEnd() {
  if (_IsLocked(TS_LF_READ)) {
    return S_OK;
  }
  _LockDocument(TS_LF_READWRITE);
  HRESULT hr
      = ChangeACPWithoutLock(static_cast<LONG>(GetTextLength()), static_cast<LONG>(GetTextLength()), TS_AE_START);
  _UnlockDocument();
  m_AdviseSink.pTextStoreACPSink->OnLayoutChange(TS_LC_CHANGE, EDIT_VIEW_COOKIE);
  return hr;
}

void ui::UITextStoreACP::PushWndMessage(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) {
  m_messageQueue.Push(hWnd, message, wParam, lParam);
}

HRESULT ui::UITextStoreACP::InsertTextAtACP(std::wstring &text) {
  if (_IsLocked(TS_LF_READ)) {
    return S_OK;
  }
  _LockDocument(TS_LF_READWRITE);

  bool changed = false;
  TS_TEXTCHANGE ts;
  if (m_acpStart == m_acpEnd) {
    _Internal_InsertText(m_acpStart, text);
    ts.acpStart = m_acpStart;
    ts.acpOldEnd = m_acpStart;
    ts.acpNewEnd = static_cast<LONG>(m_acpStart + text.length());
    changed = true;
    m_acpStart = m_acpEnd = m_acpStart + 1;
  } else {
    _Internal_ReplaceText(m_acpStart, m_acpEnd - m_acpStart, text);
    ts.acpStart = m_acpStart;
    ts.acpOldEnd = m_acpEnd;
    ts.acpNewEnd = static_cast<LONG>(m_acpStart + text.length());
    changed = true;
    m_acpStart = m_acpEnd = static_cast<LONG>(m_acpStart + text.length());
  }

  if (changed) {
    m_AdviseSink.pTextStoreACPSink->OnTextChange(0, &ts);
  }

  _UnlockDocument();

  m_AdviseSink.pTextStoreACPSink->OnLayoutChange(TS_LC_CHANGE, EDIT_VIEW_COOKIE);
  return S_OK;
}

ITextStoreACPSink *ui::UITextStoreACP::GetTextStoreACPSink() {
  return m_AdviseSink.pTextStoreACPSink;
}

void ui::UITextStoreACP::_Internal_InsertText(LONG startPos, std::wstring &s) {
  LONG oldLength = static_cast<LONG>(m_text.length());
  m_text.insert(startPos, s);
  auto pEditControl = m_editControl.lock();
  pEditControl->NotifyTextInsert(oldLength, startPos, static_cast<LONG>(s.length()));
}

void ui::UITextStoreACP::_Internal_RemoveText(LONG startPos, LONG length) {
  LONG oldLength = static_cast<LONG>(m_text.length());
  m_text.erase(startPos, length);
  auto pEditControl = m_editControl.lock();
  pEditControl->NotifyTextRemove(oldLength, startPos, length);
}

void ui::UITextStoreACP::_Internal_ReplaceText(LONG startPos, LONG length, std::wstring &s) {
  LONG oldLength = static_cast<LONG>(m_text.length());
  m_text.replace(startPos, length, s);
  auto pEditControl = m_editControl.lock();
  pEditControl->NotifyTextReplace(oldLength, startPos, length, static_cast<LONG>(s.length()));
}

HRESULT ui::UITextStoreACP::OnChar(HWND hWnd, WPARAM wParam, LPARAM lParam, CHAR_TYPE charType, wchar_t c,
                                   bool isShiftKeyDown) {
  UNREFERENCED_PARAMETER(hWnd);
  UNREFERENCED_PARAMETER(wParam);
  UNREFERENCED_PARAMETER(lParam);

  switch (charType) {
  case CHAR_TYPE_NORMAL: {
    std::wstring s(1, (wchar_t)c);
    InsertTextAtACP(s);
  } break;
  case CHAR_TYPE_BACKSPACE:
    Backspace();
    break;
  case CHAR_TYPE_DELETE:
    Delete();
    break;
  case CHAR_TYPE_CURSOR_LEFT:
    CursorLeft(isShiftKeyDown);
    break;
  case CHAR_TYPE_CURSOR_RIGHT:
    CursorRight(isShiftKeyDown);
    break;
  }
  return S_OK;
}

HRESULT ui::UITextStoreACP::Backspace() {
  if (_IsLocked(TS_LF_READ)) {
    return S_OK;
  }
  _LockDocument(TS_LF_READWRITE);

  bool changed = false;
  TS_TEXTCHANGE ts;
  if (m_acpStart == m_acpEnd) {
    if (m_acpStart > 0) {
      _Internal_RemoveText(m_acpStart - 1, 1);
      ts.acpStart = m_acpStart - 1;
      ts.acpOldEnd = m_acpStart;
      ts.acpNewEnd = m_acpStart - 1;
      changed = true;
      m_acpStart = m_acpEnd = m_acpStart - 1;
    }
  } else {
    _Internal_RemoveText(m_acpStart, m_acpEnd - m_acpStart);
    ts.acpStart = m_acpStart;
    ts.acpOldEnd = m_acpEnd;
    ts.acpNewEnd = m_acpStart;
    changed = true;
    m_acpEnd = m_acpStart;
  }

  if (changed) {
    m_AdviseSink.pTextStoreACPSink->OnTextChange(0, &ts);
  }

  _UnlockDocument();
  m_AdviseSink.pTextStoreACPSink->OnLayoutChange(TS_LC_CHANGE, EDIT_VIEW_COOKIE);
  return S_OK;
}

HRESULT ui::UITextStoreACP::Delete() {
  if (_IsLocked(TS_LF_READ)) {
    return S_OK;
  }
  _LockDocument(TS_LF_READWRITE);

  bool changed = false;
  TS_TEXTCHANGE ts;
  if (m_acpStart == m_acpEnd) {
    if (m_acpStart < (LONG)m_text.length()) {
      _Internal_RemoveText(m_acpStart, 1);
      ts.acpStart = m_acpStart;
      ts.acpOldEnd = m_acpStart + 1;
      ts.acpNewEnd = m_acpStart;
      changed = true;
    }
  } else {
    _Internal_RemoveText(m_acpStart, m_acpEnd - m_acpStart);
    ts.acpStart = m_acpStart;
    ts.acpOldEnd = m_acpEnd;
    ts.acpNewEnd = m_acpStart;
    changed = true;
  }

  if (changed) {
    m_AdviseSink.pTextStoreACPSink->OnTextChange(0, &ts);
  }

  _UnlockDocument();

  m_AdviseSink.pTextStoreACPSink->OnLayoutChange(TS_LC_CHANGE, EDIT_VIEW_COOKIE);
  return S_OK;
}

HRESULT ui::UITextStoreACP::CursorLeft(bool isShiftKeyDown) {
  if (_IsLocked(TS_LF_READ)) {
    return S_OK;
  }
  _LockDocument(TS_LF_READWRITE);

  LONG newAcpStart = GetACPStart();
  LONG newAcpEnd = GetACPEnd();
  TsActiveSelEnd newActiveSelEnd = GetActiveSelEnd();

  if (!isShiftKeyDown) {
    if (newAcpStart == newAcpEnd) {
      newAcpStart--;
      if (newAcpStart < 0) {
        newAcpStart = 0;
      }
      newAcpEnd = newAcpStart;
      newActiveSelEnd = TS_AE_START;
    } else {
      newAcpEnd = newAcpStart;
      newActiveSelEnd = TS_AE_START;
    }
  } else {
    switch (newActiveSelEnd) {
    case TS_AE_END:
      newAcpEnd--;
      if (newAcpEnd < 0) {
        newAcpEnd = 0;
      }
      if (newAcpEnd < newAcpStart) {
        // caret position move over the end of the selection
        std::swap(newAcpStart, newAcpEnd);
        newActiveSelEnd = TS_AE_START;
      }
      break;
    default: // TS_AE_START, TS_AE_NONE
      newAcpStart--;
      if (newAcpStart < 0) {
        newAcpStart = 0;
      }
      break;
    }
    if (newAcpStart == newAcpEnd) {
      // there is no selection, then the caret move to the start of the selection
      newActiveSelEnd = TS_AE_START;
    }
  }

  m_acpStart = newAcpStart;
  m_acpEnd = newAcpEnd;
  m_ActiveSelEnd = newActiveSelEnd;
  m_AdviseSink.pTextStoreACPSink->OnSelectionChange();

  _UnlockDocument();
  return S_OK;
}

HRESULT ui::UITextStoreACP::CursorRight(bool isShiftKeyDown) {
  if (_IsLocked(TS_LF_READ)) {
    return S_OK;
  }
  _LockDocument(TS_LF_READWRITE);

  LONG newAcpStart = GetACPStart();
  LONG newAcpEnd = GetACPEnd();
  TsActiveSelEnd newActiveSelEnd = GetActiveSelEnd();

  if (!isShiftKeyDown) {
    if (newAcpStart == newAcpEnd) {
      newAcpStart++;
      if (newAcpStart > static_cast<LONG>(GetTextLength())) {
        newAcpStart = static_cast<LONG>(GetTextLength());
      }
      newAcpEnd = newAcpStart;
      newActiveSelEnd = TS_AE_START;
    } else {
      newAcpStart = newAcpEnd;
      newActiveSelEnd = TS_AE_START;
    }
  } else {
    switch (newActiveSelEnd) {
    case TS_AE_START:
      newAcpStart++;
      if (newAcpStart > static_cast<LONG>(GetTextLength())) {
        newAcpStart = static_cast<LONG>(GetTextLength());
      }
      if (newAcpEnd < newAcpStart) {
        // caret position move over the end of the selection
        std::swap(newAcpStart, newAcpEnd);
        newActiveSelEnd = TS_AE_END;
      }
      break;
    default: // TS_AE_END, TS_AE_NONE
      newAcpEnd++;
      if (newAcpEnd > static_cast<LONG>(GetTextLength())) {
        newAcpEnd = static_cast<LONG>(GetTextLength());
      }
      break;
    }
    if (newAcpStart == newAcpEnd) {
      // there is no selection, then the caret move to the start of the selection
      newActiveSelEnd = TS_AE_START;
    }
  }

  m_acpStart = newAcpStart;
  m_acpEnd = newAcpEnd;
  m_ActiveSelEnd = newActiveSelEnd;
  m_AdviseSink.pTextStoreACPSink->OnSelectionChange();

  _UnlockDocument();
  return S_OK;
}

HRESULT ui::UITextStoreACP::OnMouseEvent(FLOAT startX, FLOAT startY, FLOAT endX, FLOAT endY) {
  if (_IsLocked(TS_LF_READ)) {
    return S_OK;
  }
  _LockDocument(TS_LF_READWRITE);

  bool inside = false;
  LONG acpStart, acpEnd;
  TsActiveSelEnd activeSelEnd;
  m_editControl.lock()->CalculateSelectionFromDragRect(startX, startY, endX, endY, &inside, &acpStart, &acpEnd,
                                                       &activeSelEnd);
  if (inside) {
    m_acpStart = acpStart;
    m_acpEnd = acpEnd;
    m_ActiveSelEnd = activeSelEnd;
  }
  _UnlockDocument();
  GetTextStoreACPSink()->OnLayoutChange(TS_LC_CHANGE, EDIT_VIEW_COOKIE);
  m_editControl.lock()->NotifySelectionHasChanged();
  return S_OK;
}

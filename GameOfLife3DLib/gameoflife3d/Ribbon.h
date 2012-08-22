#pragma once

#include "Common.h"

namespace gameOfLife3D
{
class RibbonFactory;

class Ribbon :
    public CComObjectRootEx<CComMultiThreadModel>,
    public IUIApplication,
    public IUICommandHandler
{
private:
    RibbonFactory *m_pRibbonFactory;
    IUIRibbon *m_pUiRibbon;

public:
    BEGIN_COM_MAP(Ribbon)
    COM_INTERFACE_ENTRY(IUIApplication)
    COM_INTERFACE_ENTRY(IUICommandHandler)
    END_COM_MAP()

    Ribbon(void);
    ~Ribbon(void);

    HRESULT Initialize(RibbonFactory *pRibbonFactory);
    UINT GetRibbonHeight() const {
        UINT ribbonHeight = 0;
        m_pUiRibbon->GetHeight(&ribbonHeight);
        return ribbonHeight;
    }

    STDMETHOD(OnViewChanged)(
        UINT32 nViewID,
        __in UI_VIEWTYPE typeID,
        __in IUnknown* pView,
        UI_VIEWVERB verb,
        INT32 uReasonCode);

    void updateRibbonHeight();


    STDMETHOD(OnCreateUICommand)(
        UINT32 nCmdID,
        __in UI_COMMANDTYPE typeID,
        __deref_out IUICommandHandler** ppCommandHandler);

    STDMETHOD(OnDestroyUICommand)(
        UINT32 commandId,
        __in UI_COMMANDTYPE typeID,
        __in_opt IUICommandHandler* pCommandHandler);

    STDMETHODIMP Execute(
        UINT nCmdID,
        UI_EXECUTIONVERB verb,
        __in_opt const PROPERTYKEY* key,
        __in_opt const PROPVARIANT* ppropvarValue,
        __in_opt IUISimplePropertySet* pCommandExecutionProperties);

    STDMETHODIMP UpdateProperty(
        UINT nCmdID,
        __in REFPROPERTYKEY key,
        __in_opt const PROPVARIANT* ppropvarCurrentValue,
        __out PROPVARIANT* ppropvarNewValue);
};

}

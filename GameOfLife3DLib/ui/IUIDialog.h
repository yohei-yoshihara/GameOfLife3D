#ifndef UI_IUIDIALOG_H_
#define UI_IUIDIALOG_H_

#include "ui/UICommon.h"
#include "ui/UIContainer.h"

namespace ui {
  enum DIALOG_STATUS {
    DIALOG_STATUS_CLOSE,
    DIALOG_STATUS_DISPLAY,
  };
  enum DIALOG_RESULT {
    DIALOG_RESULT_OK,
    DIALOG_RESULT_CANCEL,
    DIALOG_RESULT_YES,
    DIALOG_RESULT_NO,
  };

  typedef std::function<void(std::shared_ptr<ui::IUIDialog>)> BeforeDialogDisplayedCallback;
  typedef std::function<void(std::shared_ptr<ui::IUIDialog>)> DialogClosedCallback;

  class IUIDialog : public ui::UIContainer {
  public:
    virtual void SetTitle(const std::wstring &title) = 0;
    virtual const std::wstring &GetTitle() const = 0;
    virtual void ShowDialog() = 0;
    virtual void CloseDialog() = 0;
    virtual DIALOG_STATUS GetDialogStatus() const = 0;
    virtual DIALOG_RESULT GetDialogResult() const = 0;

    virtual void AddBeforeDialogDisplayedCallback(BeforeDialogDisplayedCallback callback) = 0;
    virtual void FireBeforeDialogDisplayedCallbacks() = 0;
    virtual void ClearBeforeDialogDisplayedCallbacks() = 0;

    virtual void AddDialogClosedCallback(DialogClosedCallback callback) = 0;
    virtual void FireDialogClosedCallbacks() = 0;
    virtual void ClearDialogClosedCallbacks() = 0;
  };
}

#endif // UI_IUIDIALOG_H_

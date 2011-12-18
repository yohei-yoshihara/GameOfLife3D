#ifndef GAMEOFLIFE3D_DIALOG_SETTINGSDIALOG_H_
#define GAMEOFLIFE3D_DIALOG_SETTINGSDIALOG_H_

#include "ui/UICommon.h"
#include "ui/UIInternalDialog.h"
#include "gameOfLife3D/dialogs/SettingData.h"

namespace gameOfLife3D
{
namespace dialogs
{

enum SETTINGSDIALOG_RESULT {
    SETTINGSDIALOG_RESULT_OK,
    SETTINGSDIALOG_RESULT_CANCEL,
};
typedef std::function<void(gameOfLife3D::dialogs::SettingData)> SettingModifiedCallback;

class SettingsDialog
{
private:
    std::shared_ptr<ui::UIInternalDialog> m_dialog;
    SettingData m_settingData;
    std::weak_ptr<ui::UITextField> m_widthTextField;
    std::weak_ptr<ui::UITextField> m_heightTextField;
    std::weak_ptr<ui::UITextField> m_generationsTextField;
    std::weak_ptr<ui::UIRadioButton> m_slowRadioButton;
    std::weak_ptr<ui::UIRadioButton> m_normalRadioButton;
    std::weak_ptr<ui::UIRadioButton> m_fastRadioButton;
    std::weak_ptr<ui::UIRadioButton> m_cpuVertexGenerationButton;
    std::weak_ptr<ui::UIRadioButton> m_gpuVertexGenerationButton;
    std::weak_ptr<ui::UILabel> m_errorLabel;
    SETTINGSDIALOG_RESULT m_result;
    HWND m_hwnd;
    SettingModifiedCallback m_settingModifiedCallback;

public:
    SettingsDialog();
    virtual ~SettingsDialog();
    void SetHWnd(HWND hWnd) {
        m_hwnd = hWnd;
    }
    HRESULT Initialize(
        std::shared_ptr<tsf::TSFManager> pTsfManager);
    std::shared_ptr<ui::UIInternalDialog> GetDialog() const {
        return m_dialog;
    }
    void SetSettingModifiedCallback(SettingModifiedCallback settingModifiedCallback) {
        m_settingModifiedCallback = settingModifiedCallback;
    }
    void SetData(SettingData settingData) {
        m_settingData = settingData;
    }
    SettingData GetData() const {
        return m_settingData;
    }
    SETTINGSDIALOG_RESULT GetResult() const {
        return m_result;
    }
};
}
}

#endif // GAMEOFLIFE3D_DIALOG_SETTINGSDIALOG_H_

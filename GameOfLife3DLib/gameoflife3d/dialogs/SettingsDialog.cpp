#include "stdafx.h"
#include "Common.h"
#include "tsf/TSFManager.h"
#include "mui/MUI.h"
#include "ui/UILabel.h"
#include "ui/UITextField.h"
#include "ui/UIButton.h"
#include "ui/UIRadioButton.h"
#include "ui/UIRadioButtonGroup.h"
#include "gameOfLife3D/dialogs/SettingsDialog.h"

gameOfLife3D::dialogs::SettingsDialog::SettingsDialog()
    : m_dialog(std::make_shared<ui::UIInternalDialog>()) {
  m_settingData.width = 24;
  m_settingData.height = 24;
  m_settingData.generations = 24;
  m_settingData.updateInterval = 1000;
  m_settingData.vertexGenerationMethod = VERTEX_GENERATION_METHOD_GPU;
}

gameOfLife3D::dialogs::SettingsDialog::~SettingsDialog() {
}

HRESULT gameOfLife3D::dialogs::SettingsDialog::Initialize(std::shared_ptr<tsf::TSFManager> pTsfManager) {
  const static FLOAT ROW0_WIDTH = 120.0f /*90.0f*/;
  const static FLOAT ROW1_WIDTH = 120.0f;
  const static float MARGIN = 10.0f;

  m_dialog->SetBounds(0.0f, 0.0f, 325, 350 /*285*/);
  m_dialog->SetTitle(mui::MUI::GetString(IDS_SETTINGS));
  m_dialog->AddBeforeDialogDisplayedCallback([&](std::shared_ptr<ui::IUIDialog>) -> void {
    // width
    std::wostringstream ss_width;
    ss_width << m_settingData.width;
    if (!m_widthTextField.expired()) {
      m_widthTextField.lock()->SetText(ss_width.str());
    }

    // height
    std::wostringstream ss_height;
    ss_height << m_settingData.height;
    if (!m_heightTextField.expired()) {
      m_heightTextField.lock()->SetText(ss_height.str());
    }

    // generations
    std::wostringstream ss_generations;
    ss_generations << m_settingData.generations;
    if (!m_generationsTextField.expired()) {
      m_generationsTextField.lock()->SetText(ss_generations.str());
    }

    // speed
    if (!m_slowRadioButton.expired() && !m_normalRadioButton.expired() && !m_fastRadioButton.expired()) {
      if (m_settingData.updateInterval < 1000) {
        m_fastRadioButton.lock()->SetSelected(true);
      } else if (m_settingData.updateInterval > 1000) {
        m_slowRadioButton.lock()->SetSelected(true);
      } else {
        m_normalRadioButton.lock()->SetSelected(true);
      }
    }

    // vertex generation
    if (!m_cpuVertexGenerationButton.expired() && !m_gpuVertexGenerationButton.expired()) {
      if (m_settingData.vertexGenerationMethod == VERTEX_GENERATION_METHOD_CPU) {
        m_cpuVertexGenerationButton.lock()->SetSelected(true);
      } else {
        m_gpuVertexGenerationButton.lock()->SetSelected(true);
      }
    }

    if (!m_errorLabel.expired()) {
      m_errorLabel.lock()->SetText(L"");
    }
  });
  float y = MARGIN;

  // 1st column: Width
  auto widthLabel = std::make_shared<ui::UILabel>();
  widthLabel->SetBounds(MARGIN, y, ROW0_WIDTH, 25);
  widthLabel->SetText(mui::MUI::GetString(IDS_WIDTH));
  m_dialog->AddElement(widthLabel);

  auto widthTextField = std::make_shared<ui::UITextField>();
  widthTextField->SetTSFManager(pTsfManager);
  widthTextField->SetBounds(ROW0_WIDTH + MARGIN * 2, y, ROW1_WIDTH, 25);
  widthTextField->SetText(L"24");
  m_dialog->AddElement(widthTextField);
  m_widthTextField = widthTextField;

  y += widthLabel->GetHeight() + MARGIN;

  // 2nd column: Height
  auto heightLabel = std::make_shared<ui::UILabel>();
  heightLabel->SetBounds(MARGIN, y, ROW0_WIDTH, 25);
  heightLabel->SetText(mui::MUI::GetString(IDS_HEIGHT));
  m_dialog->AddElement(heightLabel);

  auto heightTextField = std::make_shared<ui::UITextField>();
  heightTextField->SetTSFManager(pTsfManager);
  heightTextField->SetBounds(ROW0_WIDTH + MARGIN * 2, y, ROW1_WIDTH, 25);
  heightTextField->SetText(L"24");
  m_dialog->AddElement(heightTextField);
  m_heightTextField = heightTextField;

  y += heightLabel->GetHeight() + MARGIN;

  // 3rd column: Generations
  auto generationsLabel = std::make_shared<ui::UILabel>();
  generationsLabel->SetBounds(MARGIN, y, ROW0_WIDTH, 25);
  generationsLabel->SetText(mui::MUI::GetString(IDS_GENERATIONS));
  m_dialog->AddElement(generationsLabel);

  auto generationsTextField = std::make_shared<ui::UITextField>();
  generationsTextField->SetTSFManager(pTsfManager);
  generationsTextField->SetBounds(ROW0_WIDTH + MARGIN * 2, y, ROW1_WIDTH, 25);
  generationsTextField->SetText(L"24");
  m_dialog->AddElement(generationsTextField);
  m_generationsTextField = generationsTextField;

  y += generationsLabel->GetHeight() + MARGIN;

  // 4th column: Speed
  auto speedLabel = std::make_shared<ui::UILabel>();
  speedLabel->SetBounds(MARGIN, y, ROW0_WIDTH, 25);
  speedLabel->SetText(mui::MUI::GetString(IDS_SPEED));
  m_dialog->AddElement(speedLabel);

  auto speedRadioButtonGroup = std::make_shared<ui::UIRadioButtonGroup>();
  speedRadioButtonGroup->SetBounds(ROW0_WIDTH + MARGIN * 2, y, ROW1_WIDTH, 25);
  auto slowRadioButton = std::make_shared<ui::UIRadioButton>();
  slowRadioButton->SetText(mui::MUI::GetString(IDS_SPEED_SLOW));
  slowRadioButton->SetSelected(true);
  speedRadioButtonGroup->AddElement(slowRadioButton);
  auto normalRadioButton = std::make_shared<ui::UIRadioButton>();
  normalRadioButton->SetText(mui::MUI::GetString(IDS_SPEED_NORMAL));
  speedRadioButtonGroup->AddElement(normalRadioButton);
  auto fastRadioButton = std::make_shared<ui::UIRadioButton>();
  fastRadioButton->SetText(mui::MUI::GetString(IDS_SPEED_FAST));
  speedRadioButtonGroup->AddElement(fastRadioButton);
  m_dialog->AddElement(speedRadioButtonGroup);
  m_slowRadioButton = slowRadioButton;
  m_fastRadioButton = fastRadioButton;
  m_normalRadioButton = normalRadioButton;

  y += 25 * 3 + MARGIN;

  // 5th column: Vertex Generation
  auto vertexGenerationLabel = std::make_shared<ui::UILabel>();
  vertexGenerationLabel->SetBounds(MARGIN, y, ROW0_WIDTH, 25);
  vertexGenerationLabel->SetText(mui::MUI::GetString(IDS_VERTEXGENERATION));
  m_dialog->AddElement(vertexGenerationLabel);

  auto vertexGenerationRadioButtonGroup = std::make_shared<ui::UIRadioButtonGroup>();
  vertexGenerationRadioButtonGroup->SetBounds(ROW0_WIDTH + MARGIN * 2, y, ROW1_WIDTH, 25);
  auto cpuVertexGenerationRadioButton = std::make_shared<ui::UIRadioButton>();
  cpuVertexGenerationRadioButton->SetText(mui::MUI::GetString(IDS_VERTEXGENERATION_CPU));
  cpuVertexGenerationRadioButton->SetSelected(true);
  vertexGenerationRadioButtonGroup->AddElement(cpuVertexGenerationRadioButton);
  auto gpuVertexGenerationRadioButton = std::make_shared<ui::UIRadioButton>();
  gpuVertexGenerationRadioButton->SetText(mui::MUI::GetString(IDS_VERTEXGENERATION_GPU));
  vertexGenerationRadioButtonGroup->AddElement(gpuVertexGenerationRadioButton);
  m_dialog->AddElement(vertexGenerationRadioButtonGroup);
  m_cpuVertexGenerationButton = cpuVertexGenerationRadioButton;
  m_gpuVertexGenerationButton = gpuVertexGenerationRadioButton;

  y += 25 * 2 + MARGIN;

  // 6th column: Error
  auto errorLabel = std::make_shared<ui::UILabel>();
  errorLabel->SetBounds(10, y, 300, 25);
  errorLabel->SetText(L"");
  std::vector<D2D1_GRADIENT_STOP> m_fgColor(2);
  m_fgColor[0] = D2D1::GradientStop(0.0f, D2D1::ColorF(D2D1::ColorF::Red));
  m_fgColor[1] = D2D1::GradientStop(1.0f, D2D1::ColorF(D2D1::ColorF::Red));
  errorLabel->SetForegroundColor(m_fgColor, graphics::GRADIENT_DIRECTION_TOP_BOTTOM);
  m_dialog->AddElement(errorLabel);
  m_errorLabel = errorLabel;

  auto okButton = std::make_shared<ui::UIButton>();
  okButton->SetBounds(10, 10, 100, 25);
  okButton->SetAnchor(ui::UIAnchor(ui::ANCHOR_RIGHT | ui::ANCHOR_BOTTOM, 0.0f, 0.0f, 120.0f, 10.0f));
  okButton->SetText(mui::MUI::GetString(IDS_OK));
  okButton->AddActionListener([&](ui::event::UIActionEvent event) {
    const static std::wregex rx(L"[0-9]*");
    bool modified = false;
    // width
    const std::wstring &width = m_widthTextField.lock()->GetText();
    if (std::regex_match(width.begin(), width.end(), rx)) {
      wchar_t *offset;
      INT newWidth = wcstol(width.c_str(), &offset, 10);
      if (newWidth <= 0) {
        if (!m_errorLabel.expired()) {
          m_errorLabel.lock()->SetText(mui::MUI::GetString(IDS_SETTINGSDIALOG_ERROR_000));
        }
        return;
      } else if (newWidth > 1024) {
        if (!m_errorLabel.expired()) {
          m_errorLabel.lock()->SetText(mui::MUI::GetString(IDS_SETTINGSDIALOG_ERROR_001));
        }
        return;
      } else { /* if (static_cast<INT>(m_data.width) != newWidth) */
        m_settingData.width = newWidth;
        modified = true;
      }
    } else {
      if (!m_errorLabel.expired()) {
        m_errorLabel.lock()->SetText(mui::MUI::GetString(IDS_SETTINGSDIALOG_ERROR_002));
      }
      return;
    }

    // height
    const std::wstring &height = m_heightTextField.lock()->GetText();
    if (std::regex_match(height.begin(), height.end(), rx)) {
      wchar_t *offset;
      INT newHeight = wcstol(height.c_str(), &offset, 10);
      if (newHeight <= 0) {
        if (!m_errorLabel.expired()) {
          m_errorLabel.lock()->SetText(mui::MUI::GetString(IDS_SETTINGSDIALOG_ERROR_003));
        }
        return;
      } else if (newHeight > 1024) {
        if (!m_errorLabel.expired()) {
          m_errorLabel.lock()->SetText(mui::MUI::GetString(IDS_SETTINGSDIALOG_ERROR_004));
        }
        return;
      } else { /* if (static_cast<INT>(m_data.height) != newHeight) */
        m_settingData.height = newHeight;
        modified = true;
      }
    } else {
      if (!m_errorLabel.expired()) {
        m_errorLabel.lock()->SetText(mui::MUI::GetString(IDS_SETTINGSDIALOG_ERROR_005));
      }
      return;
    }

    // generations
    const std::wstring &generations = m_generationsTextField.lock()->GetText();
    if (std::regex_match(generations.begin(), generations.end(), rx)) {
      wchar_t *offset;
      INT newGenerations = wcstol(generations.c_str(), &offset, 10);
      if (newGenerations <= 0) {
        if (!m_errorLabel.expired()) {
          m_errorLabel.lock()->SetText(mui::MUI::GetString(IDS_SETTINGSDIALOG_ERROR_006));
        }
        return;
      } else if (newGenerations > 1024) {
        if (!m_errorLabel.expired()) {
          m_errorLabel.lock()->SetText(mui::MUI::GetString(IDS_SETTINGSDIALOG_ERROR_007));
        }
        return;
      } else { /*if (static_cast<INT>(m_data.generations) != newGenerations)*/
        m_settingData.generations = newGenerations;
        modified = true;
      }
    } else {
      if (!m_errorLabel.expired()) {
        m_errorLabel.lock()->SetText(mui::MUI::GetString(IDS_SETTINGSDIALOG_ERROR_008));
      }
      return;
    }

    // speed
    if (!m_slowRadioButton.expired() && !m_normalRadioButton.expired() && !m_fastRadioButton.expired()) {
      if (m_slowRadioButton.lock()->IsSelected()) {
        m_settingData.updateInterval = 2000;
      } else if (m_fastRadioButton.lock()->IsSelected()) {
        m_settingData.updateInterval = 500;
      } else {
        m_settingData.updateInterval = 1000;
      }
      modified = true;
    }

    // vertex generation
    if (!m_cpuVertexGenerationButton.expired() && !m_gpuVertexGenerationButton.expired()) {
      if (m_cpuVertexGenerationButton.lock()->IsSelected()) {
        m_settingData.vertexGenerationMethod = VERTEX_GENERATION_METHOD_CPU;
      } else {
        m_settingData.vertexGenerationMethod = VERTEX_GENERATION_METHOD_GPU;
      }
      modified = true;
    }

    if (modified) {
      m_settingModifiedCallback(m_settingData);
    }
    m_result = SETTINGSDIALOG_RESULT_OK;
    ui::UIInternalDialog::CloseDialog(event);
  });
  m_dialog->AddElement(okButton);

  auto cancelButton = std::make_shared<ui::UIButton>();
  cancelButton->SetBounds(10, 10, 100, 25);
  cancelButton->SetAnchor(ui::UIAnchor(ui::ANCHOR_RIGHT | ui::ANCHOR_BOTTOM, 0.0f, 0.0f, 10.0f, 10.0f));
  cancelButton->SetText(mui::MUI::GetString(IDS_CANCEL));
  cancelButton->AddActionListener([&](ui::event::UIActionEvent event) {
    m_result = SETTINGSDIALOG_RESULT_CANCEL;
    ui::UIInternalDialog::CloseDialog(event);
  });
  m_dialog->AddElement(cancelButton);

  return S_OK;
}

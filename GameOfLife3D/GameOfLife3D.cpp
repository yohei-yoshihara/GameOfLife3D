#include "stdafx.h"
#include "Common.h"
#include "mui/MUI.h"
#include "util/OSVersionChecker.h"
#include "GameOfLife3D/MainWnd.h"
#include "GameOfLife3D.h"

#include "libs.h"

class CDummyModule : public CAtlExeModuleT<CDummyModule> {};
CDummyModule _Module;

using namespace gameOfLife3D;
using namespace mui;

int APIENTRY wWinMain(HINSTANCE /* hInstance */, HINSTANCE /* hPrevInstance */, LPWSTR /* lpCmdLine */,
                      int /* nCmdShow */) {
  HeapSetInformation(nullptr, HeapEnableTerminationOnCorruption, nullptr, 0);

  srand(static_cast<unsigned>(time(nullptr)));

  // prepare logging
  PWSTR ppszPath = nullptr;
  HRESULT hr = ::SHGetKnownFolderPath(FOLDERID_LocalAppData, 0, nullptr, &ppszPath);
  if (SUCCEEDED(hr)) {
    std::wstring path(ppszPath);
    path += _LOG_FOLDERNAME;
    int ret = ::SHCreateDirectory(nullptr, path.c_str());
    hr = (ret == ERROR_SUCCESS || ret == ERROR_FILE_EXISTS || ret == ERROR_ALREADY_EXISTS) ? S_OK : E_FAIL;
    if (SUCCEEDED(hr)) {
      path += _LOG_FILENAME;
      logging::InitLogger(path.c_str());
    }
  }
  if (FAILED(hr)) {
    TCHAR lpTempPathBuffer[MAX_PATH];
    GetTempPath(MAX_PATH, lpTempPathBuffer);
    std::wstring tempPath(lpTempPathBuffer);
    tempPath += _LOG_FILENAME;
    logging::InitLogger(tempPath.c_str());
  }
  if (ppszPath != nullptr) {
    CoTaskMemFree(ppszPath);
  }

  LPWSTR lpCmdLine = GetCommandLineW();
  SPDLOG_INFO(L"cmdLine=\"{}\"", lpCmdLine);

  LPWSTR *szArglist = nullptr;
  int nArgs = 0;
  szArglist = CommandLineToArgvW(lpCmdLine, &nArgs);
  if (nullptr == szArglist) {
    SPDLOG_ERROR(L"CommandLineToArgvW failed");
    return 0;
  }

  // ULONG numLangs;
  //    SetProcessPreferredUILanguages(MUI_LANGUAGE_NAME, L"en-US", &numLangs);
  //    SetProcessPreferredUILanguages(MUI_LANGUAGE_NAME, L"de-DE", &numLangs);
  //    SetProcessPreferredUILanguages(MUI_LANGUAGE_NAME, L"nl-NL", &numLangs);
  if (MUI::Initialize(L"GameOfLife3D_MUI.dll") != S_OK) {
    return 3;
  }

  if (SUCCEEDED(CoInitialize(nullptr))) {
    {
      auto app = std::make_shared<MainWnd>();

      if (SUCCEEDED(app->Initialize())) {
        app->RunMessageLoop();
      }
    }
    CoUninitialize();
  }
  return 0;
}

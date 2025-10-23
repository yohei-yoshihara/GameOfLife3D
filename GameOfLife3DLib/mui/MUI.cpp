#include "StdAfx.h"
#include "mui/MUI.h"
#include "logging/Logging.h"

mui::MUI::MUI(void) {
}

mui::MUI::~MUI(void) {
}

HMODULE mui::MUI::s_hModule = nullptr;

HRESULT mui::MUI::Initialize(const std::wstring &dllName) {
  if (s_hModule != nullptr) {
    // already loaded
    return S_OK;
  }
  s_hModule = LoadLibraryExW(dllName.c_str(), 0, LOAD_LIBRARY_AS_DATAFILE | LOAD_LIBRARY_AS_IMAGE_RESOURCE);
  if (s_hModule == nullptr) {
    SPDLOG_ERROR(L"Could not load {}", dllName);
    return E_FAIL;
  }
  return S_OK;
}

wchar_t buf[4096];
const wchar_t *mui::MUI::GetString(UINT uID) {
  if (LoadStringW(s_hModule, uID, buf, 4096) != 0) {
    return buf;
  }
  SPDLOG_WARN(L"Could not load the string '{}'", uID);
  buf[0] = '\0';
  return buf;
}

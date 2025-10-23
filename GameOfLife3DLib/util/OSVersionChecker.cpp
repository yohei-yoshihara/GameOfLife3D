#include "StdAfx.h"
#include "logging/Logging.h"
#include "OSVersionChecker.h"
#include <strsafe.h>
#include <VersionHelpers.h>

util::OSVersion util::OSVersionChecker::GetOSVersion() {
  if (IsWindows7OrGreater()) {
    return OSVersion_Windows7;
  } else if (IsWindowsVistaOrGreater()) {
    return OSVersion_WindowsVista;
  } else {
    return OSVersion_WindowsXP;
  }
}

bool util::OSVersionChecker::IsVista() {
  OSVERSIONINFOEX ver;
  DWORDLONG condMask = 0;
  ZeroMemory(&ver, sizeof(OSVERSIONINFOEX));
  ver.dwOSVersionInfoSize = sizeof(OSVERSIONINFOEX);
  ver.dwMajorVersion = 6;
  ver.dwMinorVersion = 0;
  VER_SET_CONDITION(condMask, VER_MAJORVERSION, VER_GREATER_EQUAL);
  VER_SET_CONDITION(condMask, VER_MINORVERSION, VER_GREATER_EQUAL);
  BOOL bRet = VerifyVersionInfo(&ver, VER_MAJORVERSION | VER_MINORVERSION, condMask);
  return bRet == TRUE;
}

bool util::OSVersionChecker::IsWin7() {
  OSVERSIONINFOEX ver;
  DWORDLONG condMask = 0;
  ZeroMemory(&ver, sizeof(OSVERSIONINFOEX));
  ver.dwOSVersionInfoSize = sizeof(OSVERSIONINFOEX);
  ver.dwMajorVersion = 6;
  ver.dwMinorVersion = 1;
  VER_SET_CONDITION(condMask, VER_MAJORVERSION, VER_GREATER_EQUAL);
  VER_SET_CONDITION(condMask, VER_MINORVERSION, VER_GREATER_EQUAL);
  BOOL bRet = VerifyVersionInfo(&ver, VER_MAJORVERSION | VER_MINORVERSION, condMask);
  return bRet == TRUE;
}

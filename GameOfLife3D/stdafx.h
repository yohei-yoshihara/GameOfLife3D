#pragma once

#include "targetver.h"

#ifdef DEBUG
#define DEBUG_RESOURCE
#endif

#include <cstdlib>
#include <ctime>
#include <cwchar>
#include <cmath>
#include <cstdint>
#include <cassert>

#include <string>
#include <iostream>
#include <fstream>
#include <vector>
#include <queue>
#include <stack>
#include <array>
#include <map>
#include <unordered_map>
#include <tuple>
#include <regex>
/*
#ifdef INT8_MIN
#undef INT8_MIN
#endif
#ifdef INT8_MAX
#undef INT8_MAX
#endif
#ifdef INT16_MIN
#undef INT16_MIN
#endif
#ifdef INT16_MAX
#undef INT16_MAX
#endif
#ifdef INT32_MIN
#undef INT32_MIN
#endif
#ifdef INT32_MAX
#undef INT32_MAX
#endif
#ifdef INT64_MIN
#undef INT64_MIN
#endif
#ifdef INT64_MAX
#undef INT64_MAX
#endif

#ifdef UINT8_MIN
#undef UINT8_MIN
#endif
#ifdef UINT8_MAX
#undef UINT8_MAX
#endif
#ifdef UINT16_MIN
#undef UINT16_MIN
#endif
#ifdef UINT16_MAX
#undef UINT16_MAX
#endif
#ifdef UINT32_MIN
#undef UINT32_MIN
#endif
#ifdef UINT32_MAX
#undef UINT32_MAX
#endif
#ifdef UINT64_MIN
#undef UINT64_MIN
#endif
#ifdef UINT64_MAX
#undef UINT64_MAX
#endif
*/
#define WIN32_LEAN_AND_MEAN
#include <windows.h>

#include <ppl.h>
#include <d3d11.h>
#include <d3d11.h>
#include <d3d10_1.h>
//#include <dxerr.h>
#include <DirectXMath.h>
#include <d3dcompiler.h>
//#include <xnamath.h>
#include <d2d1.h>
#include <d2d1helper.h>
#include <dwrite.h>
#include <wincodec.h>
#include <atlbase.h>
#include <atlcom.h>
#include <atlwin.h>
#include <initguid.h>
#include <uiribbon.h>
#include <mshtmcid.h>
#include <msctf.h>
#include <OleCtl.h>
#include <initguid.h>
#include <tsattrs.h>
#include <shlobj.h>
#include <shobjidl.h>
#include <shellapi.h>
#include <knownfolders.h>
#include <UIAutomationCore.h>
#include <UIAnimation.h>

#ifndef Assert
#if defined( DEBUG ) || defined( _DEBUG )
#define Assert(b) do {if (!(b)) {OutputDebugStringA("Assert: " #b "\n");}} while(0)
#else
#define Assert(b)
#endif //DEBUG || _DEBUG
#endif

#define ASSERT assert

#ifndef HINST_THISCOMPONENT
EXTERN_C IMAGE_DOS_HEADER __ImageBase;
#define HINST_THISCOMPONENT ((HINSTANCE)&__ImageBase)
#endif

#import "MSXML6.dll" rename_namespace(_T("MSXML"))
#include <msxml6.h>

#undef min
#undef max

#include "logging/Logging.h"

// SafeAcquire
template <typename InterfaceType>
inline InterfaceType* SafeAcquire(InterfaceType* newObject)
{
    if (newObject != nullptr) {
#ifdef DEBUG_RESOURCE
        LOG(SEVERITY_LEVEL_INFO) << L"[RESOURCE] Acquire [" << std::hex << newObject << L"]" << std::dec;
#endif
        newObject->AddRef();
    }
    return newObject;
}

// SafeRelease
template<class Interface>
inline void SafeRelease(
    Interface **ppInterfaceToRelease
)
{
    if (*ppInterfaceToRelease != nullptr) {
#ifdef DEBUG_RESOURCE
        LOG(SEVERITY_LEVEL_INFO) << L"[RESOURCE] Release [" << std::hex << *ppInterfaceToRelease << L"]" << std::dec;
#endif
        (*ppInterfaceToRelease)->Release();
        (*ppInterfaceToRelease) = nullptr;
    }
}

#pragma warning(disable: 4714)
#pragma warning(disable: 4503)

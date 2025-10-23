#ifndef COMMON_H_
#define COMMON_H_

#define LOG_RESOURCE(level, resource)                                                                                  \
  LOG(level) << L"[RESOURCE] " << #resource << L"(" << typeid(resource).name() << L") = " << std::hex << resource      \
             << std::dec

#define CHK_FATAL_HRESULT(expression)                                                                                  \
  \
{                                                                                                                 \
    HRESULT __hr__ = expression;                                                                                       \
    if (FAILED(__hr__)) {                                                                                              \
      SPDLOG_CRITICAL(L"{}, hr = {:x}", expression, __hr__);                            \
      throw std::runtime_error(#expression);                                                                           \
    }                                                                                                                  \
  \
}

#define CHK_FATAL_BOOL(expression)                                                                                     \
  \
{                                                                                                                 \
    BOOL __b__ = expression;                                                                                           \
    if (__b__ == 0) {                                                                                                  \
      LOG(SEVERITY_LEVEL_FATAL) << #expression << L", last error = " << std::hex << GetLastError() << std::dec;        \
      throw std::runtime_error(#expression);                                                                           \
    }                                                                                                                  \
  \
}

#define CHK_ERROR_HRESULT(retValue, expression)                                                                        \
  retValue = expression;                                                                                               \
  if (FAILED(retValue)) {                                                                                              \
    SPDLOG_CRITICAL(L"{}, hr = {:x}", expression, retValue);                        \
  }

#define CHK_ERROR_BOOL(retValue, expression)                                                                           \
  retValue = expression;                                                                                               \
  if (retValue == 0) {                                                                                                 \
    SPDLOG_CRITICAL(L"{}, last error = {}", expression, GetLastError());                                                   \
    throw std::runtime_error(#expression);                                                                             \
  }

#define CHK_WARN_HRESULT(expression)                                                                                   \
  \
{                                                                                                                 \
    HRESULT __hr__ = expression;                                                                                       \
    if (FAILED(__hr__)) {                                                                                              \
      SPDLOG_WARN(L"{}, hr = {:x}", expression, __hr__);                         \
    }                                                                                                                  \
  \
}

#define CHK_WARN_BOOL(expression)                                                                                      \
  \
{                                                                                                                 \
    BOOL __b__ = expression;                                                                                           \
    if (__b__ == 0) {                                                                                                  \
      SPDLOG_WARN(L"{}, last error = {:x}", expression, GetLastError());         \
    }                                                                                                                  \
  \
}

#include "../GameOfLife3D/Resource.h"
#include "../GameOfLife3D_en_us/ribbonres.h"
#include "../GameOfLife3D_en_us/Resource.h"

#define _LOG_FOLDERNAME L"\\GameOfLife3D"
#define _LOG_FILENAME L"\\GameOfLife3D.log"

#define _WINDOW_TITLE L"GameOfLife3D"

#define WM_RIBBON_RESIZED WM_USER + 1
#define WM_D2DERR_RECREATE_TARGET WM_USER + 2

#include "../GameOfLife3D/Resource.h"

#endif // COMMON_H_
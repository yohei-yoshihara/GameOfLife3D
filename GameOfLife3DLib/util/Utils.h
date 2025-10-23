#ifndef UTIL_H_
#define UTIL_H_

namespace util {
  /*
class WMessageArgs
{
public:
  WMessageArgs() :
      hwnd(0), message(0), wParam(0), lParam(0) {}
  WMessageArgs(HWND _hwnd, UINT _message, WPARAM _wParam, LPARAM _lParam) :
      hwnd(_hwnd), message(_message), wParam(_wParam), lParam(_lParam)
  {}
  HWND hwnd;
  UINT message;
  WPARAM wParam;
  LPARAM lParam;
};

extern void _WMessage(std::ios_base& os, util::WMessageArgs message);

inline std::_Smanip<WMessageArgs> __cdecl WMessage(util::WMessageArgs message)
{
  return (std::_Smanip<util::WMessageArgs>(&_WMessage, message));
}


class LastErrorArgs
{
public:
  LastErrorArgs() : functionName(), lastError(0) {}
  LastErrorArgs(const std::wstring &_functionName, DWORD _lastError) :
      functionName(_functionName), lastError(_lastError) {}
  std::wstring functionName;
  DWORD lastError;
};

extern void _LastError(std::ios_base& os, LastErrorArgs args);

inline std::_Smanip<LastErrorArgs> __cdecl LastError(LastErrorArgs args)
{
  return (std::_Smanip<LastErrorArgs>(&_LastError, args));
}
*/
  inline size_t ToBlockSize(size_t original, size_t blockSize) {
    size_t result = blockSize;
    if (0 != original) {
      size_t remainder = original % blockSize;
      result = original + (0 != remainder ? blockSize - remainder : 0);
    }
    return result;
  }
}

#endif // UTIL_H_

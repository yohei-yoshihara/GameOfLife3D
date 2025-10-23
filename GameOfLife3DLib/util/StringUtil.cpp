#include "StdAfx.h"
#include "logging/Logging.h"
#include "util/StringUtil.h"

void util::StringUtil::Tokenize(IN const std::wstring &str, IN const std::wstring &delimiters,
                                OUT std::vector<std::wstring> &tokens) {
  std::wstring::size_type delimPos = 0, tokenPos = 0, pos = 0;

  if (str.length() < 1) {
    return;
  }
  while (true) {
    delimPos = str.find_first_of(delimiters, pos);
    tokenPos = str.find_first_not_of(delimiters, pos);

    if (std::wstring::npos != delimPos) {
      if (std::wstring::npos != tokenPos) {
        if (tokenPos < delimPos) {
          tokens.push_back(str.substr(pos, delimPos - pos));
        } else {
          tokens.push_back(L"");
        }
      } else {
        tokens.push_back(L"");
      }
      pos = delimPos + 1;
    } else {
      if (std::wstring::npos != tokenPos) {
        tokens.push_back(str.substr(pos));
      } else {
        tokens.push_back(L"");
      }
      break;
    }
  }
}

void util::StringUtil::RemoveSpaces(IN const std::wstring &inStr, OUT std::wstring &outStr) {
  std::wstringstream str;
  for (size_t i = 0; i < inStr.length(); ++i) {
    wchar_t c = inStr[i];
    if (c == L' ' || c == L'\t' || c == L'\r' || c == L'\n') {
      continue;
    }
    str << c;
  }
  outStr = str.str();
}

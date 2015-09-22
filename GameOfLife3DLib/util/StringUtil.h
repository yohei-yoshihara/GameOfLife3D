#ifndef UTIL_STRINGUTIL_H_
#define UTIL_STRINGUTIL_H_

namespace util {
  class StringUtil {
  private:
    StringUtil(void);
    ~StringUtil(void);

  public:
    static void Tokenize(IN const std::wstring &str, IN const std::wstring &delimiters,
                         OUT std::vector<std::wstring> &tokens);
    static void RemoveSpaces(IN const std::wstring &inStr, OUT std::wstring &outStr);
  };
}

#endif // UTIL_STRINGUTIL_H_

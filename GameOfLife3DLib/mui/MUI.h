#ifndef MUI_MUI_H_
#define MUI_MUI_H_

namespace mui {

  class MUI {
  private:
    MUI(void);
    ~MUI(void);
    static HMODULE s_hModule;

  public:
    static HRESULT Initialize(const std::wstring &dllName);
    static HMODULE GetHModule() {
      return s_hModule;
    }
    static const wchar_t *GetString(UINT uID);
  };
}

#endif // MUI_MUI_H_
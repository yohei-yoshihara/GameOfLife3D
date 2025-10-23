#ifndef GAMEOFLIFE3D_IO_LIFEFILE_H_
#define GAMEOFLIFE3D_IO_LIFEFILE_H_

#include "gameOfLife3D/lifeSimulator/LifeData.h"

namespace gameOfLife3D {
  namespace io {
    enum LIFEFILE_RESULT {
      LIFEFILE_RESULT_OK,
      LIFEFILE_RESULT_FILENOTFOUND,
      LIFEFILE_RESULT_ILLEGALFORMAT,
    };
    enum LIFEFILE_FORMAT {
      LIFEFILE_FORMAT_LIFE106,
      LIFEFILE_FORMAT_LIFE3D100,
    };

    class LifeFile {
    private:
      const static wchar_t ALIVE_CHARS[];
      UINT m_width;
      UINT m_height;
      std::vector<gameOfLife3D::lifeSimulator::LifeData> m_data;

    protected:
      bool CheckAlive(wchar_t c);

    public:
      LifeFile();
      virtual ~LifeFile();

      LIFEFILE_RESULT Load(const std::wstring &fileName);
      LIFEFILE_RESULT Save(const std::wstring &fileName, LIFEFILE_FORMAT format);

      LIFEFILE_RESULT Load_Default(std::wistream &ifs);
      LIFEFILE_RESULT Load_Life106(std::wistream &ifs);
      LIFEFILE_RESULT Save_Life106(std::wostream &ofs);
      LIFEFILE_RESULT Load_Life3D100(std::wistream &ifs);
      LIFEFILE_RESULT Save_Life3D100(std::wostream &ofs);

      void SetWidth(UINT width) {
        m_width = width;
      }
      UINT GetWidth() const {
        return m_width;
      }
      void SetHeight(UINT height) {
        m_height = height;
      }
      UINT GetHeight() const {
        return m_height;
      }
      void SetLifeData(UINT x, UINT y, const gameOfLife3D::lifeSimulator::LifeData &lifeData) {
        m_data[x + y * m_width] = lifeData;
      }
      const gameOfLife3D::lifeSimulator::LifeData &GetLifeData(UINT x, UINT y) const {
        return m_data[x + y * m_width];
      }
    };
  }
}

#endif // GAMEOFLIFE3D_IO_LIFEFILE_H_
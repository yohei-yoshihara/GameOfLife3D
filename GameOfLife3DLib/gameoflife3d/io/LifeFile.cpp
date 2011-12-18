#include "stdafx.h"
#include "Common.h"
#include "gameOfLife3D//io/LifeFile.h"

const wchar_t gameOfLife3D::io::LifeFile::ALIVE_CHARS[] = {
    L'*',
    L'O',
    L'o',
    L'X',
    L'x',
    L'1',
};

bool gameOfLife3D::io::LifeFile::CheckAlive( wchar_t c )
{
    for (size_t i = 0; i < ARRAYSIZE(ALIVE_CHARS); ++i) {
        if (c == ALIVE_CHARS[i]) {
            return true;
        }
    }
    return false;
}

gameOfLife3D::io::LifeFile::LifeFile() :
    m_width(0),
    m_height(0),
    m_data()
{
}

gameOfLife3D::io::LifeFile::~LifeFile()
{
}

gameOfLife3D::io::LIFEFILE_RESULT gameOfLife3D::io::LifeFile::Load(const std::wstring &fileName)
{
    std::wifstream ifs(fileName);
    std::wstring line;
    std::getline(ifs, line);
    if (ifs.eof() || ifs.fail() || ifs.bad()) {
        LOG(SEVERITY_LEVEL_ERROR) << L"could not read the file '" << fileName << L"'";
        return LIFEFILE_RESULT_ILLEGALFORMAT;
    }
    LIFEFILE_RESULT result;
    if (line == L"#Life 1.06") {
        result = Load_Life106(ifs);
    } else if (line == L"#GameOfLife3D 1.00") {
        result = Load_Life3D100(ifs);
    } else {
        result = Load_Default(ifs);
    }
    ifs.close();
    return LIFEFILE_RESULT_OK;
}

gameOfLife3D::io::LIFEFILE_RESULT gameOfLife3D::io::LifeFile::Save(
    const std::wstring &fileName,
    LIFEFILE_FORMAT format)
{
    std::wofstream ofs(fileName);
    LIFEFILE_RESULT result;
    switch (format) {
    case LIFEFILE_FORMAT_LIFE106:
        result = Save_Life106(ofs);
        break;
    case LIFEFILE_FORMAT_LIFE3D100:
        result = Save_Life3D100(ofs);
        break;
    }
    ofs.close();
    return LIFEFILE_RESULT_OK;
}

gameOfLife3D::io::LIFEFILE_RESULT gameOfLife3D::io::LifeFile::Load_Default(std::wistream &ifs)
{
    std::vector<std::wstring> lines;
    std::wstring line;
    size_t max = 0;
    while (!ifs.eof()) {
        std::getline(ifs, line);
        if (line.length() == 0 || line[0] == L'#' || line[0] == L'!') {
            continue;
        }
        max = std::max(line.length(), max);
        lines.push_back(line);
    }

    m_width  = static_cast<UINT>(max);
    m_height = static_cast<UINT>(lines.size());
    m_data.resize(m_width * m_height);

    for (UINT y = 0; y < m_height; ++y) {
        for (UINT x = 0; x < m_width; ++x) {
            const std::wstring &line = lines[y];
            gameOfLife3D::lifeSimulator::LifeData &lifeData = m_data[x + y * m_width];
            if (x < line.length() && CheckAlive(line[x])) {
                lifeData.alive = 1;
                lifeData.color = XMFLOAT3(1.0f, 1.0f, 1.0f);
            } else {
                lifeData.alive = 0;
                lifeData.color = XMFLOAT3(0.0f, 0.0f, 0.0f);
            }
        }
    }

    return LIFEFILE_RESULT_OK;
}

struct Pos {
    int x;
    int y;
    int r;
    int g;
    int b;
};

gameOfLife3D::io::LIFEFILE_RESULT gameOfLife3D::io::LifeFile::Load_Life106(std::wistream & ifs)
{
    std::vector<Pos> posArray;
    std::wstring line;
    int maxX = 0;
    int maxY = 0;
    while (!ifs.eof()) {
        std::getline(ifs, line);
        if (line.length() == 0 || line[0] == L'#' || line[0] == L'!') {
            continue;
        }
        std::wistringstream sstr(line);
        Pos pos;
        sstr >> pos.x >> pos.y;
        maxX = std::max(maxX, pos.x);
        maxY = std::max(maxY, pos.y);
        posArray.push_back(pos);
    }

    m_width = maxX + 1;
    m_height = maxY + 1;
    m_data.resize(m_width * m_height);

    for (auto it = posArray.begin(); it != posArray.end(); ++it) {
        Pos &pos = *it;
        gameOfLife3D::lifeSimulator::LifeData &lifeData = m_data[pos.x + pos.y * m_width];
        lifeData.alive = 1;
        lifeData.color = XMFLOAT3(1.0f, 1.0f, 1.0f);
    }

    return LIFEFILE_RESULT_OK;
}

gameOfLife3D::io::LIFEFILE_RESULT gameOfLife3D::io::LifeFile::Load_Life3D100(std::wistream & ifs)
{
    std::vector<Pos> posArray;
    std::wstring line;
    int maxX = 0;
    int maxY = 0;
    while (!ifs.eof()) {
        std::getline(ifs, line);
        if (line.length() == 0 || line[0] == L'#' || line[0] == L'!') {
            continue;
        }
        std::wistringstream sstr(line);
        Pos pos;
        sstr >> pos.x >> pos.y >> pos.r >> pos.g >> pos.b;
        maxX = std::max(maxX, pos.x);
        maxY = std::max(maxY, pos.y);
        posArray.push_back(pos);
    }

    m_width = maxX;
    m_height = maxY;
    m_data.resize(m_width * m_height);

    for (auto it = posArray.begin(); it != posArray.end(); ++it) {
        Pos &pos = *it;
        gameOfLife3D::lifeSimulator::LifeData &lifeData = m_data[pos.x + pos.y * m_width];
        lifeData.alive = 1;
        lifeData.color = XMFLOAT3(pos.r / 255.f, pos.g / 255.f, pos.b / 255.f);
    }

    return LIFEFILE_RESULT_OK;
}

gameOfLife3D::io::LIFEFILE_RESULT gameOfLife3D::io::LifeFile::Save_Life106(std::wostream & ofs)
{
    ofs << L"#Life 1.06" << std::endl;
    for (UINT y = 0; y < m_height; ++y) {
        for (UINT x = 0; x < m_width; ++x) {
            gameOfLife3D::lifeSimulator::LifeData &lifeData = m_data[x + y * m_width];
            if (lifeData.alive != 0) {
                ofs << x << L" " << y << std::endl;
            }
        }
    }
    return LIFEFILE_RESULT_OK;
}

gameOfLife3D::io::LIFEFILE_RESULT gameOfLife3D::io::LifeFile::Save_Life3D100(std::wostream & ofs)
{
    ofs << L"#GameOfLife3D 1.00" << std::endl;
    for (UINT y = 0; y < m_height; ++y) {
        for (UINT x = 0; x < m_width; ++x) {
            gameOfLife3D::lifeSimulator::LifeData &lifeData = m_data[x + y * m_width];
            if (lifeData.alive != 0) {
                int r = static_cast<int>(lifeData.color.x * 255.f);
                int g = static_cast<int>(lifeData.color.y * 255.f);
                int b = static_cast<int>(lifeData.color.z * 255.f);
                ofs << x << L" " << y << L" " << r << L" " << g << L" " << b << std::endl;
            }
        }
    }
    return LIFEFILE_RESULT_OK;
}

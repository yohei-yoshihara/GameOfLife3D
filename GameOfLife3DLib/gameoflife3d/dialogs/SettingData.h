#ifndef GAMEOFLIFE3D_DIALOG_SETTINGDATA_H_
#define GAMEOFLIFE3D_DIALOG_SETTINGDATA_H_

namespace gameOfLife3D
{
namespace dialogs
{
enum VERTEX_GENERATION_METHOD {
    VERTEX_GENERATION_METHOD_CPU = 0,
    VERTEX_GENERATION_METHOD_GPU = 1
};

class SettingData
{
public:
    SettingData() :
        width(24),
        height(24),
        generations(24),
        updateInterval(1000),
        vertexGenerationMethod(VERTEX_GENERATION_METHOD_GPU)
    {}
    SettingData(const SettingData& settingData) :
        width(settingData.width),
        height(settingData.height),
        generations(settingData.generations),
        updateInterval(settingData.updateInterval),
        vertexGenerationMethod(settingData.vertexGenerationMethod)
    {}
    SettingData& operator=(const SettingData& settingData) {
        if (this != &settingData) {
            width = settingData.width;
            height = settingData.height;
            generations = settingData.generations;
            updateInterval = settingData.updateInterval;
            vertexGenerationMethod = settingData.vertexGenerationMethod;
        }
        return *this;
    }
    ~SettingData() {}
    bool operator==(const SettingData& settingData) const {
        return width == settingData.width &&
               height == settingData.height &&
               generations == settingData.generations &&
               updateInterval == settingData.updateInterval &&
               vertexGenerationMethod == settingData.vertexGenerationMethod;
    }
    bool operator!=(const SettingData& other) const {
        return !(*this == other);
    }

    UINT width;
    UINT height;
    UINT generations;
    UINT updateInterval;
    VERTEX_GENERATION_METHOD vertexGenerationMethod;
};
}
}

#endif // GAMEOFLIFE3D_DIALOG_SETTINGDATA_H_
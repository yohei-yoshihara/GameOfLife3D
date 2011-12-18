#ifndef GAMEOFLIFE3D_LIFESIMULATORFACTORY_H_
#define GAMEOFLIFE3D_LIFESIMULATORFACTORY_H_

#include "gameOfLife3D/lifeSimulator/AbstractLifeSimulator.h"

namespace gameOfLife3D
{
namespace lifeSimulator
{
enum SIMULATOR_TYPE {
    SIMULATOR_TYPE_CPU,
    SIMULATOR_TYPE_COMPUTESHADER,
};

class LifeSimulatorFactory
{
public:
    static std::shared_ptr<gameOfLife3D::lifeSimulator::AbstractLifeSimulator> Create(SIMULATOR_TYPE simulatorType);
};
}
}

#endif // GAMEOFLIFE3D_LIFESIMULATORFACTORY_H_

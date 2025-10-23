#include "stdafx.h"
#include "Common.h"
#include "gameOfLife3D/lifeSimulator/LifeSimulatorCS.h"
#include "gameOfLife3D/lifeSimulator/LifeSimulatorCPU.h"
#include "gameOfLife3D/lifeSimulator/LifeSimulatorFactory.h"

std::shared_ptr<gameOfLife3D::lifeSimulator::AbstractLifeSimulator>
gameOfLife3D::lifeSimulator::LifeSimulatorFactory::Create(gameOfLife3D::lifeSimulator::SIMULATOR_TYPE simulatorType) {
  std::shared_ptr<gameOfLife3D::lifeSimulator::AbstractLifeSimulator> simulator;
  switch (simulatorType) {
  case SIMULATOR_TYPE_COMPUTESHADER:
    simulator = std::make_shared<gameOfLife3D::lifeSimulator::LifeSimulatorCS>();
    break;
  case SIMULATOR_TYPE_CPU:
    simulator = std::make_shared<gameOfLife3D::lifeSimulator::LifeSimulatorCPU>();
    break;
  }
  return simulator;
}

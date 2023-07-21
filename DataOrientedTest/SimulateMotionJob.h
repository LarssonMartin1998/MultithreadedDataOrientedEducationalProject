#pragma once

#include <thread>
#include <array>

#include "Entity.h"

namespace SimulateMotionJob
{
    std::thread* Run(std::array<Entity::Position, Entity::numEntities>& positions, std::array<Entity::Velocity, Entity::numEntities>& velocities, std::array<Entity::Physics, Entity::numEntities>& physics, const float deltaTime);
}

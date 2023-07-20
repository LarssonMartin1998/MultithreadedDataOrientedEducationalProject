#pragma once

#include <array>

#include "Entity.h"

namespace SimulateMotionJob
{
    void Run(std::array<Entity::Position, Entity::numEntities>& positions, std::array<Entity::Velocity, Entity::numEntities>& velocities, std::array<Entity::Physics, Entity::numEntities>& physics);
}

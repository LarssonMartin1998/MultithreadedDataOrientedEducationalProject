#include "SimulateMotionJob.h"

#include <iostream>
#include <algorithm>

namespace SimulateMotionJob
{
    constexpr float gravity = 9.82f;
    constexpr unsigned simTimeSeconds = 10;

    inline void UpdateMotion(std::array<Entity::Position, Entity::numEntities>& positions, std::array<Entity::Velocity, Entity::numEntities>& velocities, std::array<Entity::Physics, Entity::numEntities>& physics, const float deltaTime)
    {
        for (size_t i = 0; i < velocities.size(); i++)
        {
            physics[i].acceleration = physics[i].acceleration - gravity * deltaTime;
            velocities[i].speed = std::clamp(velocities[i].speed + physics[i].acceleration * deltaTime, 0.0f, std::numeric_limits<float>::max());

            positions[i].pos += velocities[i].direction * velocities[i].speed * deltaTime;
        }
    }

    std::thread* Run(std::array<Entity::Position, Entity::numEntities>& positions, std::array<Entity::Velocity, Entity::numEntities>& velocities, std::array<Entity::Physics, Entity::numEntities>& physics, const float deltaTime)
    {
        return new std::thread(&UpdateMotion, std::ref(positions), std::ref(velocities), std::ref(physics), deltaTime);
    }
}

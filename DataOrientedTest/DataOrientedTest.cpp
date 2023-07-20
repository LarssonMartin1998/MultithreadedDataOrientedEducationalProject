#include <iostream>
#include <format>
#include <array>

#include "Entity.h"
#include "RandomizeJob.h"
#include "SimulateMotionJob.h"

void PrintEntities(const std::array<Entity::Position, Entity::numEntities>& positions, const std::array<Entity::Velocity, Entity::numEntities>& velocities, const std::array<Entity::Physics, Entity::numEntities>& physics)
{
    for (size_t i = 0; i < positions.size(); i++)
    {
        std::cout << std::format("\nEntity: {}", i);
        std::cout << std::format("\nPos: x={}, y={}", positions[i].pos.x, positions[i].pos.y);
        std::cout << std::format("\nVelocity: x={}, y={}, speed={}", velocities[i].direction.x, velocities[i].direction.y, velocities[i].speed);
        std::cout << std::format("\nPhysics: mass={}, acceleration={}", physics[i].mass, physics[i].acceleration);
        std::cout << std::format("\n----------------------------");
    }
}

int main()
{
    std::array<Entity::Position, Entity::numEntities> positions {};
    std::array<Entity::Velocity, Entity::numEntities> velocities {};
    std::array<Entity::Physics, Entity::numEntities> physics {};

    RandomizeJob::Run(positions, velocities, physics);
    PrintEntities(positions, velocities, physics);
    SimulateMotionJob::Run(positions, velocities, physics);
    PrintEntities(positions, velocities, physics);

    return 0;
}

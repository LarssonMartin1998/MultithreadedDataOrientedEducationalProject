#include <cstdio>
#include <array>

#include "Entity.h"
#include "RandomizeJob.h"
#include "SimulateMotionJob.h"

void PrintEntities(const std::array<Entity::Position, Entity::numEntities>& positions, const std::array<Entity::Velocity, Entity::numEntities>& velocities, const std::array<Entity::Physics, Entity::numEntities>& physics)
{
    for (size_t i = 0; i < positions.size(); i++)
    {
        printf("\nEntity: %zu", i);
        printf("\nPos: x=%f, y=%f", positions[i].pos.x, positions[i].pos.y);
        printf("\nEntity::Velocity: x=%f, y=%f, speed=%f", velocities[i].direction.x, velocities[i].direction.y, velocities[i].speed);
        printf("\nEntity::Physics: mass=%f, acceleration=%f", physics[i].mass, physics[i].acceleration);
        printf("\n----------------------------");
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

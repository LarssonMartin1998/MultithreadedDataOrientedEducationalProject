#include <iostream>
#include <array>

#include "Entity.h"
#include "RandomizeJob.h"
#include "SimulateMotionJob.h"

enum class ApplicationSettings : long unsigned int
{
    NUM_ENTITIES = 500
};

template<std::size_t SIZE>
void PrintEntities(std::array<Position, SIZE>& positions, std::array<Velocity, SIZE>& velocities, std::array<Physics, SIZE>& physics)
{
    for (size_t i = 0; i < positions.size(); i++)
    {
        printf("\nEntity: %zu", i);
        printf("\nPos: x=%f, y=%f", positions[i].pos.x, positions[i].pos.y);
        printf("\nVelocity: x=%f, y=%f, speed=%f", velocities[i].direction.x, velocities[i].direction.y, velocities[i].speed);
        printf("\nPhysics: mass=%f, acceleration=%f", physics[i].mass, physics[i].acceleration);
        printf("\n----------------------------");
    }
}

int main(int argc, char* argv[])
{
    constexpr size_t numEntities = static_cast<size_t>(ApplicationSettings::NUM_ENTITIES);
    std::array<Position, numEntities> positions;
    std::array<Velocity, numEntities> velocities;
    std::array<Physics, numEntities> physics;

    RandomizeJob::Run(positions, velocities, physics);
    PrintEntities(positions, velocities, physics);
    SimulateMotionJob::Run(positions, velocities, physics);
    PrintEntities(positions, velocities, physics);

    return 0;
}

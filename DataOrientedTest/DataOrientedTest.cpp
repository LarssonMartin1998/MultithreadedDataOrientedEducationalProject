#include <iostream>
#include <thread>
#include <array>
#include <chrono>

#include "Vector.h"
#include "Entity.h"
#include "RandomizeJob.h"
#include "SimulateMotionJob.h"
#include "RenderJob.h"

int main()
{
    std::array<Entity::Position, Entity::numEntities> positions {};
    std::array<Entity::Velocity, Entity::numEntities> velocities {};
    std::array<Entity::Physics, Entity::numEntities> physics {};

    RandomizeJob::Run(positions, velocities, physics);
    RenderJob renderJob(velocities, physics);

    constexpr float simTimeSeconds = 3.5f;
    const std::chrono::high_resolution_clock::time_point start = std::chrono::high_resolution_clock::now();
    std::chrono::high_resolution_clock::time_point last = start;
    std::chrono::duration<float> totalTime = last - start;
    while (totalTime.count() < simTimeSeconds)
    {
        const std::chrono::high_resolution_clock::time_point current = std::chrono::high_resolution_clock::now();
        totalTime = current - start;
        const float deltaTime = static_cast<std::chrono::duration<float>>(current - last).count();

        std::array<Entity::Position, Entity::numEntities> lastPositions = positions;
        std::thread* renderThread = renderJob.Run(lastPositions);
        std::thread* simulateMotionThread = SimulateMotionJob::Run(positions, velocities, physics, deltaTime);

        renderThread->join();
        simulateMotionThread->join();

        delete renderThread;
        delete simulateMotionThread;

        last = current;
    }
    
    return 0;
}

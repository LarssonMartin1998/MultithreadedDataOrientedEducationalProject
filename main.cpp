#include <iostream>
#include <thread>
#include <array>
#include <chrono>

#include "Vector.h"
#include "Entity.h"
#include "RandomizeJob.h"
#include "SimulateMotionJob.h"
#include "RenderJob.h"

#define RUN_ASYNC

int main()
{
    std::array<Entity::Position, Entity::numEntities> positions {};
    std::array<Entity::Velocity, Entity::numEntities> velocities {};
    std::array<Entity::Physics, Entity::numEntities> physics {};

    RandomizeJob::Run(positions, velocities, physics);
    RenderJob renderJob(velocities);

    size_t numFrames = 0;
    constexpr float simTimeSeconds = 2.0f;
    const std::chrono::high_resolution_clock::time_point start = std::chrono::high_resolution_clock::now();
    std::chrono::high_resolution_clock::time_point last = start;
    std::chrono::duration<float> totalTime = last - start;
    while (totalTime.count() < simTimeSeconds)
    {
        const std::chrono::high_resolution_clock::time_point current = std::chrono::high_resolution_clock::now();
        totalTime = current - start;
        const float deltaTime = static_cast<std::chrono::duration<float>>(current - last).count();

        std::array<Entity::Position, Entity::numEntities> lastPositions = positions;
#ifdef RUN_ASYNC
        std::thread* renderThread = renderJob.Run(lastPositions);
        std::thread* simulateMotionThread = SimulateMotionJob::Run(positions, velocities, physics, deltaTime);

        renderThread->join();
        simulateMotionThread->join();
#else
        std::thread* renderThread = renderJob.Run(lastPositions);
        renderThread->join();

        std::thread* simulateMotionThread = SimulateMotionJob::Run(positions, velocities, physics, deltaTime);
        simulateMotionThread->join();
#endif

        delete renderThread;
        delete simulateMotionThread;

        last = current;
        numFrames++;
    }

    RenderJob::ShutDownConsole();

    if (system("clear") == -1)
    {
        return 0;
    }

    const float fps = static_cast<float>(numFrames) / simTimeSeconds;
    const float frameTime = simTimeSeconds * 1000.0f / static_cast<float>(numFrames);
    std::cout << "Num frames: " << numFrames << ", FPS: " << fps << ", Frame Time: " << frameTime << std::endl;

    return 0;
}

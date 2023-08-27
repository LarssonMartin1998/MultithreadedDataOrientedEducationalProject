#include <iostream>
#include <thread>
#include <array>

#include "Vector.h"
#include "Clocks.h"
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
    constexpr float simTimeSeconds = 10.0f;

    Clocks::StartAppClock();
    while (Clocks::GetTotalTime() < simTimeSeconds)
    {
        Clocks::Update();
        std::array<Entity::Position, Entity::numEntities> lastPositions = positions;

#ifdef RUN_ASYNC
        std::thread* renderThread = renderJob.Run(lastPositions);
        std::thread* simulateMotionThread = SimulateMotionJob::Run(positions, velocities, physics);

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

        Clocks::SavePreviousFrameClock();
        numFrames++;
    }

    RenderJob::ShutDownConsole();

    if (system("clear") == -1)
    {
        return 0;
    }

    const float fps = static_cast<float>(numFrames) / simTimeSeconds;
    const float frameTime = simTimeSeconds * 1000.0f / static_cast<float>(numFrames);
    std::cout << "Num frames: " << numFrames << ", Average FPS: " << fps << std::endl;
    std::cout << "Average Frame Time: " << frameTime << std::endl;
    std::cout << "Average Sim Thread Time: " << Clocks::GetSimTime() << std::endl;
    std::cout << "Average Render Thread Time: " << Clocks::GetRenderTime() << std::endl;

    return 0;
}

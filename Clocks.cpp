#include "Clocks.h"

#include <chrono>

namespace Clocks
{
    std::chrono::high_resolution_clock::time_point appStart;

    std::chrono::high_resolution_clock::time_point currentFrame;
    std::chrono::duration<float> totalFrame;
    std::chrono::high_resolution_clock::time_point lastFrame;

    std::chrono::high_resolution_clock::time_point currentSimThread;
    std::chrono::duration<float> totalSimThread;

    std::chrono::high_resolution_clock::time_point currentRenderThread;
    std::chrono::duration<float> totalRenderThread;

    float deltaTime;

    void StartAppClock()
    {
        appStart = std::chrono::high_resolution_clock::now();
        currentFrame = appStart;
        totalFrame = lastFrame - appStart;
    }

    void Update()
    {
        currentFrame = std::chrono::high_resolution_clock::now();
        totalFrame = currentFrame - appStart;
        deltaTime = static_cast<std::chrono::duration<float>>(currentFrame - lastFrame).count();
    }

    void SavePreviousFrameClock()
    {
        lastFrame = currentFrame;
    }

    float GetTotalTime()
    {
        return totalFrame.count();
    }

    float GetDeltaTime()
    {
        return deltaTime;
    }

    void StartSimClock()
    {
        currentSimThread = std::chrono::high_resolution_clock::now();
    }

    void PauseSimClock()
    {
        const std::chrono::high_resolution_clock::time_point now = std::chrono::high_resolution_clock::now();
        totalSimThread += now - currentSimThread;
    }

    float GetSimTime()
    {
        return totalSimThread.count();
    }

    void StartRenderClock()
    {
        currentRenderThread = std::chrono::high_resolution_clock::now();
    }

    void PauseRenderClock()
    {
        const std::chrono::high_resolution_clock::time_point now = std::chrono::high_resolution_clock::now();
        totalRenderThread += now - currentRenderThread;
    }

    float GetRenderTime()
    {
        return totalRenderThread.count();
    }
}
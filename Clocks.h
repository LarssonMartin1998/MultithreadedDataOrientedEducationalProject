#pragma once

namespace Clocks
{
    void StartAppClock();
    void Update();
    void SavePreviousFrameClock();

    float GetTotalTime();
    float GetDeltaTime();

    void StartSimClock();
    void PauseSimClock();
    float GetSimTime();

    void StartRenderClock();
    void PauseRenderClock();
    float GetRenderTime();
};
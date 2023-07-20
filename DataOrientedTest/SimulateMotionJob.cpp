#include "SimulateMotionJob.h"

#include <iostream>
#include <thread>
#include <mutex>
#include <algorithm>

namespace SimulateMotionJob
{
    constexpr float gravity = 9.82f;
    constexpr unsigned simTimeSeconds = 10;

    void UpdateVelocities(std::array<Entity::Velocity, Entity::numEntities>& velocities, std::array<Entity::Physics, Entity::numEntities>& physics, std::mutex mutexes[], const float deltaTime)
    {
        for (size_t i = 0; i < velocities.size(); i++)
        {
            const size_t flipFlop = i % 2;
            std::lock_guard lock(mutexes[flipFlop]);
            physics[i].acceleration = physics[i].acceleration - gravity * deltaTime;
            velocities[i].speed = std::clamp(velocities[i].speed + physics[i].acceleration * deltaTime, 0.0f, std::numeric_limits<float>::max());
        }
    }
    
    void UpdatePositions(std::array<Entity::Position, Entity::numEntities>& positions, const std::array<Entity::Velocity, Entity::numEntities>& velocities, std::mutex mutexes[], const float deltaTime)
    {
        for (size_t i = 0; i < positions.size(); i++)
        {
            const size_t flipFlop = i % 2;
            std::lock_guard lock(mutexes[flipFlop]);
            positions[i].pos.x += velocities[i].direction.x * velocities[i].speed * deltaTime;
            positions[i].pos.y += velocities[i].direction.y * velocities[i].speed * deltaTime;
        }
    }

    void Run(std::array<Entity::Position, Entity::numEntities>& positions, std::array<Entity::Velocity, Entity::numEntities>& velocities, std::array<Entity::Physics, Entity::numEntities>& physics)
    {
        std::mutex mutexes[2];

        const std::chrono::high_resolution_clock::time_point start = std::chrono::high_resolution_clock::now();
        std::chrono::high_resolution_clock::time_point last = start;
        std::chrono::duration<float> totalTime = last - start;
        while (totalTime.count() < simTimeSeconds)
        {
            const std::chrono::high_resolution_clock::time_point current = std::chrono::high_resolution_clock::now();
            totalTime = current - start;
            const float deltaTime = static_cast<std::chrono::duration<float>>(current - last).count();

            const int lastSecondInt = static_cast<int>(static_cast<std::chrono::duration<float>>(last - start).count());
            const int currentSecondInt = static_cast<int>(totalTime.count());
            if (lastSecondInt != currentSecondInt)
            {
                std::cout << std::endl << currentSecondInt;
            }
            
            std::thread velocitiesThread = std::thread(&UpdateVelocities, std::ref(velocities), std::ref(physics), std::ref(mutexes), deltaTime);
            std::thread positionsThread = std::thread(&UpdatePositions, std::ref(positions), std::ref(velocities), std::ref(mutexes), deltaTime);

            velocitiesThread.join();
            positionsThread.join();
            
            last = current;
        }
    }
}

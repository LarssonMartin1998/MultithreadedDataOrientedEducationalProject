#include "RandomizeJob.h"

#include <thread>
#include <mutex>
#include <random>

namespace RandomizeJob
{
    std::mt19937 GetRandomGenerator()
    {
        std::random_device randomDevice;
        return std::mt19937(randomDevice());
    }

    template<std::size_t SIZE>
    void RandomizePositions(std::array<Position, SIZE>& positions)
    {
        std::mt19937 generator = GetRandomGenerator();
        std::uniform_real_distribution<float> range(-50.0f, 50.0f);
    
        for (size_t i = 0; i < positions.size(); i++)
        {
            positions[i].pos.x = range(generator);       
            positions[i].pos.y = range(generator);       
        }
    }

    template<std::size_t SIZE>
    void RandomizeVelocities(std::array<Velocity, SIZE>& velocities)
    {
        std::mt19937 generator = GetRandomGenerator();
        std::uniform_real_distribution<float> speedRange(0.0f, 80.0f);
        std::uniform_real_distribution<float> directionRange(-1.0f, 1.0f);
    
        for (size_t i = 0; i < velocities.size(); i++)
        {
            velocities[i].speed = speedRange(generator);
            
            const float randomDirX = directionRange(generator);       
            const float randomDirY = directionRange(generator);
            const float magnitude = std::sqrt(randomDirX * randomDirX + randomDirY * randomDirY);
            velocities[i].direction.x = randomDirX / magnitude;
            velocities[i].direction.y = randomDirY / magnitude;
        }
    }

    template<std::size_t SIZE>
    void RandomizePhysics(std::array<Physics, SIZE>& physics)
    {
        std::mt19937 generator = GetRandomGenerator();
        std::uniform_real_distribution<float> massRange(40.0f, 100.0f);
        std::uniform_real_distribution<float> accelerationRange(-8.0f, 8.0f);
    
        for (size_t i = 0; i < physics.size(); i++)
        {
            physics[i].mass = massRange(generator);
            physics[i].acceleration = accelerationRange(generator);
        }
    }

    template<std::size_t SIZE>
    void Run(std::array<Position, SIZE>& positions, std::array<Velocity, SIZE>& velocities, std::array<Physics, SIZE>& physics)
    {
        std::thread positionsThread = std::thread(&RandomizeJob::RandomizePositions, positions);
        std::thread velocitiesThread = std::thread(&RandomizeJob::RandomizeVelocities, velocities);
        std::thread physicsThread = std::thread(&RandomizeJob::RandomizePhysics, physics);
    
        positionsThread.join();
        velocitiesThread.join();
        physicsThread.join();
    }
}

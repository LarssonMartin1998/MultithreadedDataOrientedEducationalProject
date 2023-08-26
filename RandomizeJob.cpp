#include "RandomizeJob.h"

#include <thread>
#include <random>

namespace RandomizeJob
{
    std::mt19937 GetRandomGenerator()
    {
        std::random_device randomDevice;
        return std::mt19937(randomDevice());
    }
    
    void RandomizePositions(std::array<Entity::Position, Entity::numEntities>& positions)
    {
        std::mt19937 generator = GetRandomGenerator();
        std::uniform_real_distribution range(-20.0f, 20.0f);
    
        for (auto& [pos] : positions)
        {
            pos.x = range(generator);
            pos.y = range(generator);       
        }
    }

    void RandomizeVelocities(std::array<Entity::Velocity, Entity::numEntities>& velocities)
    {
        std::mt19937 generator = GetRandomGenerator();
        std::uniform_real_distribution speedRange(0.0f, 20.0f);
        std::uniform_real_distribution directionRange(-1.0f, 1.0f);
    
        for (auto& [speed, direction] : velocities)
        {
            speed = speedRange(generator);
            
            const float randomDirX = directionRange(generator);       
            const float randomDirY = directionRange(generator);
            const float magnitude = std::sqrt(randomDirX * randomDirX + randomDirY * randomDirY);
            direction.x = randomDirX / magnitude;
            direction.y = randomDirY / magnitude;
        }
    }

    void RandomizePhysics(std::array<Entity::Physics, Entity::numEntities>& physics)
    {
        std::mt19937 generator = GetRandomGenerator();
        std::uniform_real_distribution accelerationRange(-3.0f, 3.0f);
    
        for (auto& [acceleration] : physics)
        {
            acceleration = accelerationRange(generator);
        }
    }

    void Run(std::array<Entity::Position, Entity::numEntities>& positions, std::array<Entity::Velocity, Entity::numEntities>& velocities, std::array<Entity::Physics, Entity::numEntities>& physics)
    {
        std::thread positionsThread = std::thread(&RandomizePositions, std::ref(positions));
        std::thread velocitiesThread = std::thread(&RandomizeVelocities, std::ref(velocities));
        std::thread physicsThread = std::thread(&RandomizePhysics, std::ref(physics));
    
        positionsThread.join();
        velocitiesThread.join();
        physicsThread.join();
    }
}

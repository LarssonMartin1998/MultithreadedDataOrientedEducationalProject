#include <thread>
#include <random>
#include <cmath>
#include <mutex>
#include <algorithm>

#define GRAVITY 9.82f

#define SIM_TIME_SECONDS 10.0f
#define NUM_ENTITIES 5

struct Vector2
{
    float x;
    float y;
};

struct Position
{
    Vector2 pos;
};

struct Velocity
{
    float speed;
    Vector2 direction;
};

struct Physics
{
    float mass;
    float acceleration;
};

class RandomizeJob
{
public:
    typedef void (*RandomizeJobFinishedCallback)(Position outPositions[], Velocity outVelocities[], Physics outPhysics[]);
    RandomizeJob(Position positions[], Velocity velocities[], Physics physics[], RandomizeJobFinishedCallback onFinishedCallback)
    {
        positionsThread = std::thread(&RandomizeJob::RandomizePositions, positions);       
        velocitiesThread = std::thread(&RandomizeJob::RandomizeVelocities, velocities);
        physicsThread = std::thread(&RandomizeJob::RandomizePhysics, physics);

        positionsThread.join();
        velocitiesThread.join();
        physicsThread.join();

        onFinishedCallback(positions, velocities, physics);
    }
    
private:
    static std::mt19937 GetRandomGenerator();
    static void RandomizePositions(Position positions[]);
    static void RandomizeVelocities(Velocity velocities[]);
    static void RandomizePhysics(Physics physics[]);

private:
    std::thread positionsThread;
    std::thread velocitiesThread;
    std::thread physicsThread;
};

std::mt19937 RandomizeJob::GetRandomGenerator()
{
    std::random_device randomDevice;
    return std::mt19937(randomDevice());
}

void RandomizeJob::RandomizePositions(Position positions[])
{
    std::mt19937 generator = GetRandomGenerator();
    std::uniform_real_distribution<float> range(-50.0f, 50.0f);

    for (size_t i = 0; i < NUM_ENTITIES; i++)
    {
        positions[i].pos.x = range(generator);       
        positions[i].pos.y = range(generator);       
    }
}

void RandomizeJob::RandomizeVelocities(Velocity velocities[])
{
    std::mt19937 generator = GetRandomGenerator();
    std::uniform_real_distribution<float> speedRange(0.0f, 80.0f);
    std::uniform_real_distribution<float> directionRange(-1.0f, 1.0f);

    for (size_t i = 0; i < NUM_ENTITIES; i++)
    {
        velocities[i].speed = speedRange(generator);
        
        const float randomDirX = directionRange(generator);       
        const float randomDirY = directionRange(generator);
        const float magnitude = std::sqrt(randomDirX * randomDirX + randomDirY * randomDirY);
        velocities[i].direction.x = randomDirX / magnitude;
        velocities[i].direction.y = randomDirY / magnitude;
    }
}

void RandomizeJob::RandomizePhysics(Physics physics[])
{
    std::mt19937 generator = GetRandomGenerator();
    std::uniform_real_distribution<float> massRange(40.0f, 100.0f);
    std::uniform_real_distribution<float> accelerationRange(-8.0f, 8.0f);

    for (size_t i = 0; i < NUM_ENTITIES; i++)
    {
        physics[i].mass = massRange(generator);
        physics[i].acceleration = accelerationRange(generator);
    }
}

class SimulateMotionJob
{
public:
    typedef void (*SimulateMotionJobFinishedCallback)(Position resultPositions[], Velocity resultVelocities[], Physics resultPhysics[]);
    SimulateMotionJob(Position positions[], Velocity velocities[], Physics physics[], SimulateMotionJobFinishedCallback onFinishedCallback)
    {
        const std::chrono::high_resolution_clock::time_point start = std::chrono::high_resolution_clock::now();
        std::chrono::high_resolution_clock::time_point last = start;
        std::chrono::duration<float> totalTime = last - start;
        while (totalTime.count() < SIM_TIME_SECONDS)
        {
            const std::chrono::high_resolution_clock::time_point current = std::chrono::high_resolution_clock::now();
            totalTime = current - start;
            const float deltaTime = static_cast<std::chrono::duration<float>>(current - last).count();

            const int lastSecondInt = static_cast<int>(static_cast<std::chrono::duration<float>>(last - start).count());
            const int currentSecondInt = static_cast<int>(totalTime.count());
            if (lastSecondInt != currentSecondInt)
            {
                printf("\n%i", currentSecondInt);
            }
            
            velocitiesThread = std::thread(&SimulateMotionJob::UpdateVelocities, velocities, physics, deltaTime);
            positionsThread = std::thread(&SimulateMotionJob::UpdatePositions, positions, velocities, deltaTime);

            velocitiesThread.join();
            positionsThread.join();
            
            last = current;
        }

        onFinishedCallback(positions, velocities, physics);
    }

private:
    static void UpdateVelocities(Velocity velocities[], Physics physics[], const float deltaTime);
    static void UpdatePositions(Position positions[], Velocity velocities[], const float deltaTime);
    
private:
    static std::mutex mutexes[2];
    std::thread velocitiesThread;
    std::thread positionsThread;
};

std::mutex SimulateMotionJob::mutexes[2];

void SimulateMotionJob::UpdateVelocities(Velocity velocities[], Physics physics[], const float deltaTime)
{
    for (size_t i = 0; i < NUM_ENTITIES; i++)
    {
        const size_t flipFlop = i % 2;
        std::lock_guard<std::mutex> lock(mutexes[flipFlop]);
        physics[i].acceleration = physics[i].acceleration - GRAVITY * deltaTime;
        velocities[i].speed = std::clamp(velocities[i].speed + physics[i].acceleration * deltaTime, 0.0f, std::numeric_limits<float>::max());
    }
}

void SimulateMotionJob::UpdatePositions(Position positions[], Velocity velocities[], const float deltaTime)
{
    for (size_t i = 0; i < NUM_ENTITIES; i++)
    {
        const size_t flipFlop = i % 2;
        std::lock_guard<std::mutex> lock(mutexes[flipFlop]);
        positions[i].pos.x += velocities[i].direction.x * velocities[i].speed * deltaTime;
        positions[i].pos.y += velocities[i].direction.y * velocities[i].speed * deltaTime;
    }
}

void PrintEntities(Position positions[], Velocity velocities[], Physics physics[])
{
    for (size_t i = 0; i < NUM_ENTITIES; i++)
    {
        printf("\nEntity: %i", i);
        printf("\nPos: x=%f, y=%f", positions[i].pos.x, positions[i].pos.y);
        printf("\nVelocity: x=%f, y=%f, speed=%f", velocities[i].direction.x, velocities[i].direction.y, velocities[i].speed);
        printf("\nPhysics: mass=%f, acceleration=%f", physics[i].mass, physics[i].acceleration);
        printf("\n----------------------------");
    }
}
int main(int argc, char* argv[])
{
    Position positions[NUM_ENTITIES];
    Velocity velocities[NUM_ENTITIES];
    Physics physics[NUM_ENTITIES];

    RandomizeJob randomizeJob(positions, velocities, physics,
        [](Position resultPositions[], Velocity resultVelocities[], Physics resultPhysics[])
    {
        PrintEntities(resultPositions, resultVelocities, resultPhysics);
            
        SimulateMotionJob motionJob(resultPositions, resultVelocities, resultPhysics,
            [](Position resultPositions[], Velocity resultVelocities[], Physics resultPhysics[])
        {
            PrintEntities(resultPositions, resultVelocities, resultPhysics);
        });
    });
    
    return 0;
}

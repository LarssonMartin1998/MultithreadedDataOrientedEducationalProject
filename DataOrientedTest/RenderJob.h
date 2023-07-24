#include <thread>
#include <array>

#include "Entity.h"

struct DrawProperties
{
    char direction;
    bool isBold;
};

class RenderJob
{
public:
    RenderJob(const std::array<Entity::Velocity, Entity::numEntities>& velocities, const std::array<Entity::Physics, Entity::numEntities>& physics);
    ~RenderJob();
    std::thread* Run(const std::array<Entity::Position, Entity::numEntities>& positions);

private:
    static void InitializeConsole();
    void InitializeDrawProperties(const std::array<Entity::Velocity, Entity::numEntities>& velocities, const std::array<Entity::Physics, Entity::numEntities>& physics);

    void ShutDownConsole();

    inline void WriteToBackBuffer(const size_t x, const size_t y, const char character);
    void ClearBackBuffer();
    void SwapBuffers();

    char ConvertDirectionToCharacter(Vector2 direction);
    inline size_t GetCenterForAxis(const size_t axisSize);
    inline Vector2 GetConsoleCoordsFromWorldPos(const Vector2& position);

    void WriteWorld();
    void WriteHorizontal(const size_t row);
    void WriteVertical(const size_t column);
    void WriteXAxis();
    void WriteYAxis();
    void WriteOrigo();
    void WriteEntities(const std::array<Entity::Position, Entity::numEntities>& positions);

private:
    const static size_t worldWidth = 53;
    const static size_t consoleWidth = worldWidth * 2; // Double world Width to compensate line margin. Every other character is drawn as a blank to space it out.
    const static size_t worldHeight = 53;

    std::array<char, consoleWidth * worldHeight> clearBuffer;
    std::array<char, consoleWidth * worldHeight> drawBuffer;
    std::array<char, consoleWidth * worldHeight> backBuffer;

    std::array<DrawProperties, Entity::numEntities> drawProperties;
};


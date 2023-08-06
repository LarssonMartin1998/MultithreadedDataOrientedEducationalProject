#include <thread>
#include <array>

#include "Entity.h"

struct DrawProperties
{
    char direction;
};

class RenderJob
{
public:
    explicit RenderJob(const std::array<Entity::Velocity, Entity::numEntities>& velocities);
    std::thread* Run(const std::array<Entity::Position, Entity::numEntities>& positions);
    static void ShutDownConsole();

private:
    static void InitializeConsole();
    void InitializeDrawProperties(const std::array<Entity::Velocity, Entity::numEntities>& velocities);

    inline void WriteToBackBuffer(const size_t x, const size_t y, const char character);
    inline void WriteToClearBuffer(const size_t x, const size_t y, const char character);
    void FillClearBuffer();
    void ClearBackBuffer();
    void SwapBuffers();

    static char ConvertDirectionToCharacter(Vector2 direction);
    static inline size_t GetCenterForAxis(const size_t axisSize);
    static inline void GetConsoleCoordsFromWorldPos(const Vector2& position, size_t& outX, size_t& outY);

    void WriteWorld();
    void WriteXAxis();
    void WriteYAxis();
    void WriteOrigo();
    void WriteEntities(const std::array<Entity::Position, Entity::numEntities>& positions);

private:
    const static size_t worldWidth = 83;
    const static size_t consoleWidth = worldWidth * 2; // Double world Width to compensate line margin. Every other character is drawn as a blank to space it out.
    const static size_t worldHeight = 39;
    const static size_t bufferSize = consoleWidth * worldHeight;

    static float cachedWorldWidthCenter;
    static float cachedWorldHeightCenter;

    std::array<char, bufferSize> clearBuffer{};
    std::array<char, bufferSize> drawBuffer{};

    std::array<DrawProperties, Entity::numEntities> drawProperties{};

private:
    static inline void WriteToBuffer(std::array<char, bufferSize>& buffer, const size_t x, const size_t y, const char character);
    static void WriteHorizontal(std::array<char, bufferSize>& buffer, const size_t row);
    static void WriteVertical(std::array<char, bufferSize>& buffer, const size_t column);
};


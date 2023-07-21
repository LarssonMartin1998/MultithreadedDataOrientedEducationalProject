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
    std::thread* Run(const std::array<Entity::Position, Entity::numEntities>& positions);

private:
    void PrintWorld(const std::array<Entity::Position, Entity::numEntities>& position);
    char ConvertDirectionToCharacter(Vector2 direction);
    size_t GetLineCenterIndex(const size_t worldSize);
    void PrintHorizontal(const size_t worldSize);
    void PrintVerticalRow(const size_t worldSize, const char fill, const char center, const char spacing);
    void PrintVerticalRow(const size_t worldSize, const char fill, const char center, const char spacing, const size_t rowIndex, const std::array<Entity::Position, Entity::numEntities>& positions);
    Vector2 GetConsoleCoordsFromWorldPos(const size_t worldSize, const Vector2& position);
    bool IsWorldPosEqualToConsolePos(const size_t worldSize, const size_t column, const size_t row, const Vector2& position);

private:
    std::array<DrawProperties, Entity::numEntities> drawProperties;
};


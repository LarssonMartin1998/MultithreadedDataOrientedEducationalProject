#include "RenderJob.h"

#include <iostream>
#include <format>
#include <cmath>

#include "Vector.h"

enum class DirectionalCharacter
{
    TOP,
    RIGHT,
    BOTTOM,
    LEFT,

    NUM_DIRECTIONS
};

const std::array<Vector2, static_cast<size_t>(DirectionalCharacter::NUM_DIRECTIONS)> directions 
{
    Vector2(0.0f, 1.0f),
    Vector2(1.0f, 0.0f),
    Vector2(0.0f, -1.0f),
    Vector2(-1.0f, 0.0f)
};

const std::array<char, static_cast<size_t>(DirectionalCharacter::NUM_DIRECTIONS)> directionalCharacters
{
    '^',
    '>',
    'v',
    '<'
};

RenderJob::RenderJob(const std::array<Entity::Velocity, Entity::numEntities>& velocities, const std::array<Entity::Physics, Entity::numEntities>& physics)
{
    constexpr float massThreshold = 50.0f;
    for (size_t i = 0; i < Entity::numEntities; i++)
    {
        drawProperties[i].direction = ConvertDirectionToCharacter(velocities[i].direction);
        drawProperties[i].isBold = physics[i].mass >= massThreshold;
    }
}

std::thread* RenderJob::Run(const std::array<Entity::Position, Entity::numEntities>& positions)
{
    auto lambda = [this](const std::array<Entity::Position, Entity::numEntities>& inPositions)
    {
        this->PrintWorld(inPositions);
    };

    return new std::thread(lambda, std::ref(positions));
}

void RenderJob::PrintWorld(const std::array<Entity::Position, Entity::numEntities>& positions)
{
    constexpr size_t worldSize = 65;
    system("clear");

    PrintHorizontal(worldSize);

    const size_t centerIndex = GetLineCenterIndex(worldSize);
    for (size_t i = 0; i < worldSize - 2; i++)
    {
        char fill;
        char center;
        char spacing;
        if (i == centerIndex)
        {
            fill = '-';
            center = 'O';
            spacing = fill;
        }
        else
        {
            fill = ' ';
            center = '|';
            spacing = ' ';
        }

        PrintVerticalRow(worldSize, fill, center, spacing, i, positions);
    }

    PrintHorizontal(worldSize);
}

char RenderJob::ConvertDirectionToCharacter(Vector2 direction)
{
    size_t bestDirection = 0;
    float bestResult = -1.0f;
    for (size_t i = 0; i < static_cast<size_t>(DirectionalCharacter::NUM_DIRECTIONS); i++)
    {
        const float result = Vector::Dot01_Copy(direction, directions[i]);
        if (result > bestResult)
        {
            bestDirection = i;
            bestResult = result;
        }
    }

    return directionalCharacters[bestDirection];
}

size_t RenderJob::GetLineCenterIndex(const size_t worldSize)
{
    return worldSize / 2 - 1;
}

void RenderJob::PrintHorizontal(const size_t worldSize)
{
    for (size_t i = 0; i < worldSize; i++)
    {
        std::cout << "X ";
    }

    std::cout << std::endl;
}

void RenderJob::PrintVerticalRow(const size_t worldSize, const char fill, const char center, const char spacing, const size_t rowIndex, const std::array<Entity::Position, Entity::numEntities>& positions)
{
    std::cout << "X ";
    const size_t centerIndex = GetLineCenterIndex(worldSize);
    for (size_t i = 0; i < worldSize - 2; i++)
    {
        const char active = i == centerIndex ? center : fill;
        char final = active;
        for (size_t j = 0; j < positions.size(); j++)
        {
            if (IsWorldPosEqualToConsolePos(worldSize, i, rowIndex, positions[j].pos))
            {
                final = drawProperties[j].direction;
                break;
            }
        }

        std::cout << std::format("{}{}", final, spacing);
    }

    std::cout << 'X' << std::endl;
}

Vector2 RenderJob::GetConsoleCoordsFromWorldPos(const size_t worldSize, const Vector2& position)
{
    const int adder = GetLineCenterIndex(worldSize);
    return Vector2(std::floor(position.x) + adder, std::floor(-position.y) + adder);
}

bool RenderJob::IsWorldPosEqualToConsolePos(const size_t worldSize, const size_t column, const size_t row, const Vector2& position)
{
    const Vector2 converted = GetConsoleCoordsFromWorldPos(worldSize, position);
    return column == static_cast<size_t>(converted.x) && row == static_cast<size_t>(converted.y);
}

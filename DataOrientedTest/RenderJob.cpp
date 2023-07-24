#include "RenderJob.h"

#include <iostream>
#include <format>
#include <cmath>
#include <algorithm>
#include <ncurses.h>

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
    InitializeConsole();
    InitializeDrawProperties(velocities, physics);
    ClearBackBuffer();
}

RenderJob::~RenderJob()
{
    ShutDownConsole();
}

std::thread* RenderJob::Run(const std::array<Entity::Position, Entity::numEntities>& positions)
{
    auto lambda = [this](const std::array<Entity::Position, Entity::numEntities>& inPositions)
    {
        this->SwapBuffers();
        this->ClearBackBuffer();

        this->WriteWorld();
        this->WriteEntities(inPositions);
    };

    return new std::thread(lambda, std::ref(positions));
}

void RenderJob::InitializeConsole()
{
    initscr();
    cbreak();
    noecho();
    curs_set(0);
}

void RenderJob::InitializeDrawProperties(const std::array<Entity::Velocity, Entity::numEntities>& velocities, const std::array<Entity::Physics, Entity::numEntities>& physics)
{
    constexpr float massThreshold = 50.0f;
    for (size_t i = 0; i < Entity::numEntities; i++)
    {
        drawProperties[i].direction = ConvertDirectionToCharacter(velocities[i].direction);
        drawProperties[i].isBold = physics[i].mass >= massThreshold;
    }
}

void RenderJob::ShutDownConsole()
{
    endwin();
}

void RenderJob::WriteToBackBuffer(const size_t x, const size_t y, const char character)
{
    const size_t bufferIndex = y * consoleWidth + x;
    backBuffer[bufferIndex] = character;
}

void RenderJob::ClearBackBuffer()
{
    for (char& character : backBuffer)
    {
        character = ' ';
    }
}

void RenderJob::SwapBuffers()
{
    for (size_t i = 0; i < consoleWidth * worldHeight; i++)
    {
        const size_t x = i % consoleWidth;
        const size_t y = i / consoleWidth;
        mvaddch(y, x, backBuffer[i]);
        drawBuffer[i] = backBuffer[i];
    }

    refresh();
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

size_t RenderJob::GetCenterForAxis(const size_t axisSize)
{
    return axisSize / 2 - 1;
}

Vector2 RenderJob::GetConsoleCoordsFromWorldPos(const Vector2& position)
{
    const Vector2 worldLimit(static_cast<float>(GetCenterForAxis(worldWidth) - 1), static_cast<float>(GetCenterForAxis(worldHeight)) - 1);
    const Vector2 clampedPos(std::clamp(position.x, -worldLimit.x, worldLimit.x), std::clamp(position.y, -worldLimit.y, worldLimit.y));
    const size_t widthAdder = GetCenterForAxis(consoleWidth);
    const size_t heightAdder = GetCenterForAxis(worldHeight);
    return Vector2(std::floor(clampedPos.x * 2.0f) + widthAdder, std::floor(-clampedPos.y) + heightAdder);
}

void RenderJob::WriteWorld()
{
    WriteHorizontal(0);
    WriteHorizontal(worldHeight - 1);
    WriteVertical(0);
    WriteVertical(consoleWidth - 1);
    WriteXAxis();
    WriteYAxis();
    WriteOrigo();
}

void RenderJob::WriteHorizontal(const size_t column)
{
    for (size_t i = 0; i < consoleWidth; i++)
    {
        WriteToBackBuffer(i, column, 'X');
    }
}

void RenderJob::WriteVertical(const size_t row)
{
    for (size_t i = 0; i < worldHeight; i++)
    {
        WriteToBackBuffer(row, i, 'X');
    }
}

void RenderJob::WriteXAxis()
{
    const size_t row = GetCenterForAxis(worldHeight);
    for (size_t i = 1; i < consoleWidth - 1; i++)
    {
        WriteToBackBuffer(i, row, '-');
    }
}

void RenderJob::WriteYAxis()
{
    const size_t column = GetCenterForAxis(consoleWidth);
    for (size_t i = 1; i < worldHeight - 1; i++)
    {
        WriteToBackBuffer(column, i, '|');
    }
}

void RenderJob::WriteOrigo()
{
    WriteToBackBuffer(GetCenterForAxis(consoleWidth), GetCenterForAxis(worldHeight), 'O');
}

void RenderJob::WriteEntities(const std::array<Entity::Position, Entity::numEntities>& positions)
{
    for (size_t i = 0; i < positions.size(); i++)
    {
        const Vector2 consoleCoords = GetConsoleCoordsFromWorldPos(positions[i].pos);
        WriteToBackBuffer(consoleCoords.x, consoleCoords.y, drawProperties[i].direction);
    }
}

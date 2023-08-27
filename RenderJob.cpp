#include "RenderJob.h"

#include <iostream>
#include <algorithm>
#include <ncurses.h>

#include "Vector.h"
#include "Clocks.h"

float RenderJob::cachedWorldWidthCenter = 0.0f;
float RenderJob::cachedWorldHeightCenter = 0.0f;

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

RenderJob::RenderJob(const std::array<Entity::Velocity, Entity::numEntities>& velocities)
{
    InitializeConsole();
    InitializeDrawProperties(velocities);
    FillClearBuffer();
    ClearBackBuffer();

    cachedWorldWidthCenter = static_cast<float>(GetCenterForAxis(worldWidth));
    cachedWorldHeightCenter = static_cast<float>(GetCenterForAxis(worldHeight));
}

std::thread* RenderJob::Run(const std::array<Entity::Position, Entity::numEntities>& positions)
{
    auto lambda = [this](const std::array<Entity::Position, Entity::numEntities>& inPositions)
    {
        Clocks::StartRenderClock();

        this->SwapBuffers();
        this->ClearBackBuffer();
        this->WriteEntities(inPositions);

        Clocks::PauseRenderClock();
    };

    return new std::thread(lambda, std::ref(positions));
}

void RenderJob::ShutDownConsole()
{
    endwin();
}

void RenderJob::InitializeConsole()
{
    initscr();
    cbreak();
    noecho();
    curs_set(0);
}

void RenderJob::InitializeDrawProperties(const std::array<Entity::Velocity, Entity::numEntities>& velocities)
{
    for (size_t i = 0; i < Entity::numEntities; i++)
    {
        drawProperties[i].direction = ConvertDirectionToCharacter(velocities[i].direction);
    }
}

void RenderJob::WriteToBuffer(std::array<char, RenderJob::bufferSize>& buffer, const size_t x, const size_t y, const char character)
{
    const size_t index = y * consoleWidth + x;
    if (index >= bufferSize)
    {
        return;
    }

    buffer[index] = character;
}

void RenderJob::WriteToBackBuffer(const size_t x, const size_t y, const char character)
{
    WriteToBuffer(drawBuffer, x, y, character);
}

void RenderJob::WriteToClearBuffer(const size_t x, const size_t y, const char character)
{
    WriteToBuffer(clearBuffer, x, y, character);
}

void RenderJob::FillClearBuffer()
{
    for (char& character : clearBuffer)
    {
        character = ' ';
    }

    WriteWorld();
}

void RenderJob::ClearBackBuffer()
{
    drawBuffer = clearBuffer;
}

void RenderJob::SwapBuffers()
{
    for (size_t i = 0; i < consoleWidth * worldHeight; i++)
    {
        const size_t x = i % consoleWidth;
        const size_t y = i / consoleWidth;
        mvaddch(y, x, drawBuffer[i]);
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

void RenderJob::GetConsoleCoordsFromWorldPos(const Vector2& position, size_t& outX, size_t& outY)
{
    outX = static_cast<size_t>((position.x + cachedWorldWidthCenter) * 2.0f) - 1;
    outY = static_cast<size_t>(-position.y + cachedWorldHeightCenter) - 1;
}

void RenderJob::WriteWorld()
{
    WriteHorizontal(clearBuffer, 0);
    WriteHorizontal(clearBuffer, worldHeight - 1);
    WriteVertical(clearBuffer, 0);
    WriteVertical(clearBuffer, consoleWidth - 1);
    WriteXAxis();
    WriteYAxis();
    WriteOrigo();
}

void RenderJob::WriteHorizontal(std::array<char, bufferSize>& buffer, const size_t column)
{
    for (size_t i = 0; i < consoleWidth; i++)
    {
        WriteToBuffer(buffer, i, column, 'X');
    }
}

void RenderJob::WriteVertical(std::array<char, bufferSize>& buffer, const size_t row)
{
    for (size_t i = 0; i < worldHeight; i++)
    {
        WriteToBuffer(buffer, row, i, 'X');
    }
}

void RenderJob::WriteXAxis()
{
    const size_t row = GetCenterForAxis(worldHeight);
    for (size_t i = 1; i < consoleWidth - 1; i++)
    {
        WriteToClearBuffer(i, row, '-');
    }
}

void RenderJob::WriteYAxis()
{
    const size_t column = GetCenterForAxis(consoleWidth);
    for (size_t i = 1; i < worldHeight - 1; i++)
    {
        WriteToClearBuffer(column, i, '|');
    }
}

void RenderJob::WriteOrigo()
{
    WriteToClearBuffer(GetCenterForAxis(consoleWidth), GetCenterForAxis(worldHeight), 'O');
}

void RenderJob::WriteEntities(const std::array<Entity::Position, Entity::numEntities>& positions)
{
    for (size_t i = 0; i < positions.size(); i++)
    {
        size_t x;
        size_t y;
        GetConsoleCoordsFromWorldPos(positions[i].pos, x, y);
        WriteToBackBuffer(x, y, drawProperties[i].direction);
    }

    // We no longer make sure to keep the entities inside the grid. We let them wander outside, then we draw the borders on top after wards.
    // This is a bit of a hack, but it's much faster than clamping the positions due to the number of entities were dealing with.
    WriteHorizontal(drawBuffer, 0);
    WriteHorizontal(drawBuffer, worldHeight - 1);
    WriteVertical(drawBuffer, 0);
    WriteVertical(drawBuffer, consoleWidth - 1);
}
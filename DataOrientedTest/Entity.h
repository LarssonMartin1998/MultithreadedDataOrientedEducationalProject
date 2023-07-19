#pragma once

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

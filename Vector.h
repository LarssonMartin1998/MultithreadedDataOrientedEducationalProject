#pragma once

#include <cmath>

struct Vector2
{
    float x;
    float y;

    Vector2() : x(0.0f), y(0.0f) {}
    Vector2(float xIn, float yIn) : x(xIn), y(yIn) {}

    Vector2 operator*(const float scalar) const
    {
        return Vector2(x * scalar, y * scalar);
    }

    Vector2 operator/(const float scalar) const
    {
        return Vector2(x / scalar, y / scalar);
    }

    Vector2 operator+(const Vector2& other) const
    {
        return Vector2(x + other.x, y + other.y);
    }

    Vector2 operator-(const Vector2& other) const
    {
        return Vector2(x - other.x, y - other.y);
    }

    Vector2& operator+=(const Vector2& other)
    {
        x += other.x;
        y += other.y;
        return *this;
    }

    Vector2& operator-=(const Vector2& other)
    {
        x -= other.x;
        y -= other.y;
        return *this;
    }
};

namespace Vector
{
    inline float Magnitude(const Vector2& vector)
    {
        return std::sqrt(vector.x * vector.x + vector.y * vector.y);
    }

    inline void Normalize(Vector2& vector)
    {
        vector = vector / Magnitude(vector);
    }

    inline float Dot(const Vector2& a, const Vector2& b)
    {
        return a.x * b.x + a.y * b.y;
    }

    inline float Dot01(Vector2& a, Vector2& b)
    {
        Normalize(a);
        Normalize(b);
        return Dot(a, b);
    }

    inline float Dot01_Copy(Vector2 a, Vector2 b)
    {
        return Dot01(a, b);
    }
}

#pragma once

#include <cstddef>

#include "Vector.h"

namespace Entity
{
    constexpr size_t numEntities = 5;
    
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
}

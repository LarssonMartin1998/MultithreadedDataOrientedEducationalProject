#pragma once

#include <iostream>
#include <array>

#include "Entity.h"

namespace RandomizeJob
{
    template<std::size_t SIZE>
    void Run(std::array<Position, SIZE>& positions, std::array<Velocity, SIZE>& velocities, std::array<Physics, SIZE>& physics);
}

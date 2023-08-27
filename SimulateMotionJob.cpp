#include "SimulateMotionJob.h"

#include <iostream>
#include <algorithm>

#if defined(__x86_64__) || defined(_M_X64) // x64
#include <immintrin.h>
#elif defined(__arm__) || defined(__aarch64__) // ARM
#include <arm_neon.h>
#endif

#include "Clocks.h"
//#define RUN_WITHOUT_SIMD

namespace SimulateMotionJob
{
    constexpr float gravity = 9.82f;

    inline void IterateAndUpdateMotionOnRemaining(std::array<Entity::Position, Entity::numEntities>& positions, std::array<Entity::Velocity, Entity::numEntities>& velocities, std::array<Entity::Physics, Entity::numEntities>& physics, const size_t startIndex, const float gravityDelta)
    {
        const float deltaTime = Clocks::GetDeltaTime();
        for (size_t i = startIndex; i < velocities.size(); i++)
        {
            physics[i].acceleration = physics[i].acceleration - gravityDelta;
            velocities[i].speed = std::max(velocities[i].speed + physics[i].acceleration * deltaTime, 0.0f);

            positions[i].pos += velocities[i].direction * velocities[i].speed * deltaTime;
        }
    }

    inline void UpdateMotion(std::array<Entity::Position, Entity::numEntities>& positions, std::array<Entity::Velocity, Entity::numEntities>& velocities, std::array<Entity::Physics, Entity::numEntities>& physics)
    {
        Clocks::StartSimClock();

        const float deltaTime = Clocks::GetDeltaTime();
        const float gravityDelta = gravity * deltaTime;

#ifdef RUN_WITHOUT_SIMD
        IterateAndUpdateMotionOnRemaining(positions, velocities, physics, 0, gravityDelta);
        return;
#else

#if defined(__x86_64__) || defined(_M_X64) // x64
        const size_t simdWidth = 8;
        const __m256 deltaTimeEightLane = _mm256_set1_ps(deltaTime);
        const __m256 gravityDeltaEightLane = _mm256_set1_ps(gravityDelta);
#elif defined(__arm__) || defined(__aarch64__)
        const size_t simdWidth = 4;
        const float32x4_t gravityDeltaFourLane = vdupq_n_f32(gravityDelta);
#endif

        size_t i = 0;
        for (; i < velocities.size(); i += simdWidth)
        {
#if defined(__x86_64__) || defined(_M_X64) // x64
            __m256 accel = _mm256_setzero_ps();
            __m256 speed = _mm256_setzero_ps();
            __m256 directionX = _mm256_setzero_ps();
            __m256 directionY = _mm256_setzero_ps();
            __m256 posX = _mm256_setzero_ps();
            __m256 posY = _mm256_setzero_ps();
#elif defined(__arm__) || defined(__aarch64__)
            float32x4_t accel = vdupq_n_f32(0.0f);
            float32x4_t speed = vdupq_n_f32(0.0f);
            float32x4_t directionX = vdupq_n_f32(0.0f);
            float32x4_t directionY = vdupq_n_f32(0.0f);
            float32x4_t posX = vdupq_n_f32(0.0f);
            float32x4_t posY = vdupq_n_f32(0.0f);
#endif

#if defined(__x86_64__) || defined(_M_X64) // x64 architecture (AVX)
            for (int j = 0; j < simdWidth; ++j)
            {
                accel = _mm256_set_ps(0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, physics[i + j].acceleration);
                speed = _mm256_set_ps(0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, velocities[i + j].speed);
                directionX = _mm256_set_ps(0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, velocities[i + j].direction.x);
                directionY = _mm256_set_ps(0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, velocities[i + j].direction.y);
                posX = _mm256_set_ps(0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, positions[i + j].pos.x);
                posY = _mm256_set_ps(0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, positions[i + j].pos.y);
            }
#elif defined(__arm__) || defined(__aarch64__) // ARM architecture (NEON)
            accel = vsetq_lane_f32(physics[i].acceleration, accel, 0);
            speed = vsetq_lane_f32(velocities[i].speed, speed, 0);
            directionX = vsetq_lane_f32(velocities[i].direction.x, directionX, 0);
            directionY = vsetq_lane_f32(velocities[i].direction.y, directionY, 0);
            posX = vsetq_lane_f32(positions[i].pos.x, posX, 0);
            posY = vsetq_lane_f32(positions[i].pos.y, posY, 0);

            accel = vsetq_lane_f32(physics[i + 1].acceleration, accel, 1);
            speed = vsetq_lane_f32(velocities[i + 1].speed, speed, 1);
            directionX = vsetq_lane_f32(velocities[i + 1].direction.x, directionX, 1);
            directionY = vsetq_lane_f32(velocities[i + 1].direction.y, directionY, 1);
            posX = vsetq_lane_f32(positions[i + 1].pos.x, posX, 1);
            posY = vsetq_lane_f32(positions[i + 1].pos.y, posY, 1);

            accel = vsetq_lane_f32(physics[i + 2].acceleration, accel, 2);
            speed = vsetq_lane_f32(velocities[i + 2].speed, speed, 2);
            directionX = vsetq_lane_f32(velocities[i + 2].direction.x, directionX, 2);
            directionY = vsetq_lane_f32(velocities[i + 2].direction.y, directionY, 2);
            posX = vsetq_lane_f32(positions[i + 2].pos.x, posX, 2);
            posY = vsetq_lane_f32(positions[i + 2].pos.y, posY, 2);

            accel = vsetq_lane_f32(physics[i + 3].acceleration, accel, 3);
            speed = vsetq_lane_f32(velocities[i + 3].speed, speed, 3);
            directionX = vsetq_lane_f32(velocities[i + 3].direction.x, directionX, 3);
            directionY = vsetq_lane_f32(velocities[i + 3].direction.y, directionY, 3);
            posX = vsetq_lane_f32(positions[i + 3].pos.x, posX, 3);
            posY = vsetq_lane_f32(positions[i + 3].pos.y, posY, 3);
#endif

            // Perform SIMD operations
#if defined(__x86_64__) || defined(_M_X64) // x64
            accel = _mm256_sub_ps(accel, gravityDeltaEightLane);
            speed = _mm256_max_ps(speed, _mm256_add_ps(speed, _mm256_mul_ps(accel, deltaTimeEightLane)));
            const __m256 speedResult = _mm256_mul_ps(speed, deltaTimeEightLane);
            posX = _mm256_add_ps(posX, _mm256_mul_ps(directionX, speedResult));
            posY = _mm256_add_ps(posY, _mm256_mul_ps(directionY, speedResult));
#elif defined(__arm__) || defined(__aarch64__) // ARM
            accel = vsubq_f32(accel, gravityDeltaFourLane);
            speed = vmaxq_f32(speed, vmlaq_n_f32(speed, accel, deltaTime));
            posX = vmlaq_n_f32(posX, directionX, deltaTime);
            posY = vmlaq_n_f32(posY, directionY, deltaTime);
#endif

#if defined(__x86_64__) || defined(_M_X64) // x64
            for (int j = 0; j < simdWidth; ++j)
            {
                positions[i + j].pos.x = _mm256_cvtss_f32(posX);
                positions[i + j].pos.y = _mm256_cvtss_f32(posY);
                velocities[i + j].speed = _mm256_cvtss_f32(speed);
                physics[i + j].acceleration = _mm256_cvtss_f32(accel);
            }
#elif defined(__arm__) || defined(__aarch64__) // ARM
            positions[i].pos.x = vgetq_lane_f32(posX, 0);
            positions[i].pos.y = vgetq_lane_f32(posY, 0);
            velocities[i].speed = vgetq_lane_f32(speed, 0);
            physics[i].acceleration = vgetq_lane_f32(accel, 0);

            positions[i + 1].pos.x = vgetq_lane_f32(posX, 1);
            positions[i + 1].pos.y = vgetq_lane_f32(posY, 1);
            velocities[i + 1].speed = vgetq_lane_f32(speed, 1);
            physics[i + 1].acceleration = vgetq_lane_f32(accel, 1);

            positions[i + 2].pos.x = vgetq_lane_f32(posX, 2);
            positions[i + 2].pos.y = vgetq_lane_f32(posY, 2);
            velocities[i + 2].speed = vgetq_lane_f32(speed, 2);
            physics[i + 2].acceleration = vgetq_lane_f32(accel, 2);

            positions[i + 3].pos.x = vgetq_lane_f32(posX, 3);
            positions[i + 3].pos.y = vgetq_lane_f32(posY, 3);
            velocities[i + 3].speed = vgetq_lane_f32(speed, 3);
            physics[i + 3].acceleration = vgetq_lane_f32(accel, 3);
#endif
        }

        IterateAndUpdateMotionOnRemaining(positions, velocities, physics, i, gravityDelta);
#endif

        Clocks::PauseSimClock();
    }

    std::thread* Run(std::array<Entity::Position, Entity::numEntities>& positions, std::array<Entity::Velocity, Entity::numEntities>& velocities, std::array<Entity::Physics, Entity::numEntities>& physics)
    {
        return new std::thread(&UpdateMotion, std::ref(positions), std::ref(velocities), std::ref(physics));
    }
}
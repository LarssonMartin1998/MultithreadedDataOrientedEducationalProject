#include "SimulateMotionJob.h"

#include <iostream>
#include <algorithm>

#if defined(__x86_64__) || defined(_M_X64) // x64
#include <immintrin.h>
#elif defined(__arm__) || defined(__aarch64__) // ARM
#include <arm_neon.h>
#endif

#include "SIMDPlatform.h"

//#define RUN_WITHOUT_SIMD

namespace SimulateMotionJob
{
    constexpr float gravity = 9.82f;

    inline void IterateAndUpdateMotionOnRemaining(std::array<Entity::Position, Entity::numEntities>& positions, std::array<Entity::Velocity, Entity::numEntities>& velocities, std::array<Entity::Physics, Entity::numEntities>& physics, const float deltaTime, const size_t startIndex, const float gravityDelta)
    {
        for (size_t i = startIndex; i < velocities.size(); i++)
        {
            physics[i].acceleration = physics[i].acceleration - gravityDelta;
            velocities[i].speed = std::max(velocities[i].speed + physics[i].acceleration * deltaTime, 0.0f);

            positions[i].pos += velocities[i].direction * velocities[i].speed * deltaTime;
        }
    }

    inline void UpdateMotion(std::array<Entity::Position, Entity::numEntities>& positions, std::array<Entity::Velocity, Entity::numEntities>& velocities, std::array<Entity::Physics, Entity::numEntities>& physics, const float deltaTime)
    {
        const float gravityDelta = gravity * deltaTime;

#ifdef RUN_WITHOUT_SIMD
        IterateAndUpdateMotionOnRemaining(positions, velocities, physics, deltaTime, 0, gravityDelta);
        return;
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
            /*float32x4_t accel = vdupq_n_f32(0.0f);
            float32x4_t speed = vdupq_n_f32(0.0f);
            float32x4_t direction = vdupq_n_f32(0.0f);
            float32x4_t pos = vdupq_n_f32(0.0f);*/
#endif

            for (int j = 0; j < simdWidth; ++j)
            {
#if defined(__x86_64__) || defined(_M_X64) // x64 architecture (AVX)
                accel = _mm256_set_ps(0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, physics[i + j].acceleration);
                speed = _mm256_set_ps(0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, velocities[i + j].speed);
                directionX = _mm256_set_ps(0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, velocities[i + j].direction.x);
                directionY = _mm256_set_ps(0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, velocities[i + j].direction.y);
                posX = _mm256_set_ps(0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, positions[i + j].pos.x);
                posY = _mm256_set_ps(0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, positions[i + j].pos.y);
#elif defined(__arm__) || defined(__aarch64__) // ARM architecture (NEON)
                accel = vsetq_lane_f32(physics[i + j].acceleration, accel, j);
                speed = vsetq_lane_f32(velocities[i + j].speed, speed, j);
                direction = vsetq_lane_f32(velocities[i + j].direction, direction, j);
                pos = vsetq_lane_f32(positions[i + j].pos, pos, j);
#endif
            }

            // Perform SIMD operations
#if defined(__x86_64__) || defined(_M_X64) // x64
            accel = _mm256_sub_ps(accel, _mm256_set1_ps(gravityDelta));
            speed = _mm256_max_ps(speed, _mm256_add_ps(speed, _mm256_mul_ps(accel, _mm256_set1_ps(deltaTime))));
            posX = _mm256_add_ps(posX, _mm256_mul_ps(directionX, _mm256_mul_ps(speed, _mm256_set1_ps(deltaTime))));
            posY = _mm256_add_ps(posY, _mm256_mul_ps(directionY, _mm256_mul_ps(speed, _mm256_set1_ps(deltaTime))));
#elif defined(__arm__) || defined(__aarch64__) // ARM
            /*accel = vsubq_f32(accel, vdupq_n_f32(gravityDelta));
            speed = vmaxq_f32(speed, vmlaq_n_f32(speed, accel, deltaTime));
            pos = vmlaq_n_f32(pos, direction, deltaTime);*/
#endif

            for (int j = 0; j < simdWidth; ++j)
            {
#if defined(__x86_64__) || defined(_M_X64) // x64
                positions[i + j].pos.x = _mm256_cvtss_f32(posX);
                positions[i + j].pos.y = _mm256_cvtss_f32(posY);
                velocities[i + j].speed = _mm256_cvtss_f32(speed);
                physics[i + j].acceleration = _mm256_cvtss_f32(accel);
#elif defined(__arm__) || defined(__aarch64__) // ARM
                positions[i + j].pos = vgetq_lane_f32(pos, j);
                velocities[i + j].speed = vgetq_lane_f32(speed, j);
                physics[i + j].acceleration = vgetq_lane_f32(accel, j);
#endif
            }
        }

        IterateAndUpdateMotionOnRemaining(positions, velocities, physics, deltaTime, i, gravityDelta);
    }

    std::thread* Run(std::array<Entity::Position, Entity::numEntities>& positions, std::array<Entity::Velocity, Entity::numEntities>& velocities, std::array<Entity::Physics, Entity::numEntities>& physics, const float deltaTime)
    {
        return new std::thread(&UpdateMotion, std::ref(positions), std::ref(velocities), std::ref(physics), deltaTime);
    }
}
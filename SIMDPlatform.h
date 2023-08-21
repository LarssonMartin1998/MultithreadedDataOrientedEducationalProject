#pragma once

#include <cstddef>

#if defined(__x86_64__) || defined(_M_X64) // x64
constexpr size_t simdWidth = 8;
#elif defined(__arm__) || defined(__aarch64__) // ARM
constexpr size_t simdWidth = 4;
#else
#define RUN_WITHOUT_SIMD 1
constexpr size_t simdWidth = 1;
#endif
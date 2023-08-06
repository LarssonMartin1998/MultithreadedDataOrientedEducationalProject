#pragma once

#include <cstddef>

#if defined(__i386__) || defined(_M_IX86) // x86
constexpr size_t simdWidth = 4;
#elif defined(__x86__64__) || defined(_M_X64) // x64
constexpr size_t simdWidth = 8;
#elif defined(__arm__) || defined(__aarch64__) // ARM
constexpr size_t simdWidth = 4;
#else
#define RUN_WITHOUT_SIMD 1
constexpr size_t simdWidth = 1;
#endif
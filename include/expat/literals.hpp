#pragma once
#include <cstddef>

namespace expat {
/**
 * @brief Represents a number multiplied by 1024. So for example, 16_kb == 16384
 *
 * @param x the number
 * @return size_t x * 1024
 */
constexpr size_t operator""_kb(unsigned long long x) { return x * 1024; }
} // namespace expat

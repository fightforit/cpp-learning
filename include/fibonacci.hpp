#pragma once
#include <cstdint>

constexpr std::uint64_t fibonacciNumber(unsigned count) {
  std::uint64_t previous = 0;
  std::uint64_t current = 1;
  for (unsigned index = 0; index < count; ++index) {
    const auto nextValue = previous + current;
    previous = current;
    current = nextValue;
  }
  return previous;
}

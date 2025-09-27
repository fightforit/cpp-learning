#pragma once
#include <cstdint>

constexpr std::uint64_t fibonacci(unsigned n) {
  std::uint64_t a = 0, b = 1;
  for (unsigned i = 0; i < n; ++i) {
    auto next = a + b;
    a = b;
    b = next;
  }
  return a;
}
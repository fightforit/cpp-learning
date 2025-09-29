#pragma once
#include <concepts>

// Simple concept: integral types only
template <std::integral Integer>
[[nodiscard]] constexpr bool isPrime(Integer value) {

  if (value < 2)
    return false;

  if (value % 2 == 0)
    return value == 2;

  for (Integer divisor = 3; divisor * divisor <= value; divisor += 2) {
    if (value % divisor == 0)
      return false;
  }

  return true;
}

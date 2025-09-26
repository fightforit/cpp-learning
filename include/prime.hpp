#pragma once
#include <cmath>
#include <concepts>

// simple concept: integral types only
template <std::integral I> [[nodiscard]] constexpr bool is_prime(I n) {
  if (n < 2)
    return false;
  if (n % 2 == 0)
    return n == 2;
  // check odd divisors up to sqrt(n)
  for (I d = 3; d * d <= n; d += 2) {
    if (n % d == 0)
      return false;
  }
  return true;
}
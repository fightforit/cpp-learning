#pragma once
#include <concepts>

// simple concept: integral types only
template <std::integral I> [[nodiscard]] constexpr bool is_prime(I n) {

  // Handle edge cases
  if (n < 2)
    return false;

  // Check for even numbers greater than 2
  if (n % 2 == 0)
    return n == 2;

  // Check for factors from 3 to sqrt(n), skipping even numbers
  for (I d = 3; d * d <= n; d += 2) {
    if (n % d == 0)
      return false;
  }

  return true;
}
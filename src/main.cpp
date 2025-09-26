#include "prime.hpp"
#include <iostream>

int main() {
  std::cout << "== math-utils demo ==\n";

  // prime
  for (int n : {1, 2, 3, 4, 5, 97, 100}) {
    std::cout << n << (is_prime(n) ? " is prime\n" : " is not prime\n");
  }
  return 0;
}
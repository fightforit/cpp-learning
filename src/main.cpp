#include "fibonacci.hpp"
#include "prime.hpp"
#include "quantize.hpp"
#include <array>
#include <format>
#include <iostream>
#include <string_view>
#include <utility>

int main() {
  auto print = []<typename... Args>(std::format_string<Args...> fmt,
                                    Args &&...args) {
    std::cout << std::format(fmt, std::forward<Args>(args)...);
  };

  print("== math-utils demo ==\n");

  constexpr auto primes_to_check = std::to_array<int>({1, 2, 3, 4, 5, 97, 100});
  print("\n-- prime --\n");
  for (auto n : primes_to_check) {
    print("{} is {}prime\n", n, is_prime(n) ? "" : "not ");
  }

  constexpr auto fibonacci_inputs =
      std::to_array<unsigned>({0, 1, 2, 3, 4, 5, 10, 20});
  print("\n-- fibonacci --\n");
  for (auto n : fibonacci_inputs) {
    print("fib({}) = {}\n", n, fibonacci(n));
  }

  constexpr auto quantize_inputs = std::to_array<double>(
      {-9.0, -8.0, -7.99, -0.1, 0.0, 0.24, 0.25, 1.37, 7.74, 7.99, 8.0, 9.0});
  constexpr auto quant_params = std::pair{3, 2};

  const auto show_quantize = [&](QuantizeEncoding encoding,
                                 std::string_view label) {
    print("\n-- quantize ({}) --\n", label);

    for (auto x : quantize_inputs) {
      auto result =
          quantize(x, quant_params.first, quant_params.second, encoding);
      print("n={} -> nq={}\n", x, result);
    }
  };

  using enum QuantizeEncoding;
  show_quantize(TwosComplement, "two's complement");
  show_quantize(SignMagnitude, "sign/magnitude");

  return 0;
}

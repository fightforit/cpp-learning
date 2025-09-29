#include "fibonacci.hpp"
#include "fixed_point.hpp"
#include "prime.hpp"
#include <array>
#include <format>
#include <iostream>
#include <string_view>
#include <utility>

int main() {
  auto print = []<typename... Args>(std::format_string<Args...> formatString,
                                    Args &&...arguments) {
    std::cout << std::format(formatString, std::forward<Args>(arguments)...);
  };

  print("== math-utils demo ==\n");

  constexpr auto primesToCheck = std::to_array<int>({1, 2, 3, 4, 5, 97, 100});
  print("\n-- prime --\n");
  for (auto value : primesToCheck) {
    print("{} is {}prime\n", value, isPrime(value) ? "" : "not ");
  }

  constexpr auto fibonacciInputs =
      std::to_array<unsigned>({0, 1, 2, 3, 4, 5, 10, 20});
  print("\n-- fibonacci --\n");
  for (auto index : fibonacciInputs) {
    print("fib({}) = {}\n", index, fibonacciNumber(index));
  }

  constexpr auto quantizeInputs = std::to_array<double>(
      {-9.0,  -8.75, -8.0,  -7.99, -6.5,  -5.25, -3.5,  -2.0,  -1.25, -0.75,
       -0.51, -0.24, -0.1,  0.0,   0.24,  0.25,  0.5,   0.75,  1.0,   1.37,
        2.5,   3.25,  4.5,   5.75,  6.25,  7.0,   7.5,   7.74,  7.99,  8.0,
        9.0});
  constexpr auto quantParams = std::pair{3, 2};

  print("\n-- fixed-point quantize comparison --\n");
  print("{:>8} | {:>15} | {:>15}\n", "value", "two's complement",
        "sign magnitude");
  print("{:>8} | {:>7} {:>7} | {:>7} {:>7}\n", "", "quant", "bits", "quant",
        "bits");
  for (auto sample : quantizeInputs) {
    FixedPoint twos{sample, quantParams.first, quantParams.second,
                    QuantizeEncoding::TwosComplement};
    FixedPoint sign{sample, quantParams.first, quantParams.second,
                    QuantizeEncoding::SignMagnitude};
    print("{:>8.2f} | {:>7.2f} {:>7} | {:>7.2f} {:>7}\n", sample,
          twos.getQuantizedValue(), twos.getBinary(),
          sign.getQuantizedValue(), sign.getBinary());
  }

  print("\n-- fixed-point arithmetic demo --\n");
  FixedPoint fixedA{1.5, 3, 2, QuantizeEncoding::TwosComplement};
  FixedPoint fixedB{2.25, 3, 2, QuantizeEncoding::TwosComplement};
  const auto fixedSum = fixedA + fixedB;
  const auto fixedDifference = fixedA - fixedB;
  const auto fixedProduct = fixedA * fixedB;
  const auto fixedQuotient = fixedA / fixedB;
  const auto fixedNegation = -fixedA;

  const auto showFixedPoint = [&](std::string_view label,
                                  const FixedPoint &value) {
    print("{}: original={} quantized={} bits={}\n", label,
          value.getOriginalValue(), value.getQuantizedValue(),
          value.getBinary());
  };

  showFixedPoint("a", fixedA);
  showFixedPoint("b", fixedB);
  showFixedPoint("a + b", fixedSum);
  showFixedPoint("a - b", fixedDifference);
  showFixedPoint("a * b", fixedProduct);
  showFixedPoint("a / b", fixedQuotient);
  showFixedPoint("-a", fixedNegation);

  return 0;
}

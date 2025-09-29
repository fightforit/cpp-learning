#pragma once

#include <algorithm>
#include <cmath>
#include <cstdint>
#include <stdexcept>
#include <string>

enum class QuantizeEncoding { TwosComplement, SignMagnitude };

class FixedPoint {
public:
  static double
  quantizeValue(double value, int integerBits, int fractionalBits,
                QuantizeEncoding encoding = QuantizeEncoding::TwosComplement) {
    if (integerBits <= 0)
      throw std::invalid_argument("integerBits must be positive");
    if (fractionalBits < 0)
      throw std::invalid_argument("fractionalBits must be non-negative");

    const double step = std::ldexp(1.0, -fractionalBits);
    const double fullScale = std::ldexp(1.0, integerBits);

    const double maxValue = fullScale - step;
    const double minValue =
        encoding == QuantizeEncoding::TwosComplement ? -fullScale : -maxValue;

    const auto clampValue = [&](double candidate) {
      return std::clamp(candidate, minValue, maxValue);
    };

    const auto quantizeStep = [&](double candidate) {
      const double shiftAmount = std::ldexp(1.0, fractionalBits);
      return std::floor(candidate * shiftAmount) / shiftAmount;
    };

    if (!std::isfinite(value))
      return std::signbit(value) ? minValue : maxValue;

    const auto quantizeTwosComplement = [&]() {
      const double saturated = clampValue(value);
      const double quantized = quantizeStep(saturated);
      return clampValue(quantized);
    };

    const auto quantizeSignMagnitude = [&]() {
      const double magnitude = std::min(std::fabs(value), maxValue);
      const double quantizedMagnitude = quantizeStep(magnitude);
      const double quantized = std::copysign(quantizedMagnitude, value);
      const double clamped = clampValue(quantized);
      return clamped == 0.0 ? 0.0 : clamped;
    };

    return encoding == QuantizeEncoding::TwosComplement
               ? quantizeTwosComplement()
               : quantizeSignMagnitude();
  }

  FixedPoint(double value, int integerBits, int fractionalBits,
             QuantizeEncoding encoding = QuantizeEncoding::TwosComplement)
      : originalValue(value), integerBits(integerBits),
        fractionalBits(fractionalBits), quantizeEncoding(encoding) {
    if (integerBits <= 0)
      throw std::invalid_argument("integerBits must be positive");
    if (fractionalBits < 0)
      throw std::invalid_argument("fractionalBits must be non-negative");
    updateQuantizedValue();
  }

  double quantize(double value) {
    originalValue = value;
    return updateQuantizedValue();
  }

  double quantize() { return updateQuantizedValue(); }

  [[nodiscard]] double getOriginalValue() const { return originalValue; }
  [[nodiscard]] double getQuantizedValue() const { return quantizedValue; }
  [[nodiscard]] int getIntegerBits() const { return integerBits; }
  [[nodiscard]] int getFractionalBits() const { return fractionalBits; }
  [[nodiscard]] QuantizeEncoding getEncoding() const {
    return quantizeEncoding;
  }

  void setEncoding(QuantizeEncoding encoding) {
    quantizeEncoding = encoding;
    updateQuantizedValue();
  }

  [[nodiscard]] FixedPoint operator+(const FixedPoint &other) const {
    return add(other);
  }

  [[nodiscard]] FixedPoint operator-(const FixedPoint &other) const {
    return subtract(other);
  }

  [[nodiscard]] FixedPoint operator*(const FixedPoint &other) const {
    return multiply(other);
  }

  [[nodiscard]] FixedPoint operator/(const FixedPoint &other) const {
    return divide(other);
  }

  [[nodiscard]] FixedPoint operator-() const {
    return FixedPoint(-quantizedValue, integerBits, fractionalBits,
                      quantizeEncoding);
  }

  FixedPoint add(const FixedPoint &other) const {
    ensureCompatible(other);
    return FixedPoint(quantizedValue + other.quantizedValue, integerBits,
                      fractionalBits, quantizeEncoding);
  }

  FixedPoint subtract(const FixedPoint &other) const {
    ensureCompatible(other);
    return FixedPoint(quantizedValue - other.quantizedValue, integerBits,
                      fractionalBits, quantizeEncoding);
  }

  FixedPoint multiply(const FixedPoint &other) const {
    ensureCompatible(other);
    return FixedPoint(quantizedValue * other.quantizedValue, integerBits,
                      fractionalBits, quantizeEncoding);
  }

  FixedPoint divide(const FixedPoint &other) const {
    ensureCompatible(other);
    if (other.quantizedValue == 0.0)
      throw std::domain_error("Cannot divide by zero");
    return FixedPoint(quantizedValue / other.quantizedValue, integerBits,
                      fractionalBits, quantizeEncoding);
  }

  [[nodiscard]] std::string getBinary() const {
    const int totalBits = integerBits + fractionalBits + 1;
    if (totalBits <= 0 || totalBits > maxBinaryWidth)
      throw std::runtime_error("Unsupported bit width for binary formatting");

    const double scaled = std::ldexp(quantizedValue, fractionalBits);
    const auto rawValue = static_cast<std::int64_t>(std::llround(scaled));

    const std::uint64_t valueMask = (std::uint64_t{1} << totalBits) - 1;
    std::uint64_t encoded = 0;

    if (quantizeEncoding == QuantizeEncoding::TwosComplement) {
      encoded = static_cast<std::uint64_t>(rawValue) & valueMask;
    } else {
      const bool negative = std::signbit(quantizedValue);
      const std::uint64_t magnitudeMask =
          (std::uint64_t{1} << (totalBits - 1)) - 1;
      const std::uint64_t magnitude =
          static_cast<std::uint64_t>(std::llround(
              std::ldexp(std::fabs(quantizedValue), fractionalBits))) &
          magnitudeMask;
      encoded =
          (negative ? (std::uint64_t{1} << (totalBits - 1)) : 0) | magnitude;
    }

    return toBinaryString(encoded, totalBits);
  }

private:
  static constexpr int maxBinaryWidth = 63;

  double updateQuantizedValue() {
    quantizedValue = quantizeValue(originalValue, integerBits, fractionalBits,
                                   quantizeEncoding);
    return quantizedValue;
  }

  void ensureCompatible(const FixedPoint &other) const {
    if (integerBits != other.integerBits ||
        fractionalBits != other.fractionalBits ||
        quantizeEncoding != other.quantizeEncoding) {
      throw std::invalid_argument(
          "FixedPoint operands must share the same format to operate together");
    }
  }

  static std::string toBinaryString(std::uint64_t value, int width) {
    std::string result(width, '0');
    for (int index = width - 1; index >= 0; --index) {
      result[index] = static_cast<char>('0' + (value & 1u));
      value >>= 1u;
    }
    return result;
  }

  double originalValue;
  double quantizedValue{};
  int integerBits;
  int fractionalBits;
  QuantizeEncoding quantizeEncoding;
};

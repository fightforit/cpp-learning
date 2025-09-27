#pragma once
#include <algorithm>
#include <cmath>

enum class QuantizeEncoding { TwosComplement, SignMagnitude };

[[nodiscard]] constexpr double
quantize(double n, int n_int, int n_frc,
         QuantizeEncoding encoding = QuantizeEncoding::TwosComplement) {

  const double step = std::ldexp(1.0, -n_frc);
  const double full_scale = std::ldexp(1.0, n_int);

  const double max_b = full_scale - step;
  const double min_b =
      encoding == QuantizeEncoding::TwosComplement ? -full_scale : -max_b;

  const auto clamp = [&](double value) {
    return std::clamp(value, min_b, max_b);
  };

  const auto quant = [&](double value) {
    const double shift_amount = std::ldexp(1.0, n_frc);
    return std::floor(value * shift_amount) / shift_amount;
  };

  // Handle out-of-bounds and NaN inputs
  if (!std::isfinite(n))
    return std::signbit(n) ? min_b : max_b;

  const auto quantize_twos_complement = [&]() {
    const double saturated = clamp(n);
    const double quantized = quant(saturated);
    return clamp(quantized);
  };

  const auto quantize_sign_magnitude = [&]() {
    const double magnitude = std::min(std::fabs(n), max_b);
    const double quantized_mag = quant(magnitude);
    const double quantized = std::copysign(quantized_mag, n);
    const double clamped = clamp(quantized);
    return clamped == 0.0 ? 0.0 : clamped;
  };

  return encoding == QuantizeEncoding::TwosComplement
             ? quantize_twos_complement()
             : quantize_sign_magnitude();
}

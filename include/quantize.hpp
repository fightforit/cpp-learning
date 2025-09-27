#pragma once
#include <algorithm>
#include <cmath>
#include <utility>

enum class QuantizeEncoding { TwosComplement, SignMagnitude };

[[nodiscard]] constexpr double
quantize(double n, int n_int, int n_frc,
         QuantizeEncoding encoding = QuantizeEncoding::TwosComplement) {
  const double step = std::ldexp(1.0, -n_frc);
  const double scale = std::ldexp(1.0, n_frc);
  const double full_scale = std::ldexp(1.0, n_int);

  const auto [min_b, max_b] = [=]() {
    using enum QuantizeEncoding;
    switch (encoding) {
    case TwosComplement:
      return std::pair{-full_scale, full_scale - step};
    case SignMagnitude: {
      const double max_val = full_scale - step;
      return std::pair{-max_val, max_val};
    }
    }
    std::unreachable();
  }();

  const auto clamp = [&](double value) { return std::clamp(value, min_b, max_b); };

  if (!std::isfinite(n))
    return std::signbit(n) ? min_b : max_b;

  switch (encoding) {
  case QuantizeEncoding::TwosComplement: {
    const double clamped = clamp(n);
    const double q = std::floor(clamped * scale) / scale;
    return clamp(q);
  }
  case QuantizeEncoding::SignMagnitude: {
    const double magnitude = std::clamp(std::fabs(n), 0.0, max_b);
    const double q_mag = std::floor(magnitude * scale) / scale;
    const double q = std::signbit(n) ? -q_mag : q_mag;
    const double clamped = clamp(q);
    return clamped == 0.0 ? 0.0 : clamped;
  }
  }

  std::unreachable();
}

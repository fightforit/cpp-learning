Please use the following version

1. clang 20
2. clangd 20 (if you are using vscode)

Run the command to setup the clangd: `bazel run @wolfd_bazel_compile_commands//:generate_compile_commands`
Build the project: `bazel build ...`
Run the program: `bazel run //:<target_name>`

## FixedPoint toolkit

- `include/fixed_point.hpp` exposes the `QuantizeEncoding` enum, the static `FixedPoint::quantizeValue(...)` helper, and the `FixedPoint` class.
- `FixedPoint` retains both the original and quantized values, enforces consistent formats for arithmetic, and can emit the bit pattern via `getBinary()`.
- `src/main.cpp` shows how to call the static `FixedPoint::quantizeValue` helper and the class-based API side-by-side.


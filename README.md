Please use the following version

1. clang 20
2. clangd 20 (if you are using vscode)

Run the command to setup the clangd: `bazel run @wolfd_bazel_compile_commands//:generate_compile_commands`
Build the project: `bazel build ...`
Run the program: `bazel run //:<target_name>`
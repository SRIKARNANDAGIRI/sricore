# srichakra

C++20 graph complexity engine with JSON schema support, state machine lifecycle, and optional Python bindings.

## Requirements

- CMake 3.20+
- A C++20 compiler (MSVC 2019+, GCC 10+, Clang 12+)
- Git (for FetchContent dependency downloads)

## Build

```bash
cmake -B build && cmake --build build
```

On Windows with Visual Studio multi-config generators:

```powershell
cmake -B build
cmake --build build --config Release
```

### Targets

| Target           | Description              |
|------------------|--------------------------|
| `srichakra`      | Static library           |
| `srichakra_cli`  | CLI binary (output: `srichakra`) |
| `srichakra_tests`| Catch2 test suite        |

### Run tests

```bash
ctest --test-dir build --output-on-failure
```

On Windows:

```powershell
ctest --test-dir build -C Release --output-on-failure
```

### Python bindings

Build with CMake:

```bash
cmake -B build -DSRICHAKRA_BUILD_PYTHON=ON
cmake --build build
```

Or install via setuptools (requires a local C++20 toolchain):

```bash
pip install .
```

## Dependencies

All C++ dependencies are fetched automatically via CMake FetchContent:

- [nlohmann/json](https://github.com/nlohmann/json)
- [fmt](https://github.com/fmtlib/fmt)
- [Catch2](https://github.com/catchorg/Catch2)
- [pybind11](https://github.com/pybind/pybind11)

## Project layout

```
srichakra/
├── include/srichakra/   # Public headers
├── src/                 # Library and CLI sources
├── tests/               # Catch2 tests
├── bindings/            # pybind11 bindings
└── CMakeLists.txt
```

## License

MIT

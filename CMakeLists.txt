cmake_minimum_required(VERSION 3.20)

project(srichakra VERSION 0.1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

include(FetchContent)

# --- Dependencies via FetchContent ---

FetchContent_Declare(
    nlohmann_json
    GIT_REPOSITORY https://github.com/nlohmann/json.git
    GIT_TAG        v3.11.3
    GIT_SHALLOW    TRUE
)

FetchContent_Declare(
    fmt
    GIT_REPOSITORY https://github.com/fmtlib/fmt.git
    GIT_TAG        11.0.2
    GIT_SHALLOW    TRUE
)

FetchContent_Declare(
    Catch2
    GIT_REPOSITORY https://github.com/catchorg/Catch2.git
    GIT_TAG        v3.5.4
    GIT_SHALLOW    TRUE
)

FetchContent_Declare(
    pybind11
    GIT_REPOSITORY https://github.com/pybind/pybind11.git
    GIT_TAG        v2.13.6
    GIT_SHALLOW    TRUE
)

set(FMT_INSTALL OFF CACHE BOOL "" FORCE)
set(JSON_BuildTests OFF CACHE BOOL "" FORCE)
set(CATCH_INSTALL_DOCS OFF CACHE BOOL "" FORCE)
set(CATCH_INSTALL_EXTRAS OFF CACHE BOOL "" FORCE)
set(CATCH_BUILD_TESTING OFF CACHE BOOL "" FORCE)

FetchContent_MakeAvailable(nlohmann_json fmt Catch2 pybind11)

# --- Static library ---

add_library(srichakra STATIC
    src/complexity.cpp
    src/engine.cpp
)

target_include_directories(srichakra
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:include>
)

target_link_libraries(srichakra
    PUBLIC
        nlohmann_json::nlohmann_json
        fmt::fmt
)

target_compile_features(srichakra PUBLIC cxx_std_20)

if(MSVC)
    target_compile_options(srichakra PRIVATE /W4 /permissive-)
else()
    target_compile_options(srichakra PRIVATE -Wall -Wextra -Wpedantic)
endif()

# --- CLI binary ---

add_executable(srichakra_cli src/main.cpp)
target_link_libraries(srichakra_cli PRIVATE srichakra)
set_target_properties(srichakra_cli PROPERTIES OUTPUT_NAME srichakra)

# --- Test suite ---

enable_testing()

add_executable(srichakra_tests
    tests/test_state_machine.cpp
    tests/test_performance.cpp
)

target_link_libraries(srichakra_tests
    PRIVATE
        srichakra
        Catch2::Catch2WithMain
)

if(MSVC)
    target_compile_options(srichakra_tests PRIVATE /W4 /permissive-)
else()
    target_compile_options(srichakra_tests PRIVATE -Wall -Wextra -Wpedantic)
endif()

include(Catch)
catch_discover_tests(srichakra_tests)

# --- Python bindings (optional) ---

option(SRICHAKRA_BUILD_PYTHON "Build Python bindings" OFF)

if(SRICHAKRA_BUILD_PYTHON)
    pybind11_add_module(_srichakra bindings/python_bindings.cpp)
    target_link_libraries(_srichakra PRIVATE srichakra)
    set_target_properties(_srichakra PROPERTIES
        LIBRARY_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
    )
endif()

cmake_minimum_required(VERSION 3.28)

project(gsim
    VERSION 0.1.0
    DESCRIPTION "Headless 3D game simulation for learning C++, 3D math and game architecture"
    LANGUAGES CXX)

# ---------------------------------------------------------------------------
# Global policies / settings
# ---------------------------------------------------------------------------
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

if(PROJECT_IS_TOP_LEVEL)
    set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin")
    set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib")
endif()

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")
include(CompilerWarnings)

# ---------------------------------------------------------------------------
# Options
# ---------------------------------------------------------------------------
option(GSIM_BUILD_APP "Build the CLI application" ON)
option(GSIM_BUILD_TESTS "Build the unit tests" ${PROJECT_IS_TOP_LEVEL})
option(GSIM_ENABLE_ASSERTS "Force GSIM_ASSERT on, even in Release" OFF)

# ---------------------------------------------------------------------------
# Subdirectories
# ---------------------------------------------------------------------------
add_subdirectory(src/core)

if (GSIM_BUILD_APP)
    add_subdirectory(app)
endif()

if (GSIM_BUILD_TESTS)
    enable_testing()
    add_subdirectory(tests)
endif()

message(STATUS "gsim ${PROJECT_VERSION} / build type: ${CMAKE_BUILD_TYPE}")

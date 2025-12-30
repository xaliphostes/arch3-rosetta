# ============================================================================
# Place this file as "CMakeLists_generator.txt" in your project root,
# alongside project.json, and create my_generator.cxx there as well.
# ============================================================================

cmake_minimum_required(VERSION 3.16)
project(arch3_binding_generator VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20) # 20+ is needed for Rosetta
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# ============================================================================
# Paths - adjust these to your structure
# ============================================================================
set(ROSETTA_INCLUDE_DIR   "/Users/fmaerten/devs/c++/rosetta/include")
set(BINDING_GENERATOR_DIR "${CMAKE_CURRENT_SOURCE_DIR}/generator")

set(ARCH3_INCLUDE_DIR     "/Users/fmaerten/devs/c++/arch3/include")
set(ARCH3_LIB_DIR         "/Users/fmaerten/devs/c++/bin")
set(EIGEN_INCLUDE_DIR     "/Users/fmaerten/devs/c++/arch3/extern/eigen")
set(KRYLOV_INCLUDE_DIR    "/Users/fmaerten/devs/c++/arch3/extern/krylov/include")

# ============================================================================
# Dependencies
# ============================================================================

find_package(nlohmann_json 3.9 QUIET)
if(NOT nlohmann_json_FOUND)
    include(FetchContent)
    FetchContent_Declare(
        nlohmann_json
        GIT_REPOSITORY https://github.com/nlohmann/json.git
        GIT_TAG v3.11.2
    )
    FetchContent_MakeAvailable(nlohmann_json)
endif()

# ============================================================================
# Generator executable
# ============================================================================

add_executable(arch3_generator
    main.cxx
)

target_include_directories(arch3_generator PRIVATE
    ${BINDING_GENERATOR_DIR}
    ${ROSETTA_INCLUDE_DIR}
    ${ARCH3_INCLUDE_DIR}
    ${EIGEN_INCLUDE_DIR}
    ${KRYLOV_INCLUDE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}  # For bindings/
)

# ============================================================================
# Find the Arch3 library
# ============================================================================
find_library(ARCH3_LIBRARY
    NAMES Arch3 libArch3
    PATHS ${ARCH3_LIB_DIR}
    NO_DEFAULT_PATH
)

if(NOT ARCH3_LIBRARY)
    message(FATAL_ERROR "Arch3 library not found in ${ARCH3_LIB_DIR}")
endif()

message(STATUS "Found Arch3: ${ARCH3_LIBRARY}")

# ============================================================================
# nlohmann_json library
# ============================================================================
target_link_libraries(arch3_generator PRIVATE
    nlohmann_json::nlohmann_json
    ${ARCH3_LIBRARY}
)

# ============================================================================
# Usage:
#   mkdir build && cd build
#   cmake ..
#   make
#   ./generator
# ============================================================================

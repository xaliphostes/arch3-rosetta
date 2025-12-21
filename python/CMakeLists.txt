cmake_minimum_required(VERSION 3.15)
project(pyarch)

set(MODULE_NAME "pyarch")

# C++ standard
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include(FetchContent)

FetchContent_Declare(
    pybind11
    GIT_REPOSITORY https://github.com/pybind/pybind11.git
    GIT_TAG v3.0.1
)
FetchContent_MakeAvailable(pybind11)

pybind11_add_module(${MODULE_NAME} binding.cxx)

# Use the imported target 'arch' from the parent CMakeLists.txt
target_link_libraries(${MODULE_NAME} PRIVATE arch)

# =============================================================================
# Handle RPATH for macOS/Linux so the .so can find libArch.dylib at runtime
# =============================================================================
set_target_properties(${MODULE_NAME} PROPERTIES
    BUILD_RPATH "${CMAKE_CURRENT_SOURCE_DIR}/../../bin"
    INSTALL_RPATH "@loader_path/../bin"
    INSTALL_RPATH_USE_LINK_PATH TRUE
)

# Set output directory
set_target_properties(${MODULE_NAME} PROPERTIES
    LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}"
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}"
)
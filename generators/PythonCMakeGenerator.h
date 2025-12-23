#pragma once
#include "CodeWriter.h"

// ============================================================================
// Python CMakeLists.txt Generator
// ============================================================================
class PythonCMakeGenerator : public CodeWriter {
  public:
    using CodeWriter::CodeWriter;

    void generate() override {
        line("cmake_minimum_required(VERSION 3.15)");
        line("project(" + config_.module_name + "_python)");
        line();
        line("set(MODULE_NAME \"" + config_.module_name + "\")");
        line();
        line("set(CMAKE_CXX_STANDARD 20)");
        line("set(CMAKE_CXX_STANDARD_REQUIRED ON)");
        line();
        line("include(FetchContent)");
        line();
        line("if(WIN32)");
        line("    set(Python_EXECUTABLE \"C:/Python313/\")");
        line("endif()");
        line();
        line("FetchContent_Declare(");
        line("    pybind11");
        line("    GIT_REPOSITORY https://github.com/pybind/pybind11.git");
        line("    GIT_TAG v2.13.0");
        line(")");
        line("FetchContent_MakeAvailable(pybind11)");
        line();
        line("# "
             "================================================================="
             "============");
        line("# Include directories");
        line("# "
             "================================================================="
             "============");
        line("include_directories(");
        line("    ${CMAKE_CURRENT_SOURCE_DIR}");
        line("    ${CMAKE_CURRENT_SOURCE_DIR}/..");
        line("    ${Python_INCLUDE_DIRS}");
        for (const auto &inc : config_.include_dirs) {
            line("    " + inc);
        }
        line(")");
        line();
        line("# "
             "================================================================="
             "============");
        line("# Create the Python module");
        line("# "
             "================================================================="
             "============");
        line("pybind11_add_module(${MODULE_NAME}");
        line("    generated_pybind11.cxx");
        line(")");
        line();
        line("target_include_directories(${MODULE_NAME} PRIVATE");
        line("    ${CMAKE_CURRENT_SOURCE_DIR}");
        line("    ${CMAKE_CURRENT_SOURCE_DIR}/..");
        line(")");
        line();
        line("if(WIN32 AND Python_LIBRARIES)");
        line("    target_link_libraries(${MODULE_NAME} PRIVATE "
             "${Python_LIBRARIES})");
        line("endif()");
        line();
        line("set_target_properties(${MODULE_NAME} PROPERTIES");
        line("    LIBRARY_OUTPUT_DIRECTORY \"${CMAKE_BINARY_DIR}\"");
        line("    RUNTIME_OUTPUT_DIRECTORY \"${CMAKE_BINARY_DIR}\"");
        line(")");
        line();
        line("if(WIN32)");
        line("    set_target_properties(${MODULE_NAME} PROPERTIES SUFFIX "
             "\".pyd\")");
        line("endif()");
        line();
        line("# Installation");
        line("install(TARGETS ${MODULE_NAME} LIBRARY DESTINATION . RUNTIME "
             "DESTINATION .)");
        line();
        line("# Copy test script");
        line("if(EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/test.py)");
        line("    configure_file(${CMAKE_CURRENT_SOURCE_DIR}/test.py "
             "${CMAKE_BINARY_DIR}/test.py COPYONLY)");
        line("endif()");
    }
};

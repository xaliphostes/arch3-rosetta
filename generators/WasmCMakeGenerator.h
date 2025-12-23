#pragma once
#include "CodeWriter.h"

// ============================================================================
// WASM CMakeLists.txt Generator
// ============================================================================
class WasmCMakeGenerator : public CodeWriter {
  public:
    using CodeWriter::CodeWriter;

    void generate() override {
        line("cmake_minimum_required(VERSION 3.15)");
        line("project(" + config_.module_name + "_wasm)");
        line();
        line("set(MODULE_NAME \"" + config_.module_name + "\")");
        line();
        line("set(CMAKE_CXX_STANDARD 20)");
        line("set(CMAKE_CXX_STANDARD_REQUIRED ON)");
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
        for (const auto &inc : config_.include_dirs) {
            line("    " + inc);
        }
        line(")");
        line();
        line("# "
             "================================================================="
             "============");
        line("# Library (for non-Emscripten builds)");
        line("# "
             "================================================================="
             "============");
        line("add_library(${MODULE_NAME} SHARED generated_embind.cxx)");
        line();
        line("# "
             "================================================================="
             "============");
        line("# Emscripten JavaScript module");
        line("# "
             "================================================================="
             "============");
        line("add_executable(${MODULE_NAME}js generated_embind.cxx)");
        line();
        line("set_target_properties(${MODULE_NAME}js PROPERTIES");
        line("    SUFFIX \".js\"");
        line("    LINK_FLAGS \"--bind -s MODULARIZE=1 -s EXPORT_NAME='" +
             config_.module_name +
             "Module' -s SINGLE_FILE=1 -s NO_DISABLE_EXCEPTION_CATCHING\"");
        line(")");
        line();
        line("# "
             "================================================================="
             "============");
        line("# Copy test files");
        line("# "
             "================================================================="
             "============");
        line("if(EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/test.mjs)");
        line("    configure_file(${CMAKE_CURRENT_SOURCE_DIR}/test.mjs "
             "${CMAKE_BINARY_DIR}/test.mjs COPYONLY)");
        line("endif()");
    }
};

#pragma once
#include <string>

// ============================================================================
// Configuration for the Generator
// ============================================================================
struct GeneratorConfig {
    std::string module_name = "arch3";
    std::string version = "1.0.0";
    std::string author = "Generated";
    std::string description =
        "C++ bindings generated from Rosetta introspection";
    std::string license = "MIT";

    // Include paths relative to the generated folder
    std::vector<std::string> include_dirs = {
        "${CMAKE_CURRENT_SOURCE_DIR}/..", "${CMAKE_CURRENT_SOURCE_DIR}/../..",
        "../../../include"};

    // Arch-specific headers to include
    std::vector<std::string> arch_headers = {
        "Arch/core/Material.h", "Arch/core/Model.h",
        "Arch/core/Remote.h",   "Arch/core/Surface.h",
        "Arch/core/Triangle.h", "Arch/postprocess/ParallelPostprocess.h",
        "Arch/solvers/Seidel.h"};
};
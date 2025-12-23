// ============================================================================
// Multi-Target Binding Generator
// Generates: pybind11, embind (WASM), N-API (Node.js), and TypeScript
// declarations Also generates all necessary build configuration files
// ============================================================================
//
// Usage:
//   ./binding_generator [output_dir]
//
// Output structure:
//   output_dir/
//   ├── python/
//   │   ├── generated_pybind11.cxx
//   │   ├── CMakeLists.txt
//   │   ├── setup.py
//   │   ├── pyproject.toml
//   │   ├── arch3.pyi
//   │   └── README.md
//   ├── javascript/
//   │   ├── generated_napi.cxx
//   │   ├── package.json
//   │   ├── binding.gyp
//   │   ├── index.js
//   │   ├── arch3.d.ts
//   │   └── README.md
//   └── wasm/
//       ├── generated_embind.cxx
//       ├── CMakeLists.txt
//       ├── arch3.d.ts
//       └── README.md
//
// ============================================================================

#pragma once

// #include <algorithm>
// #include <filesystem>
// #include <fstream>
// #include <functional>
// #include <iostream>
// #include <map>
// #include <set>
// #include <sstream>
// #include <string>
// #include <vector>

#include "GeneratorConfig.h"
#include "MultiTargetGenerator.h"

int main(int argc, char *argv[]) {
    std::string output_dir = ".";
    if (argc > 1) {
        output_dir = argv[1];
    }

    GeneratorConfig config;
    config.module_name = "arch3";
    config.version = "1.0.0";
    config.description = "Arch3 - C++ bindings via Rosetta introspection";

    try {
        MultiTargetGenerator generator(output_dir, config);
        generator.generate_all();
    } catch (const std::exception &e) {
        std::cerr << "Error: " << e.what() << "\n";
        return 1;
    }

    return 0;
}

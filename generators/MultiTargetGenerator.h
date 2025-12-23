#pragma once
#include "../python/registration.h" // ???? NOT HERE
#include <filesystem>

namespace fs = std::filesystem;

class MultiTargetGenerator {
  public:
    MultiTargetGenerator(const std::string &output_dir,
                         const GeneratorConfig &config = {})
        : output_dir_(output_dir), config_(config) {
        arch_rosetta::register_arch3_classes();
    }

    void generate_all() {
        generate_python();
        generate_wasm();
        generate_javascript();
        std::cout << "\n✓ All bindings generated in: " << output_dir_ << "\n";
    }

    void generate_python() {
        std::string dir = output_dir_ + "/python";
        fs::create_directories(dir);

        write_file(dir + "/generated_pybind11.cxx", [this](std::ostream &out) {
            Pybind11Generator gen(out, config_);
            gen.generate();
        });

        write_file(dir + "/CMakeLists.txt", [this](std::ostream &out) {
            PythonCMakeGenerator gen(out, config_);
            gen.generate();
        });

        write_file(dir + "/setup.py", [this](std::ostream &out) {
            PythonSetupPyGenerator gen(out, config_);
            gen.generate();
        });

        write_file(dir + "/pyproject.toml", [this](std::ostream &out) {
            PythonPyprojectGenerator gen(out, config_);
            gen.generate();
        });

        write_file(dir + "/" + config_.module_name + ".pyi",
                   [this](std::ostream &out) {
                       PythonStubGenerator gen(out, config_);
                       gen.generate();
                   });

        write_file(dir + "/README.md", [this](std::ostream &out) {
            PythonReadmeGenerator gen(out, config_);
            gen.generate();
        });

        std::cout << "✓ Python bindings: " << dir << "\n";
    }

    void generate_wasm() {
        std::string dir = output_dir_ + "/wasm";
        fs::create_directories(dir);

        write_file(dir + "/generated_embind.cxx", [this](std::ostream &out) {
            EmbindGenerator gen(out, config_);
            gen.generate();
        });

        write_file(dir + "/CMakeLists.txt", [this](std::ostream &out) {
            WasmCMakeGenerator gen(out, config_);
            gen.generate();
        });

        write_file(dir + "/" + config_.module_name + ".d.ts",
                   [this](std::ostream &out) {
                       TypeScriptGenerator gen(out, config_);
                       gen.generate();
                   });

        write_file(dir + "/README.md", [this](std::ostream &out) {
            WasmReadmeGenerator gen(out, config_);
            gen.generate();
        });

        std::cout << "✓ WASM bindings: " << dir << "\n";
    }

    void generate_javascript() {
        std::string dir = output_dir_ + "/javascript";
        fs::create_directories(dir);

        // Note: N-API binding generator not included - similar pattern to
        // pybind11 You would add NapiGenerator here following the same
        // structure

        write_file(dir + "/package.json", [this](std::ostream &out) {
            JsPackageJsonGenerator gen(out, config_);
            gen.generate();
        });

        write_file(dir + "/binding.gyp", [this](std::ostream &out) {
            JsBindingGypGenerator gen(out, config_);
            gen.generate();
        });

        write_file(dir + "/index.js", [this](std::ostream &out) {
            JsIndexGenerator gen(out, config_);
            gen.generate();
        });

        write_file(dir + "/" + config_.module_name + ".d.ts",
                   [this](std::ostream &out) {
                       TypeScriptGenerator gen(out, config_);
                       gen.generate();
                   });

        write_file(dir + "/README.md", [this](std::ostream &out) {
            JsReadmeGenerator gen(out, config_);
            gen.generate();
        });

        std::cout << "✓ JavaScript bindings: " << dir << "\n";
    }

  private:
    std::string output_dir_;
    GeneratorConfig config_;

    void write_file(const std::string &path,
                    std::function<void(std::ostream &)> writer) {
        std::ofstream out(path);
        if (!out) {
            throw std::runtime_error("Failed to create file: " + path);
        }
        writer(out);
    }
};
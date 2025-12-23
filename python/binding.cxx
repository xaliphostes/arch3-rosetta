// ============================================================================
// Python Bindings for Arch3 using Rosetta Introspection (NO EXTENSIONS)
// ============================================================================
// This file manually writes pybind11 bindings but uses Rosetta's introspection
// metadata (ROSETTA_GET_META) to access fields, methods, constructors, etc.
// ============================================================================

#include <pybind11/functional.h>
#include <pybind11/numpy.h>
#include <pybind11/pybind11.h>
#include <pybind11/stl.h>

#include <cstdint>  // For int32_t, uint64_t, etc.

#include <rosetta/rosetta.h>  // Only core Rosetta, no extensions

#include <Arch/core/Material.h>
#include <Arch/core/Model.h>
#include <Arch/core/Remote.h>
#include <Arch/core/Surface.h>
#include <Arch/core/Triangle.h>
#include <Arch/postprocess/ParallelPostprocess.h>
#include <Arch/solvers/ParallelSolver.h>
#include <Arch/solvers/Seidel.h>

#include "registration.h"

namespace py = pybind11;

using vector3  = std::array<double, 3>;
using matrix33 = std::array<double, 6>;

// ============================================================================
// Helper: Convert rosetta::Any to py::object
// ============================================================================
py::object any_to_python(const rosetta::Any& value) {
    auto ti = value.type();
    
    // Primitives
    if (ti == typeid(int))         return py::cast(value.as<int>());
    if (ti == typeid(double))      return py::cast(value.as<double>());
    if (ti == typeid(float))       return py::cast(value.as<float>());
    if (ti == typeid(bool))        return py::cast(value.as<bool>());
    if (ti == typeid(std::string)) return py::cast(value.as<std::string>());
    
    // Various integer types (size_t can be different on different platforms)
    if (ti == typeid(size_t))        return py::cast(value.as<size_t>());
    if (ti == typeid(unsigned int))  return py::cast(value.as<unsigned int>());
    if (ti == typeid(unsigned long)) return py::cast(value.as<unsigned long>());
    if (ti == typeid(long))          return py::cast(value.as<long>());
    if (ti == typeid(uint64_t))      return py::cast(value.as<uint64_t>());
    if (ti == typeid(int64_t))       return py::cast(value.as<int64_t>());
    if (ti == typeid(uint32_t))      return py::cast(value.as<uint32_t>());
    if (ti == typeid(int32_t))       return py::cast(value.as<int32_t>());
    
    // STL containers
    if (ti == typeid(std::vector<double>)) 
        return py::cast(value.as<std::vector<double>>());
    if (ti == typeid(std::vector<int>)) 
        return py::cast(value.as<std::vector<int>>());
    
    // Arch value types
    if (ti == typeid(arch::Vector3))
        return py::cast(value.as<arch::Vector3>());
    if (ti == typeid(arch::Matrix33))
        return py::cast(value.as<arch::Matrix33>());
    
    // Arch pointer types
    if (ti == typeid(arch::BaseRemote*))
        return py::cast(value.as<arch::BaseRemote*>());
    if (ti == typeid(arch::RemoteStress*))
        return py::cast(value.as<arch::RemoteStress*>());
    if (ti == typeid(arch::UserRemote*))
        return py::cast(value.as<arch::UserRemote*>());
    if (ti == typeid(arch::Model*))
        return py::cast(value.as<arch::Model*>());
    if (ti == typeid(arch::Surface*))
        return py::cast(value.as<arch::Surface*>());
    
    throw std::runtime_error("Unsupported type in any_to_python: " + 
                             std::string(ti.name()));
}

// ============================================================================
// Helper: Convert py::object to rosetta::Any given expected type
// ============================================================================
rosetta::Any python_to_any(py::object obj, std::type_index expected) {
    // Primitives
    if (expected == typeid(int))         return rosetta::Any(obj.cast<int>());
    if (expected == typeid(double))      return rosetta::Any(obj.cast<double>());
    if (expected == typeid(float))       return rosetta::Any(obj.cast<float>());
    if (expected == typeid(bool))        return rosetta::Any(obj.cast<bool>());
    if (expected == typeid(std::string)) return rosetta::Any(obj.cast<std::string>());
    if (expected == typeid(size_t))      return rosetta::Any(obj.cast<size_t>());
    
    // STL containers (by value and by const ref - stored as value in Any)
    if (expected == typeid(std::vector<double>) || 
        expected == typeid(const std::vector<double>&))
        return rosetta::Any(obj.cast<std::vector<double>>());
    if (expected == typeid(std::vector<int>) ||
        expected == typeid(const std::vector<int>&))
        return rosetta::Any(obj.cast<std::vector<int>>());
    
    // Arch value types
    if (expected == typeid(arch::Vector3) ||
        expected == typeid(const arch::Vector3&))
        return rosetta::Any(obj.cast<arch::Vector3>());
    if (expected == typeid(arch::Matrix33) ||
        expected == typeid(const arch::Matrix33&))
        return rosetta::Any(obj.cast<arch::Matrix33>());
    
    // Arch pointer types (for method arguments like addRemote(BaseRemote*))
    if (expected == typeid(arch::BaseRemote*))
        return rosetta::Any(obj.cast<arch::BaseRemote*>());
    if (expected == typeid(arch::RemoteStress*))
        return rosetta::Any(obj.cast<arch::RemoteStress*>());
    if (expected == typeid(arch::UserRemote*))
        return rosetta::Any(obj.cast<arch::UserRemote*>());
    if (expected == typeid(arch::Model*))
        return rosetta::Any(obj.cast<arch::Model*>());
    if (expected == typeid(arch::Surface*))
        return rosetta::Any(obj.cast<arch::Surface*>());
    
    // =========================================================================
    // Reference types - use std::ref() to store as reference_wrapper
    // This avoids copying and keeps the reference to the Python-owned object
    // Note: typeid(T&) == typeid(T), so we check for T
    // =========================================================================
    if (expected == typeid(arch::Model)) {
        // Store as reference_wrapper<Model> - no copy!
        return rosetta::Any(std::ref(obj.cast<arch::Model&>()));
    }
    if (expected == typeid(arch::Surface)) {
        return rosetta::Any(std::ref(obj.cast<arch::Surface&>()));
    }
    
    // String const ref
    if (expected == typeid(const std::string&) || expected == typeid(const String&))
        return rosetta::Any(obj.cast<std::string>());
    
    throw std::runtime_error("Unsupported type in python_to_any: " + 
                             std::string(expected.name()));
}

// ============================================================================
// Template: Bind a class using Rosetta introspection
// ============================================================================

// Helper to bind constructors - handles default ctor at compile time
// NOTE: Only handles value/pointer params. Reference params need manual binding!
template <typename T>
void bind_constructors(py::class_<T, std::shared_ptr<T>>& py_class) {
    auto& meta = ROSETTA_GET_META(T);
    const auto& ctor_infos = meta.constructor_infos();
    const auto& ctors = meta.constructors();
    
    // Default constructor: compile-time check
    if constexpr (std::is_default_constructible_v<T>) {
        for (const auto& info : ctor_infos) {
            if (info.arity == 0) {
                py_class.def(py::init<>());
                break;
            }
        }
    }
    
    // Parameterized constructors via factory
    // WARNING: This does NOT work for reference parameters (Model&, etc.)
    // because rosetta::Any stores by value, creating a temporary copy.
    // Constructors with reference params must be bound manually!
    for (size_t ci = 0; ci < ctor_infos.size(); ++ci) {
        const auto& info = ctor_infos[ci];
        if (info.arity > 0) {
            auto param_types = info.param_types;  // Copy for capture
            size_t arity = info.arity;
            auto invoker = ctors[ci];  // Copy for capture
            
            py_class.def(py::init([param_types, arity, invoker](py::args args) -> T {
                if (args.size() != arity) {
                    throw std::runtime_error(
                        "Constructor expects " + std::to_string(arity) + 
                        " arguments, got " + std::to_string(args.size()));
                }
                
                std::vector<rosetta::Any> cpp_args;
                for (size_t i = 0; i < args.size(); ++i) {
                    cpp_args.push_back(python_to_any(args[i], param_types[i]));
                }
                
                auto result = invoker(cpp_args);
                return result.template as<T>();
            }));
        }
    }
}

// Helper to bind fields as properties
template <typename T>
void bind_fields(py::class_<T, std::shared_ptr<T>>& py_class) {
    auto& meta = ROSETTA_GET_META(T);
    
    // Get field names as copy
    std::vector<std::string> field_names = meta.fields();
    
    for (const auto& field_name : field_names) {
        std::string fname = field_name;  // Copy for capture
        
        py_class.def_property(
            fname.c_str(),
            // Getter - fetch type inside lambda
            [fname](T& obj) -> py::object {
                auto& m = ROSETTA_GET_META(T);
                auto value = m.get_field(obj, fname);
                return any_to_python(value);
            },
            // Setter - fetch type inside lambda
            [fname](T& obj, py::object value) {
                auto& m = ROSETTA_GET_META(T);
                auto ftype = m.get_field_type(fname);
                auto any_val = python_to_any(value, ftype);
                m.set_field(obj, fname, any_val);
            }
        );
    }
}

// Helper to bind methods
template <typename T>
void bind_methods(py::class_<T, std::shared_ptr<T>>& py_class) {
    auto& meta = ROSETTA_GET_META(T);
    
    // Get method names as a copy to avoid iterator invalidation
    std::vector<std::string> method_names = meta.methods();
    
    for (const auto& method_name : method_names) {
        std::string mname = method_name;  // Copy for capture
        
        py_class.def(
            mname.c_str(),
            [mname](T& obj, py::args args) -> py::object {
                // ALL metadata access happens inside the lambda
                auto& m = ROSETTA_GET_META(T);
                
                // Get method info fresh each time
                const auto& method_infos = m.method_info(mname);
                if (method_infos.empty()) {
                    throw std::runtime_error("No method info for: " + mname);
                }
                
                const auto& info = method_infos[0];
                
                // Convert arguments
                std::vector<rosetta::Any> cpp_args;
                for (size_t i = 0; i < args.size() && i < info.arg_types.size(); ++i) {
                    cpp_args.push_back(python_to_any(args[i], info.arg_types[i]));
                }
                
                // Invoke the method
                rosetta::Any result;
                try {
                    result = m.invoke_method(obj, mname, cpp_args);
                } catch (const std::exception& e) {
                    throw std::runtime_error("Failed to invoke method '" + mname + "': " + e.what());
                }
                
                // Handle void return
                if (info.return_type == std::type_index(typeid(void))) {
                    return py::none();
                }
                
                // Convert result to Python
                try {
                    return any_to_python(result);
                } catch (const std::exception& e) {
                    throw std::runtime_error("Failed to convert result of '" + mname + "': " + e.what());
                }
            }
        );
    }
}

// Main function to bind a class from Rosetta metadata
// 
// Usage:
//   bind_class_from_meta<MyClass>(m, "MyClass");  // Uses introspection for ctors
//   bind_class_from_meta<MyClass, Model&>(m, "MyClass");  // Explicit ctor(Model&)
//   bind_class_from_meta<MyClass, int, double>(m, "MyClass");  // Explicit ctor(int, double)
//
// For classes with reference constructors (T&), you MUST specify the constructor
// signature explicitly, because rosetta::Any stores by value and cannot handle refs.

// Primary template: no explicit constructor args - use introspection
template <typename T, typename... CtorArgs>
void bind_class_from_meta(py::module_& m, const std::string& py_name, 
                          const std::string& docstring = "") {
    py::class_<T, std::shared_ptr<T>> py_class(m, py_name.c_str(), docstring.c_str());
    
    if constexpr (sizeof...(CtorArgs) > 0) {
        // Explicit constructor signature provided - use pybind11 directly
        // This handles reference parameters correctly!
        py_class.def(py::init<CtorArgs...>());
    } else {
        // No explicit ctor args - use introspection
        // WARNING: This doesn't work for reference parameters!
        bind_constructors<T>(py_class);
    }
    
    bind_fields<T>(py_class);
    bind_methods<T>(py_class);
    
    // Add __repr__
    py_class.def("__repr__", [py_name](const T&) {
        return "<" + py_name + " object>";
    });
}

// Variant for derived classes with base
template <typename Derived, typename Base>
void bind_derived_class_from_meta(py::module_& m, const std::string& py_name,
                                   const std::string& docstring = "") {
    auto& meta = ROSETTA_GET_META(Derived);
    
    py::class_<Derived, Base, std::shared_ptr<Derived>> py_class(
        m, py_name.c_str(), docstring.c_str());
    
    // Constructors
    {
        const auto& ctor_infos = meta.constructor_infos();
        const auto& ctors = meta.constructors();
        
        if constexpr (std::is_default_constructible_v<Derived>) {
            for (const auto& info : ctor_infos) {
                if (info.arity == 0) {
                    py_class.def(py::init<>());
                    break;
                }
            }
        }
        
        for (size_t ci = 0; ci < ctor_infos.size(); ++ci) {
            const auto& info = ctor_infos[ci];
            if (info.arity > 0) {
                auto param_types = info.param_types;
                size_t arity = info.arity;
                auto invoker = ctors[ci];
                
                py_class.def(py::init([param_types, arity, invoker](py::args args) -> Derived {
                    if (args.size() != arity) {
                        throw std::runtime_error(
                            "Constructor expects " + std::to_string(arity) + 
                            " arguments, got " + std::to_string(args.size()));
                    }
                    
                    std::vector<rosetta::Any> cpp_args;
                    for (size_t i = 0; i < args.size(); ++i) {
                        cpp_args.push_back(python_to_any(args[i], param_types[i]));
                    }
                    
                    auto result = invoker(cpp_args);
                    return result.template as<Derived>();
                }));
            }
        }
    }
    
    // Fields
    std::vector<std::string> field_names = meta.fields();
    for (const auto& field_name : field_names) {
        std::string fname = field_name;
        
        py_class.def_property(
            fname.c_str(),
            [fname](Derived& obj) -> py::object {
                auto& m = ROSETTA_GET_META(Derived);
                return any_to_python(m.get_field(obj, fname));
            },
            [fname](Derived& obj, py::object value) {
                auto& m = ROSETTA_GET_META(Derived);
                auto ftype = m.get_field_type(fname);
                m.set_field(obj, fname, python_to_any(value, ftype));
            }
        );
    }
    
    // Methods
    std::vector<std::string> method_names = meta.methods();
    for (const auto& method_name : method_names) {
        std::string mname = method_name;
        
        py_class.def(
            mname.c_str(),
            [mname](Derived& obj, py::args args) -> py::object {
                auto& m = ROSETTA_GET_META(Derived);
                
                const auto& method_infos = m.method_info(mname);
                if (method_infos.empty()) {
                    throw std::runtime_error("No method info for: " + mname);
                }
                
                const auto& info = method_infos[0];
                
                std::vector<rosetta::Any> cpp_args;
                for (size_t i = 0; i < args.size() && i < info.arg_types.size(); ++i) {
                    cpp_args.push_back(python_to_any(args[i], info.arg_types[i]));
                }
                
                rosetta::Any result;
                try {
                    result = m.invoke_method(obj, mname, cpp_args);
                } catch (const std::exception& e) {
                    throw std::runtime_error("Failed to invoke method '" + mname + "': " + e.what());
                }
                
                if (info.return_type == std::type_index(typeid(void))) {
                    return py::none();
                }
                
                try {
                    return any_to_python(result);
                } catch (const std::exception& e) {
                    throw std::runtime_error("Failed to convert result of '" + mname + "': " + e.what());
                }
            }
        );
    }
    
    py_class.def("__repr__", [py_name](const Derived&) {
        return "<" + py_name + " object>";
    });
}

// ============================================================================
// Python Module Definition
// ============================================================================

PYBIND11_MODULE(pyarch3, m) {
    m.doc() = "Arch3 - 3D BEM library (bound via Rosetta introspection)";

    // Step 1: Register all classes with Rosetta
    arch_rosetta::register_arch3_classes();

    // ========================================================================
    // Step 2: Bind classes using introspection
    // ========================================================================

    // --- Simple classes: fully automatic ---
    bind_class_from_meta<arch::Matrix33>(m, "Matrix33");
    bind_class_from_meta<arch::Vector3>(m, "Vector3");
    bind_class_from_meta<arch::Model>(m, "Model", 
        "A class that manages surfaces, remotes, solvers...");
    bind_class_from_meta<arch::Surface>(m, "Surface",
        "A discontinuity surface for BEM");
    
    // --- BaseRemote: abstract base ---
    bind_class_from_meta<arch::BaseRemote>(m, "BaseRemote",
        "Base class for remote strain/stress");

    // --- RemoteStress: derived from BaseRemote ---
    bind_derived_class_from_meta<arch::RemoteStress, arch::BaseRemote>(
        m, "RemoteStress", "An Andersonian remote stress");

    // --- UserRemote: needs manual constructor (std::function) ---
    py::class_<arch::UserRemote, arch::BaseRemote, std::shared_ptr<arch::UserRemote>>(
        m, "UserRemote", "User-defined remote stress/strain via lambda")
        .def(py::init([](std::function<std::array<double, 6>(double, double, double)> fct, 
                         bool is_stress) {
            auto wrapper = [fct](double x, double y, double z) -> arch::Matrix33 {
                auto arr = fct(x, y, z);
                return arch::Matrix33(arr[0], arr[1], arr[2], arr[3], arr[4], arr[5]);
            };
            return std::make_unique<arch::UserRemote>(wrapper, is_stress);
        }), py::arg("fct"), py::arg("is_stress") = true);

    // --- Solvers: now works automatically thanks to Any handling reference_wrapper ---
    bind_class_from_meta<arch::SeidelSolver>(m, "SeidelSolver");

    // --- Postprocess: also works automatically ---
    bind_class_from_meta<arch::ParallelPostprocess>(m, "Postprocess");

    // ========================================================================
    // Utility functions for introspection
    // ========================================================================
    m.def("list_classes", []() {
        return rosetta::Registry::instance().list_classes();
    }, "List all Rosetta-registered classes");

    m.def("get_class_methods", [](const std::string& class_name) {
        auto* holder = rosetta::Registry::instance().get_by_name(class_name);
        if (!holder) throw std::runtime_error("Class not found: " + class_name);
        return holder->get_methods();
    }, "Get all method names for a class");

    m.def("get_class_fields", [](const std::string& class_name) {
        auto* holder = rosetta::Registry::instance().get_by_name(class_name);
        if (!holder) throw std::runtime_error("Class not found: " + class_name);
        return holder->get_fields();
    }, "Get all field names for a class");
}
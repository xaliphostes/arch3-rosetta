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

#include <rosetta/rosetta.h> // Only core Rosetta, no extensions

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

using vector3 = std::array<double, 3>;
using matrix33 = std::array<double, 6>;

// ============================================================================
// Helper: Convert rosetta::Any to py::object
// ============================================================================
py::object any_to_python(const rosetta::Any &value) {
    auto ti = value.get_type_index();

    // Primitives
    if (ti == typeid(int))
        return py::cast(value.as<int>());
    if (ti == typeid(double))
        return py::cast(value.as<double>());
    if (ti == typeid(float))
        return py::cast(value.as<float>());
    if (ti == typeid(bool))
        return py::cast(value.as<bool>());
    if (ti == typeid(std::string))
        return py::cast(value.as<std::string>());
    if (ti == typeid(size_t))
        return py::cast(value.as<size_t>());

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
    if (ti == typeid(arch::BaseRemote *))
        return py::cast(value.as<arch::BaseRemote *>());
    if (ti == typeid(arch::RemoteStress *))
        return py::cast(value.as<arch::RemoteStress *>());
    if (ti == typeid(arch::UserRemote *))
        return py::cast(value.as<arch::UserRemote *>());
    if (ti == typeid(arch::Model *))
        return py::cast(value.as<arch::Model *>());
    if (ti == typeid(arch::Surface *))
        return py::cast(value.as<arch::Surface *>());

    throw std::runtime_error("Unsupported type in any_to_python: " +
                             std::string(ti.name()));
}

// ============================================================================
// Helper: Convert py::object to rosetta::Any given expected type
// ============================================================================
rosetta::Any python_to_any(py::object obj, std::type_index expected) {
    // Primitives
    if (expected == typeid(int))
        return rosetta::Any(obj.cast<int>());
    if (expected == typeid(double))
        return rosetta::Any(obj.cast<double>());
    if (expected == typeid(float))
        return rosetta::Any(obj.cast<float>());
    if (expected == typeid(bool))
        return rosetta::Any(obj.cast<bool>());
    if (expected == typeid(std::string))
        return rosetta::Any(obj.cast<std::string>());
    if (expected == typeid(size_t))
        return rosetta::Any(obj.cast<size_t>());

    // STL containers (by value and by const ref - stored as value in Any)
    if (expected == typeid(std::vector<double>) ||
        expected == typeid(const std::vector<double> &))
        return rosetta::Any(obj.cast<std::vector<double>>());
    if (expected == typeid(std::vector<int>) ||
        expected == typeid(const std::vector<int> &))
        return rosetta::Any(obj.cast<std::vector<int>>());

    // Arch value types
    if (expected == typeid(arch::Vector3) ||
        expected == typeid(const arch::Vector3 &))
        return rosetta::Any(obj.cast<arch::Vector3>());
    if (expected == typeid(arch::Matrix33) ||
        expected == typeid(const arch::Matrix33 &))
        return rosetta::Any(obj.cast<arch::Matrix33>());

    // Arch pointer types (for method arguments like addRemote(BaseRemote*))
    if (expected == typeid(arch::BaseRemote *))
        return rosetta::Any(obj.cast<arch::BaseRemote *>());
    if (expected == typeid(arch::RemoteStress *))
        return rosetta::Any(obj.cast<arch::RemoteStress *>());
    if (expected == typeid(arch::UserRemote *))
        return rosetta::Any(obj.cast<arch::UserRemote *>());
    if (expected == typeid(arch::Model *))
        return rosetta::Any(obj.cast<arch::Model *>());
    if (expected == typeid(arch::Surface *))
        return rosetta::Any(obj.cast<arch::Surface *>());

    // Arch reference types (converted from Python object)
    if (expected == typeid(arch::Model &))
        return rosetta::Any(std::ref(obj.cast<arch::Model &>()));
    if (expected == typeid(arch::Surface &))
        return rosetta::Any(std::ref(obj.cast<arch::Surface &>()));

    // String const ref
    if (expected == typeid(const std::string &) ||
        expected == typeid(const String &))
        return rosetta::Any(obj.cast<std::string>());

    throw std::runtime_error("Unsupported type in python_to_any: " +
                             std::string(expected.name()));
}

// ============================================================================
// Python Module Definition
// ============================================================================

PYBIND11_MODULE(pyarch3, m) {
    m.doc() = "Arch3 - 3D BEM library (bound via Rosetta introspection)";

    // Step 1: Register all classes with Rosetta
    arch_rosetta::register_arch3_classes();

    // ========================================================================
    // Step 2: Bind classes manually, but USE introspection for iteration
    // ========================================================================

    // --- Matrix33 ---
    {
        auto &meta = ROSETTA_GET_META(arch::Matrix33);
        py::class_<arch::Matrix33>(m, "Matrix33")
            .def(py::init<>())
            .def(py::init<double, double, double, double, double, double>())
            // Use introspection to bind methods dynamically
            .def("xx",
                 [&meta](arch::Matrix33 &obj) {
                     return meta.invoke_method(obj, "xx", {}).as<double>();
                 })
            .def("xy",
                 [&meta](arch::Matrix33 &obj) {
                     return meta.invoke_method(obj, "xy", {}).as<double>();
                 })
            .def("xz",
                 [&meta](arch::Matrix33 &obj) {
                     return meta.invoke_method(obj, "xz", {}).as<double>();
                 })
            .def("yy",
                 [&meta](arch::Matrix33 &obj) {
                     return meta.invoke_method(obj, "yy", {}).as<double>();
                 })
            .def("yz",
                 [&meta](arch::Matrix33 &obj) {
                     return meta.invoke_method(obj, "yz", {}).as<double>();
                 })
            .def("zz",
                 [&meta](arch::Matrix33 &obj) {
                     return meta.invoke_method(obj, "zz", {}).as<double>();
                 })
            .def("__repr__", [](const arch::Matrix33 &m) {
                return "Matrix33(xx=" + std::to_string(m.get(0, 0)) +
                       ", xy=" + std::to_string(m.get(0, 1)) +
                       ", xz=" + std::to_string(m.get(0, 2)) +
                       ", yy=" + std::to_string(m.get(1, 1)) +
                       ", yz=" + std::to_string(m.get(1, 2)) +
                       ", zz=" + std::to_string(m.get(2, 2)) + ")";
            });
    }

    // --- Vector3 ---
    {
        auto &meta = ROSETTA_GET_META(arch::Vector3);
        py::class_<arch::Vector3>(m, "Vector3")
            .def(py::init<double, double, double>())
            .def("x",
                 [&meta](arch::Vector3 &v) {
                     return meta.invoke_method(v, "x", {}).as<double>();
                 })
            .def("y",
                 [&meta](arch::Vector3 &v) {
                     return meta.invoke_method(v, "y", {}).as<double>();
                 })
            .def("z",
                 [&meta](arch::Vector3 &v) {
                     return meta.invoke_method(v, "z", {}).as<double>();
                 })
            .def("__repr__", [](const arch::Vector3 &v) {
                return "Vector3(" + std::to_string(v[0]) + ", " +
                       std::to_string(v[1]) + ", " + std::to_string(v[2]) + ")";
            });
    }

    // // --- Model ---
    {
        auto &meta = ROSETTA_GET_META(arch::Model);
        py::class_<arch::Model>(
            m, "Model", "A class that manages surfaces, remotes, solvers...") 
            .def(py::init<>())
            .def("setHalfSpace",
                 [&meta](arch::Model &obj, bool v) {
                     meta.invoke_method(obj, "setHalfSpace", {v});
                 })
            .def("halfSpace",
                 [&meta](arch::Model &obj) {
                     return meta.invoke_method(obj, "halfSpace", {}).as<bool>();
                 })
            .def("nbDof",
                 [&meta](arch::Model &obj) {
                     return meta.invoke_method(obj, "nbDof", {}).as<size_t>();
                 })
            .def("addRemote", &arch::Model::addRemote)
            .def("bounds", &arch::Model::bounds)
            .def("setMaterial",
                 [&meta](arch::Model &obj, double p, double y, double d) {
                     meta.invoke_method(obj, "setMaterial", {p, y, d});
                 });
    }

    // --- Surface ---
    {
        auto &meta = ROSETTA_GET_META(arch::Surface);
        py::class_<arch::Surface>(m, "Surface",
                                  "A discontinuity surface for BEM")
            .def(py::init<arch::Model *, const std::vector<double> &,
                          const std::vector<int> &>())
            .def("vertices",
                 [&meta](arch::Surface &s) {
                     return meta.invoke_method(s, "vertices", {})
                         .as<std::vector<double>>();
                 })
            .def("triangles",
                 [&meta](arch::Surface &s) {
                     return meta.invoke_method(s, "triangles", {})
                         .as<std::vector<int>>();
                 })
            .def("nbTriangles",
                 [&meta](arch::Surface &s) {
                     return meta.invoke_method(s, "nbTriangles", {})
                         .as<size_t>();
                 })
            .def(
                "nbVertices",
                [&meta](arch::Surface &s) {
                    return meta.invoke_method(s, "nbVertices", {}).as<size_t>();
                })
            .def("setBcType",
                 [&meta](arch::Surface &s, const std::string &a,
                         const std::string &b) {
                     meta.invoke_method(s, "setBcType", {a, b});
                 })
            .def("setBcValue",
                 [&meta](arch::Surface &s, const std::vector<double> &v) {
                     return meta.invoke_method(s, "setBcValue", {v}).as<bool>();
                 })
            .def("displ", &arch::Surface::displ, py::arg("local") = true,
                 py::arg("atTriangles") = true)
            .def("displPlus", &arch::Surface::displPlus,
                 py::arg("local") = true, py::arg("atTriangles") = true,
                 py::arg("delta") = 1e-7)
            .def("displMinus", &arch::Surface::displMinus,
                 py::arg("local") = true, py::arg("atTriangles") = true,
                 py::arg("delta") = 1e-7);
    }

    // --- BaseRemote ---
    {
        py::class_<arch::BaseRemote>(m, "BaseRemote",
                                     "Base class for remote strain/stress")
            .def("strainAt",
                 [](arch::BaseRemote &self, const vector3 &p) {
                     auto mat = self.strainAt(arch::Vector3(p[0], p[1], p[2]));
                     return matrix33{mat(0, 0), mat(0, 1), mat(0, 2),
                                     mat(1, 1), mat(1, 2), mat(2, 2)};
                 })
            .def("stressAt",
                 [](arch::BaseRemote &self, const vector3 &p) {
                     auto mat = self.stressAt(arch::Vector3(p[0], p[1], p[2]));
                     return matrix33{mat(0, 0), mat(0, 1), mat(0, 2),
                                     mat(1, 1), mat(1, 2), mat(2, 2)};
                 })
            .def("traction", [](arch::BaseRemote &self, const vector3 &p,
                                const vector3 &n) {
                auto t = self.traction(arch::Vector3(p[0], p[1], p[2]),
                                       arch::Vector3(n[0], n[1], n[2]));
                return vector3{t[0], t[1], t[2]};
            });
    }

    // --- RemoteStress ---
    {
        auto &meta = ROSETTA_GET_META(arch::RemoteStress);
        py::class_<arch::RemoteStress, arch::BaseRemote>(
            m, "RemoteStress", "An Andersonian remote stress")
            .def(py::init<>())
            .def("seth",
                 [&meta](arch::RemoteStress &r, double v) {
                     meta.invoke_method(r, "seth", {v});
                 })
            .def("setH",
                 [&meta](arch::RemoteStress &r, double v) {
                     meta.invoke_method(r, "setH", {v});
                 })
            .def("setv",
                 [&meta](arch::RemoteStress &r, double v) {
                     meta.invoke_method(r, "setv", {v});
                 })
            .def("setTheta",
                 [&meta](arch::RemoteStress &r, double v) {
                     meta.invoke_method(r, "setTheta", {v});
                 })
            .def("__repr__", [](const arch::RemoteStress &r) {
                auto mat = r.stressAt(arch::Vector3(0, 0, 0));
                return "RemoteStress(xx=" + std::to_string(mat.get(0, 0)) +
                       ", xy=" + std::to_string(mat.get(0, 1)) +
                       ", xz=" + std::to_string(mat.get(0, 2)) +
                       ", yy=" + std::to_string(mat.get(1, 1)) +
                       ", yz=" + std::to_string(mat.get(1, 2)) +
                       ", zz=" + std::to_string(mat.get(2, 2)) + ")";
            });
    }

    // --- UserRemote ---
    {
        py::class_<arch::UserRemote, arch::BaseRemote>(
            m, "UserRemote", "User-defined remote stress/strain via lambda")
            .def(py::init([](std::function<std::array<double, 6>(double, double,
                                                                 double)>
                                 fct,
                             bool is_stress) {
                     auto wrapper = [fct](double x, double y,
                                          double z) -> arch::Matrix33 {
                         auto arr = fct(x, y, z);
                         return arch::Matrix33(arr[0], arr[1], arr[2], arr[3],
                                               arr[4], arr[5]);
                     };
                     return std::make_unique<arch::UserRemote>(wrapper,
                                                               is_stress);
                 }),
                 py::arg("fct"), py::arg("is_stress") = true);
    }

    // --- SeidelSolver ---
    {
        auto &meta = ROSETTA_GET_META(arch::SeidelSolver);
        py::class_<arch::SeidelSolver>(m, "SeidelSolver")
            .def(py::init<arch::Model &>())
            .def("setEps",
                 [&meta](arch::SeidelSolver &s, double v) {
                     meta.invoke_method(s, "setEps", {v});
                 })
            .def("setMaxIter",
                 [&meta](arch::SeidelSolver &s, int v) {
                     meta.invoke_method(s, "setMaxIter", {v});
                 })
            .def("onMessage", &arch::SeidelSolver::onMessage)
            .def("onWarning", &arch::SeidelSolver::onWarning)
            .def("onError", &arch::SeidelSolver::onError)
            .def("onProgress", &arch::SeidelSolver::onProgress)
            .def("stopRequested", &arch::SeidelSolver::stopRequested)
            .def("onEnd", &arch::SeidelSolver::onEnd)
            .def("run", [&meta](arch::SeidelSolver &s) {
                meta.invoke_method(s, "run", {});
            });
    }

    // --- ParallelSolver ---
    // {
    //     auto &meta = ROSETTA_GET_META(arch::ParallelSolver);
    //     py::class_<arch::ParallelSolver>(m, "ParallelSolver")
    //         .def(py::init<arch::Model &>())
    //         .def("setEps",
    //              [&meta](arch::ParallelSolver &s, double v) {
    //                  meta.invoke_method(s, "setEps", {v});
    //              })
    //         .def("setMaxIter",
    //              [&meta](arch::ParallelSolver &s, int v) {
    //                  meta.invoke_method(s, "setMaxIter", {v});
    //              })
    //         .def("setNbCores",
    //              [&meta](arch::ParallelSolver &s, int v) {
    //                  meta.invoke_method(s, "setNbCores", {v});
    //              })
    //         .def("onMessage", &arch::ParallelSolver::onMessage)
    //         .def("onWarning", &arch::ParallelSolver::onWarning)
    //         .def("onError", &arch::ParallelSolver::onError)
    //         .def("onProgress", &arch::ParallelSolver::onProgress)
    //         .def("stopRequested", &arch::ParallelSolver::stopRequested)
    //         .def("onEnd", &arch::ParallelSolver::onEnd)
    //         .def("run", [&meta](arch::ParallelSolver &s) {
    //             meta.invoke_method(s, "run", {});
    //         });
    // }

    // --- Postprocess (ParallelPostprocess) ---
    {
        auto &meta = ROSETTA_GET_META(arch::ParallelPostprocess);
        py::class_<arch::ParallelPostprocess>(m, "Postprocess")
            .def(py::init<arch::Model &>())
            .def("onEnd", &arch::ParallelPostprocess::onEnd)
            .def("onMessage", &arch::ParallelPostprocess::onMessage)
            .def("onWarning", &arch::ParallelPostprocess::onWarning)
            .def("onError", &arch::ParallelPostprocess::onError)
            .def("onProgress", &arch::ParallelPostprocess::onProgress)
            .def("stopRequested", &arch::ParallelPostprocess::stopRequested)
            .def("setNbCores",
                 [&meta](arch::ParallelPostprocess &p, int v) {
                     meta.invoke_method(p, "setNbCores", {v});
                 })
            .def("setDelta",
                 [&meta](arch::ParallelPostprocess &p, double v) {
                     meta.invoke_method(p, "setDelta", {v});
                 })
            .def("displAt",
                 [](arch::ParallelPostprocess &self, const vector3 &p) {
                     auto u = self.displAt(arch::Vector3(p[0], p[1], p[2]));
                     return vector3{u[0], u[1], u[2]};
                 })
            .def("strainAt",
                 [](arch::ParallelPostprocess &self, const vector3 &p) {
                     auto mat = self.strainAt(arch::Vector3(p[0], p[1], p[2]));
                     return matrix33{mat(0, 0), mat(0, 1), mat(0, 2),
                                     mat(1, 1), mat(1, 2), mat(2, 2)};
                 })
            .def("stressAt",
                 [](arch::ParallelPostprocess &self, const vector3 &p) {
                     auto mat = self.stressAt(arch::Vector3(p[0], p[1], p[2]));
                     return matrix33{mat(0, 0), mat(0, 1), mat(0, 2),
                                     mat(1, 1), mat(1, 2), mat(2, 2)};
                 })
            .def("displ", &arch::ParallelPostprocess::displ)
            .def("strain", &arch::ParallelPostprocess::strain)
            .def("stress", &arch::ParallelPostprocess::stress)
            .def("burgers", &arch::ParallelPostprocess::burgers,
                 py::arg("local") = true, py::arg("atTriangles") = true)
            .def("burgersPlus", &arch::ParallelPostprocess::burgersPlus,
                 py::arg("local") = true, py::arg("atTriangles") = true)
            .def("burgersMinus", &arch::ParallelPostprocess::burgersMinus,
                 py::arg("local") = true, py::arg("atTriangles") = true)
            .def("residualTractions",
                 &arch::ParallelPostprocess::residualTractions);
    }

    // ========================================================================
    // Utility: List all registered classes (introspection demo)
    // ========================================================================
    m.def(
        "list_classes",
        []() { return rosetta::Registry::instance().list_classes(); },
        "List all Rosetta-registered classes");

    m.def(
        "get_class_methods",
        [](const std::string &class_name) {
            auto *holder =
                rosetta::Registry::instance().get_by_name(class_name);
            if (!holder)
                throw std::runtime_error("Class not found: " + class_name);
            return holder->get_methods();
        },
        "Get all method names for a class");

    m.def(
        "get_class_fields",
        [](const std::string &class_name) {
            auto *holder =
                rosetta::Registry::instance().get_by_name(class_name);
            if (!holder)
                throw std::runtime_error("Class not found: " + class_name);
            return holder->get_fields();
        },
        "Get all field names for a class");
}

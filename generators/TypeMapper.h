#pragma once
#include <string>
#include <map>
#include "TypeInfo.h"

class TypeMapper {
  public:
    TypeMapper() {
        // Primitives
        add("void", "None", "undefined", "void", true, false);
        add("bool", "bool", "boolean", "boolean", true, false);
        add("int", "int", "number", "number", true, false);
        add("long", "int", "number", "number", true, false);
        add("size_t", "int", "number", "number", true, false);
        add("float", "float", "number", "number", true, false);
        add("double", "float", "number", "number", true, false);
        add("std::string", "str", "string", "string", true, false);

        // STL containers
        add("std::vector<double>", "numpy.ndarray", "Float64Array",
            "Float64Array", false, true);
        add("std::vector<int>", "numpy.ndarray", "Int32Array", "Int32Array",
            false, true);
        add("std::vector<float>", "numpy.ndarray", "Float32Array",
            "Float32Array", false, true);

        // Arch types
        add("arch::Vector3", "numpy.ndarray", "Float64Array", "Vector3", false,
            true);
        add("arch::Matrix33", "numpy.ndarray", "Float64Array", "Matrix33",
            false, true);

        // Arch classes (bound as objects)
        add("arch::Model", "Model", "Model", "Model", false, false);
        add("arch::Surface", "Surface", "Surface", "Surface", false, false);
        add("arch::BaseRemote", "BaseRemote", "BaseRemote", "BaseRemote", false,
            false);
        add("arch::RemoteStress", "RemoteStress", "RemoteStress",
            "RemoteStress", false, false);
        add("arch::UserRemote", "UserRemote", "UserRemote", "UserRemote", false,
            false);
        add("arch::SeidelSolver", "SeidelSolver", "SeidelSolver",
            "SeidelSolver", false, false);
        add("arch::ParallelPostprocess", "Postprocess", "Postprocess",
            "Postprocess", false, false);
    }

    const TypeInfo *get(const std::string &cpp_type) const {
        std::string normalized = normalize(cpp_type);
        auto it = types_.find(normalized);
        return it != types_.end() ? &it->second : nullptr;
    }

    std::string to_python(const std::string &cpp_type) const {
        auto *info = get(cpp_type);
        return info ? info->python_type : "Any";
    }

    std::string to_js(const std::string &cpp_type) const {
        auto *info = get(cpp_type);
        return info ? info->js_type : "any";
    }

    std::string to_ts(const std::string &cpp_type) const {
        auto *info = get(cpp_type);
        return info ? info->ts_type : "any";
    }

  private:
    std::map<std::string, TypeInfo> types_;

    void add(const std::string &cpp, const std::string &py,
             const std::string &js, const std::string &ts, bool primitive,
             bool convert) {
        types_[cpp] = {cpp, py, js, ts, primitive, convert};
    }

    std::string normalize(const std::string &type) const {
        std::string t = type;
        auto replace_all = [&](const std::string &from, const std::string &to) {
            size_t pos = 0;
            while ((pos = t.find(from, pos)) != std::string::npos) {
                t.replace(pos, from.length(), to);
            }
        };
        replace_all("const ", "");
        replace_all("&", "");
        replace_all(" *", "*");
        replace_all("*", "");
        t.erase(0, t.find_first_not_of(" "));
        if (!t.empty())
            t.erase(t.find_last_not_of(" ") + 1);
        return t;
    }
};
#pragma once
#include "TypeMapper.h"
#include "GeneratorConfig.h"
#include <fstream>
#include <string>

// ============================================================================
// Base Code Writer
// ============================================================================
class CodeWriter {
  protected:
    std::ostream &out_;
    int indent_ = 0;
    TypeMapper type_mapper_;
    GeneratorConfig config_;

    void line(const std::string &s = "") {
        out_ << std::string(indent_ * 4, ' ') << s << "\n";
    }

    void raw(const std::string &s) { out_ << s; }

    void indent() { indent_++; }
    void dedent() {
        if (indent_ > 0)
            indent_--;
    }

  public:
    explicit CodeWriter(std::ostream &out, const GeneratorConfig &config = {})
        : out_(out), config_(config) {}
    virtual ~CodeWriter() = default;
    virtual void generate() = 0;
};
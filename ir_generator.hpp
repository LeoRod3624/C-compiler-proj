#pragma once
#include "ir.hpp"
#include <vector>
#include <string>

class IRGenerator {
public:
    std::vector<IRInstr> instructions;

    void emit_assign(const std::string& dest, const std::string& value);
    void emit_binop(const std::string& dest, const std::string& lhs, const std::string& rhs, const std::string& op);
    void emit_load(const std::string& dest, const std::string& src_ptr);
    void emit_store(const std::string& dest_ptr, const std::string& value);
    void emit_return(const std::string& value);
    void emit_cmp(const std::string& dest, const std::string& lhs, const std::string& rhs, const std::string& op);

    void dump() const;
};

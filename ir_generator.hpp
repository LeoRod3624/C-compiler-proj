#pragma once
#include "ir.hpp"
#include <vector>
#include <string>

class NodeExpr;
class NodeStmt;

class IRGenerator {
public:
    std::vector<IRInstr> instructions;

    void emit_assign(const std::string& dest, const std::string& value);
    void emit_binop(const std::string& dest, const std::string& lhs, const std::string& rhs, const std::string& op);
    void emit_load(const std::string& dest, const std::string& src_ptr);
    void emit_store(const std::string& dest_ptr, const std::string& value);
    void emit_return(const std::string& value);
    void emit_cmp(const std::string& dest, const std::string& lhs, const std::string& rhs, const std::string& op);
    void emit_call(const std::string& result_var, const std::string& function_name, const std::vector<NodeExpr*>& args);
    void emit_label(const std::string& label);
    void emit_jump(const std::string& label);
    void emit_branch_if_zero(const std::string& cond_var, const std::string& label);

    std::string gen_sub(const std::string& lhs, const std::string& rhs);
    std::string gen_mul(const std::string& lhs, const std::string& rhs);
    std::string gen_div(const std::string& lhs, const std::string& rhs);

    void dump() const;
};

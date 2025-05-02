#include "ir_generator.hpp"
#include <iostream>
#include "leocc.hpp"

using namespace std;

static int tmp_counter = 0;

std::string new_tmp() {
    return "tmp_" + std::to_string(tmp_counter++);
}

void IRGenerator::emit_assign(const std::string& dest, const std::string& value) {
    instructions.push_back({IRKind::Assign, dest, value, "", ""});
}

void IRGenerator::emit_binop(const std::string& dest, const std::string& lhs, const std::string& rhs, const std::string& op) {
    instructions.push_back({IRKind::BinOp, dest, lhs, rhs, op});
}

void IRGenerator::emit_load(const std::string& dest, const std::string& src_ptr) {
    instructions.push_back({IRKind::Load, dest, src_ptr, "", ""});
}

void IRGenerator::emit_store(const std::string& dest_ptr, const std::string& value) {
    instructions.push_back({IRKind::Store, dest_ptr, value, "", ""});
}

void IRGenerator::emit_return(const std::string& value) {
    instructions.push_back({IRKind::Return, "", value, "", ""});
}

void IRGenerator::emit_cmp(const std::string& dest, const std::string& lhs, const std::string& rhs, const std::string& op) {
    instructions.push_back({IRKind::Cmp, dest, lhs, rhs, op});
}

void IRGenerator::dump() const {
    for (const auto& instr : instructions) {
        instr.print(std::cerr);
    }
}

std::string IRGenerator::gen_sub(const std::string& lhs, const std::string& rhs) {
    std::string tmp = new_tmp();
    emit_binop(tmp, lhs, rhs, "-");
    return tmp;
}

std::string IRGenerator::gen_mul(const std::string& lhs, const std::string& rhs) {
    std::string tmp = new_tmp();
    emit_binop(tmp, lhs, rhs, "*");
    return tmp;
}

std::string IRGenerator::gen_div(const std::string& lhs, const std::string& rhs) {
    std::string tmp = new_tmp();
    emit_binop(tmp, lhs, rhs, "/");
    return tmp;
}

void IRGenerator::emit_call(const string& result_var, const string& function_name, const vector<NodeExpr*>& args) {
    string args_list;
    for (size_t i = 0; i < args.size(); ++i) {
        if (i > 0) args_list += ", ";
        args_list += args[i]->result_var;
    }
    instructions.push_back({IRKind::Call, result_var, function_name, "", args_list});
}

void IRGenerator::emit_label(const std::string& label) {
    instructions.push_back({IRKind::Label, label, "", "", ""});
}

void IRGenerator::emit_jump(const std::string& label) {
    instructions.push_back({IRKind::Jump, label, "", "", ""});
}

void IRGenerator::emit_branch_if_zero(const std::string& cond_var, const std::string& label) {
    instructions.push_back({IRKind::BranchIfZero, label, cond_var, "", ""});
}


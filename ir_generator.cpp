#include "ir_generator.hpp"
#include <iostream>
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

// ✨ Correct gen_sub / gen_mul / gen_div:
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

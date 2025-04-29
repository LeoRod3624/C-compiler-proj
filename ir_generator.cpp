#include "ir_generator.hpp"
#include <iostream>
using namespace std;

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

void IRGenerator::dump() const {
    for (const auto& instr : instructions) {
        instr.print(std::cerr);  
    }
}

void IRGenerator::emit_cmp(const string& dest, const string& lhs, const string& rhs, const string& op) {
    instructions.push_back({IRKind::Cmp, dest, lhs, rhs, op});
}


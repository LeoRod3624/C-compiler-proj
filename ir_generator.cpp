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
    cout << result_var << " = call " << function_name << "(" << args_list << ");" << endl;
}

// 🔽 Add this to your NodeIfStmt::emit_ir in leocc.cpp or ast.cpp:
void NodeIfStmt::emit_ir(IRGenerator& ir) {
    condition->emit_ir(ir);
    std::string cond_var = condition->result_var;
    std::string else_label = "L_else_" + std::to_string(tmp_counter++);
    std::string end_label = "L_end_" + std::to_string(tmp_counter++);

    // Emit conditional jump instruction (simulated as comment here)
    cerr << "if (" << cond_var << " == 0) goto " << else_label << ";" << endl;

    then_branch->emit_ir(ir);
    cerr << "goto " << end_label << ";" << endl;

    cerr << else_label << ":" << endl;
    if (else_branch) {
        else_branch->emit_ir(ir);
    }

    cerr << end_label << ":" << endl;
}

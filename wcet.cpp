#include "ir_generator.hpp"
#include "ir.hpp"
#include "leocc.hpp"
#include <iostream>
#include <map>

int compute_instr_cost(IRKind kind) {
    switch (kind) {
        case IRKind::Assign:
        case IRKind::BinOp:
        case IRKind::Cmp:
        case IRKind::Load:
        case IRKind::Store:
        case IRKind::Return:
            return 1;
        case IRKind::Jump:
        case IRKind::BranchIfZero:
            return 2;
        case IRKind::Call:
            return 5;
        case IRKind::Label:
            return 0;
    }
    return 0;
}

int extract_loop_bound(Node* root, const std::string& loop_id) {
    if (auto* prog = dynamic_cast<NodeProgram*>(root)) {
        for (auto* fn : prog->func_defs) {
            if (!fn->body) continue;
            for (auto* stmt : fn->body->stmt_list) {
                if (auto* while_stmt = dynamic_cast<NodeWhileStmt*>(stmt)) {
                    auto* cmp = dynamic_cast<NodeBinOp*>(while_stmt->_expr);
                    if (!cmp || cmp->punct != "<") continue;
                    auto* rhs = dynamic_cast<NodeNum*>(cmp->rhs);
                    if (rhs) return rhs->num_literal;
                }
            }
        }
    }
    return 1;
}

int analyze_loop(const std::vector<IRInstr>& instrs,
                 const std::map<std::string, size_t>& label_map,
                 size_t loop_start_idx, Node* root) {
    std::string loop_id = instrs[loop_start_idx].dest.substr(instrs[loop_start_idx].dest.find_last_of('_') + 1);
    std::string after_label = instrs[loop_start_idx].dest.find("while") != std::string::npos
                              ? "L_while_after_" + loop_id
                              : "L_for_after_" + loop_id;

    if (!label_map.count(after_label)) return 0;

    size_t loop_end = label_map.at(after_label);
    int body_cost = 0;

    for (size_t i = loop_start_idx + 1; i < loop_end; ++i) {
        const auto& instr = instrs[i];
        if (instr.kind == IRKind::Label &&
           (instr.dest.find("L_while_loop_") != std::string::npos ||
            instr.dest.find("L_for_cond_") != std::string::npos)) {
            int inner_cost = analyze_loop(instrs, label_map, i, root);
            std::string inner_id = instr.dest.substr(instr.dest.find_last_of('_') + 1);
            std::string inner_after = instr.dest.find("while") != std::string::npos
                                      ? "L_while_after_" + inner_id
                                      : "L_for_after_" + inner_id;
            i = label_map.at(inner_after);
            body_cost += inner_cost;
        } else {
            body_cost += compute_instr_cost(instr.kind);
        }
    }

    int bound = extract_loop_bound(root, loop_id);
    return bound * body_cost;
}

int analyze_wcet(Node* root, const IRGenerator& ir) {
    const auto& instrs = ir.instructions;
    std::map<std::string, size_t> label_map;
    for (size_t i = 0; i < instrs.size(); ++i) {
        if (instrs[i].kind == IRKind::Label) {
            label_map[instrs[i].dest] = i;
        }
    }

    int total_cycles = 0;
    for (size_t i = 0; i < instrs.size(); ++i) {
        const auto& instr = instrs[i];
        if (instr.kind == IRKind::Label &&
            (instr.dest.find("L_while_loop_") != std::string::npos ||
             instr.dest.find("L_for_cond_") != std::string::npos)) {
            total_cycles += analyze_loop(instrs, label_map, i, root);
            std::string id = instr.dest.substr(instr.dest.find_last_of('_') + 1);
            std::string after_label = (instr.dest.find("while") != std::string::npos)
                                        ? "L_while_after_" + id
                                        : "L_for_after_" + id;
            i = label_map.at(after_label);
        } else {
            total_cycles += compute_instr_cost(instr.kind);
        }
    }

    std::cerr << "[WCET] Cycles: " << total_cycles << std::endl;  // <-- Restore this line
    return total_cycles;
}

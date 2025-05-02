#pragma once
#include <string>
#include <vector>
#include <iostream>

enum class IRKind {
    Assign,
    Load,
    Store,
    BinOp,
    Return,
    Cmp,
    BranchIfZero,
    Jump,
    Label,
    Call
};

struct IRInstr {
    IRKind kind;
    std::string dest;// Target variable or label
    std::string op1;// LHS or value (or condition variable)
    std::string op2;// RHS (used in binops and comparisons)
    std::string binop;// Operator symbol or arg list (for Call)

    void print(std::ostream& os) const {
        switch (kind) {
            case IRKind::Assign:
                os << dest << " = " << op1 << ";\n";
                break;
            case IRKind::Load:
                os << dest << " = *" << op1 << ";\n";
                break;
            case IRKind::Store:
                os << "*" << dest << " = " << op1 << ";\n";
                break;
            case IRKind::BinOp:
                os << dest << " = " << op1 << " " << binop << " " << op2 << ";\n";
                break;
            case IRKind::Cmp:
                os << dest << " = (" << op1 << " " << binop << " " << op2 << ");\n";
                break;
            case IRKind::Return:
                os << "return " << op1 << ";\n";
                break;
            case IRKind::BranchIfZero:
                os << "if (" << op1 << " == 0) goto " << dest << ";\n";
                break;
            case IRKind::Jump:
                os << "goto " << dest << ";\n";
                break;
            case IRKind::Label:
                os << dest << ":\n";
                break;
            case IRKind::Call:
                os << dest << " = call " << op1 << "(\"" << binop << "\");\n";

                break;
        }
    }
};

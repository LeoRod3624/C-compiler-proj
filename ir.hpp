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
    Cmp  // ← Added Cmp kind
};

struct IRInstr {
    IRKind kind;
    std::string dest;
    std::string op1;
    std::string op2;
    std::string binop;

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
        }
    }
};

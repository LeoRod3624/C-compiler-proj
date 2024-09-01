#include "leocc.hpp"
#include <cassert>

void NodeNum::codegen() {
    cout << "  movl $" << num_literal << ", %eax" <<  endl;
}

void NodeAdd::codegen() {
    ;
}

void do_codegen(Node* _expr) {
    cout << ".global main" << endl;
    cout << "main:" << endl;

    _expr->codegen();
}
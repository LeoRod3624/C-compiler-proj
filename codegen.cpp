#include "leocc.hpp"
#include <cassert>

void NodeNum::codegen() {
    cout << "  mov x0, " << num_literal <<  endl;
}

void NodeAdd::codegen() {
    lhs->codegen();
    cout << "  str x0, [sp, -16]!" << endl;
    rhs->codegen();
    cout << "  ldr x1, [sp], 16" << endl;
    cout << "  add x0, x0, x1" << endl;
}

void NodeSub::codegen(){
    lhs->codegen();
    cout << "  str x0, [sp, -16]!" << endl;
    rhs->codegen();
    cout << "  ldr x1, [sp], 16" << endl;
    cout << "  sub x0, x1, x0" << endl;
}

void NodeMul::codegen(){
    lhs->codegen();
    cout << "  str x0, [sp, -16]!" << endl;
    rhs->codegen();
    cout << "  ldr x1, [sp], 16" << endl;
    cout << "  mul x0, x1, x0" << endl;
}

void NodeDiv::codegen(){
    lhs->codegen();
    cout << "  str x0, [sp, -16]!" << endl;
    rhs->codegen();
    cout << "  ldr x1, [sp], 16" << endl;
    cout << "  sdiv x0, x1, x0" << endl;
}

void do_codegen(Node* _expr) {
    cout << ".global main" << endl;
    cout << "main:" << endl;

    _expr->codegen();
}
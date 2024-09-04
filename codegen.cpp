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

void NodeLT::codegen(){
    lhs->codegen();
    cout << "  str x0, [sp, -16]!" << endl;
    rhs->codegen();
    cout << "  ldr x1, [sp], 16" << endl;
    cout << "  subs x0, x1, x0" << endl;    
    cout << "  cset x0, lt" << endl;
    cout << "  and x0, x0, #0x1" << endl;
    ;
}

void NodeGT::codegen(){
    lhs->codegen();
    cout << "  str x0, [sp, -16]!" << endl;
    rhs->codegen();
    cout << "  ldr x1, [sp], 16" << endl;
    cout << "  subs x0, x1, x0" << endl;    
    cout << "  cset x0, gt" << endl;
    cout << "  and x0, x0, #0x1" << endl;
    ;
}

void NodeLTE::codegen(){
    lhs->codegen();
    cout << "  str x0, [sp, -16]!" << endl;
    rhs->codegen();
    cout << "  ldr x1, [sp], 16" << endl;
    cout << "  subs x0, x1, x0" << endl;    
    cout << "  cset x0, le" << endl;
    cout << "  and x0, x0, #0x1" << endl;
}

void NodeGTE::codegen(){
    lhs->codegen();
    cout << "  str x0, [sp, -16]!" << endl;
    rhs->codegen();
    cout << "  ldr x1, [sp], 16" << endl;
    cout << "  subs x0, x1, x0" << endl;    
    cout << "  cset x0, ge" << endl;
    cout << "  and x0, x0, #0x1" << endl;
}

void NodeEE::codegen(){
    lhs->codegen();
    cout << "  str x0, [sp, -16]!" << endl;
    rhs->codegen();
    cout << "  ldr x1, [sp], 16" << endl;
    cout << "  subs x0, x1, x0" << endl;    
    cout << "  cset x0, eq" << endl;
    cout << "  and x0, x0, #0x1" << endl;
    
}

void NodeNE::codegen(){
    lhs->codegen();
    cout << "  str x0, [sp, -16]!" << endl;
    rhs->codegen();
    cout << "  ldr x1, [sp], 16" << endl;
    cout << "  subs x0, x1, x0" << endl;    
    cout << "  cset x0, ne" << endl;
    cout << "  and x0, x0, #0x1" << endl;
    
}

void NodeProgram::codegen(){
    for(NodeStmt* stmt : stmts) {
        stmt->codegen();
    }
}

void NodeExprStmt::codegen(){
    _expr->codegen();
}

void do_codegen(Node* root) {
    cout << ".global main" << endl;
    cout << "main:" << endl;

    root->codegen();
}
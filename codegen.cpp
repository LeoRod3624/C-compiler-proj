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

void NodeAssign::codegen(){
    rhs->codegen();
    cout << "  str x0, [sp, -16]!" << endl;
    (lhs)->codegen();
    cout << "  ldr x1, [sp], 16" << endl;
    cout << "  str x1, [x0]" << endl;
    cout << "  mov x0, x1" << endl;
}

int round16(int n){
    if((n % 16) != 0){
        return (16-(n%16)) + n;
    }
    return n;
}

void NodeId::codegen() {
    
    if(parent && parent->is_NodeAssign()){
        int byte_number = var_map[id]->offSet;
        cout << "  add x0, fp, -" << byte_number << endl;
    }
    else{
        int byte_number = var_map[id]->offSet;
        cout << "  add x0, fp, -" << byte_number << endl;
        cout << "  ldr x0, [x0]" << endl;
    }
}

void NodeReturnStmt::codegen() {
    _expr->codegen();
    cout << "  b .L.return" << endl;
}

void NodeBlockStmt::codegen(){
    for(NodeStmt* stmt : stmt_list) {
        stmt->codegen();
    }
}

void NodeNullStmt::codegen(){
    ;
}

static void emit_prologue() {
    cout << "  stp x29, x30, [sp, -16]!" << endl;
    cout << "  mov x29, sp" << endl;
    int stackSize = round16(var_map.size()*8);
    cout << "  sub sp, sp, #" << stackSize << endl;
}

static void emit_epilogue() {
    cout << ".L.return:" << endl;
    cout << "  add sp, sp, #" << round16(var_map.size()*8) << endl;
    cout << "  ldp x29, x30, [sp], 16" << endl;
    cout << "  ret" << endl;
}

void do_codegen(Node* root) {
    cout << ".global main" << endl;
    cout << "main:" << endl;

    emit_prologue();
    root->codegen();
    emit_epilogue();
}
#include "leocc.hpp"
#include <cassert>

static void codegen(Node* _expr) {
    if(_expr->kind == ND_ADD) {
        
    }
    else if(_expr->kind == ND_SUB) {

    }
    else if(_expr->kind == ND_NUM) {

    }
    else {
        assert(false && "unreachable");
    }
}

void do_codegen(Node* _expr) {
    cout << ".global main" << endl;
    cout << "main:" << endl;

    codegen(_expr);
}
#include "leocc.hpp"
#include <iostream>
#include <cassert>

using namespace std;

// grammar without any tricks like * lists or inline ORs like (rule1 | rule2)

// expr = mul mul_list
// mul = primary primary_list
// mul_list =  "" 
//           | "+" mul mul_list 
//           | "-" mul mul_list
// primary = "(" expr ")"
//         | num
// primary_list = ""
//              | "*" primary primary_list
//              | "/" primary primary_list


void CMulList::print_cst(int depth){
    for(int i = 0; i < depth; i++){
        cout << " ";
    }
    cout << "|-mul_list" << endl;
    if(plus_or_minus == nullptr){
        string spaces = "";
        for(int i = 0; i < depth+1; i++){
            spaces += " ";
            
        }
        cout << spaces << "|-NULL" << endl;
        cout << spaces << "|-NULL" << endl;
        cout << spaces << "|-NULL" << endl;
    }
    else{
        plus_or_minus->print_cst(depth+1);
        mul->print_cst(depth+1);
        mul_list->print_cst(depth+1);
    }

}
void CPunct::print_cst(int depth){
    for(int i = 0; i < depth; i++){
        cout << " ";
    }
    cout << "|-" << punct << endl;
}

void CMul::print_cst(int depth){
    for(int i = 0; i < depth; i++){
        cout << " ";
    }
    cout << "|-mul" << endl;
    primary->print_cst(depth+1);
    primary_list->print_cst(depth+1);
}

void CPrimaryList::print_cst(int depth){
    for(int i = 0; i < depth; i++){
        cout << " ";
    }
    cout << "|-primary_list" << endl;
    
    if(times_or_divides == nullptr){
        string spaces = "";
        for(int i = 0; i < depth+1; i++){
            spaces += " ";
        }
        cout << spaces << "|-NULL" << endl;
        cout << spaces << "|-NULL" << endl;
        cout << spaces << "|-NULL" << endl;
    }
    else{
        times_or_divides->print_cst(depth+1);
        primary->print_cst(depth+1);
        primary_list->print_cst(depth+1);
    }
    
}

void CExpr::print_cst(int depth) {
    for(int i = 0; i < depth; i++){
        cout << " ";
    }
    cout << "|-expr" << endl;
    mul->print_cst(depth+1);
    mul_list->print_cst(depth+1);
}

void CPrimary::print_cst(int depth){
    for(int i = 0; i < depth; i++){
        cout << " ";
    }
    cout << "|-primary" << endl;
    if(num != nullptr){
        num->print_cst(depth+1);
    }
    else{
        assert(leftParenthesis && "SHOULD NOT BE NULL");
        leftParenthesis->print_cst(depth+1);
        expr->print_cst(depth+1);
        rightParenthesis->print_cst(depth+1);
    }
    ;
}

void CNum::print_cst(int depth){
    for(int i = 0; i < depth; i++){
        cout << " ";
    }
    cout << "|-num" << endl;
    for(int i = 0; i < depth+1; i++){
        cout << " ";
    }
    cout << "|-" << num_literal << endl;
    
}

CNum* C_num(){
    CNum* result = new CNum();
    
    assert(tokens[tokens_i]->kind == TK_NUM && "WRONG SOMETHING WENT WRONG, NEED NUMBER");
    
    result->num_literal = tokens[tokens_i++]->num;
    return result;
}

CExpr* C_expr();

CPrimary* C_primary(){
    CPrimary* result = new CPrimary();
    if(tokens[tokens_i]->kind == TK_NUM){
        result->num = C_num();
    }
    else{
        assert(tokens[tokens_i]->punct == "(" && "SHOULD BE '(', SOMETHING WENT WRONG");
        result->leftParenthesis = new CPunct();
        result->leftParenthesis->punct = tokens[tokens_i++]->punct;
        result->expr = C_expr();
        assert(tokens[tokens_i]->punct == ")" && "SHOULD BE ')', SOMETHING WENT WRONG");
        result->rightParenthesis = new CPunct();
        result->rightParenthesis->punct = tokens[tokens_i++]->punct;
    }
    return result;
};

// num_list = "" 
//          | "*" num num_list 
//          | "/" num num_list
CPrimaryList* C_primary_list(){
    // since our test case is 1+2+3, and doesn't use
    // "*" or "/", we can implement the rest later.
    CPrimaryList* result = new CPrimaryList();

    if(tokens[tokens_i]->punct == "*" || tokens[tokens_i]->punct == "/"){
        if(tokens[tokens_i]->punct == "*"){
            CPunct* times = new CPunct();
            times->punct = tokens[tokens_i++]->punct;

            result->times_or_divides = times;
            result->primary = C_primary();
            result->primary_list = C_primary_list();        
        }
        else{
            CPunct* divide = new CPunct();
            divide->punct = tokens[tokens_i++]->punct;

            result->times_or_divides = divide;
            result->primary = C_primary();
            result->primary_list = C_primary_list();
        }
    }

    return result;
}

CMul* C_mul() {
    CMul* result = new CMul();
    result->primary = C_primary();
    result->primary_list = C_primary_list();
    return result;
}

CMulList* C_mul_list() {
    CMulList* result = new CMulList();
    if(tokens[tokens_i]->punct == "+" || tokens[tokens_i]->punct == "-"){
        if(tokens[tokens_i]->punct == "+"){
            CPunct* plus = new CPunct();
            plus->punct = tokens[tokens_i++]->punct;

            result->plus_or_minus = plus;
            result->mul = C_mul();
            result->mul_list = C_mul_list();        
        }
        else{
            CPunct* minus = new CPunct();
            minus->punct = tokens[tokens_i++]->punct;

            result->plus_or_minus = minus;
            result->mul = C_mul();
            result->mul_list = C_mul_list();
        }
    }
    //finish
    
    return result;
}

CExpr* C_expr() {
    CExpr* result = new CExpr();
    result->mul = C_mul();
    result->mul_list = C_mul_list();
    return result;
}

CNode* concrete_parse() {
    CExpr* expr = C_expr();
    return expr;
}
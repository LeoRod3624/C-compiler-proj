#include "leocc.hpp"
#include <cassert>

// Current CFG, with regex:
// expr = mul ("+" mul | "-" mul)*
// mul = primary ("*" primary | "/" primary)*
// primary = "(" expr ")" | num
NodeExpr* expr();
NodeExpr* primary();
NodeExpr* mul();
NodeNum* num();

NodeNum* num() {
    assert(tokens[tokens_i]->kind == TK_NUM && "NOT A NUMBER, MUST BE A NUMBER");
    NodeNum* result = new NodeNum();
    result->num_literal = tokens[tokens_i++]->num;
    return result;
}

NodeExpr* primary() {
    if(tokens[tokens_i]->kind == TK_NUM){
        return num();
    }
    else if(tokens[tokens_i]->kind == TK_PUNCT && tokens[tokens_i]->punct == "("){
        tokens_i++;
        NodeExpr* _expr = expr();
        assert(tokens[tokens_i]->kind == TK_PUNCT && "HAS TO BE a punct");
        assert(tokens[tokens_i]->punct == ")" && "MUST BE CLOSING PARENTHESIS");
        tokens_i++;
        return _expr;
    }
    else{
        cout << "entered primary function call but didn't choose any rule." << endl;
        exit(1);
    }
}

NodeExpr* mul() {
    NodeExpr* result = primary();

    while(tokens[tokens_i]->kind == TK_PUNCT && (tokens[tokens_i]->punct == "*" || tokens[tokens_i]->punct == "/")){
        if(tokens[tokens_i]->punct == "*"){
            NodeMul* current_mul = new NodeMul();
            current_mul->lhs = result;
            current_mul->punct = tokens[tokens_i++]->punct;
            current_mul->rhs = primary();
            result = current_mul;
        }
        else{
            assert(tokens[tokens_i]->punct == "/");
            NodeDiv* current_div = new NodeDiv();
            current_div->lhs = result;
            current_div->punct = tokens[tokens_i++]->punct;
            current_div->rhs = primary();
            result = current_div;
        }
    }
    return result;
}
NodeExpr* expr() {
    NodeExpr* result = mul();

    while(tokens[tokens_i]->kind == TK_PUNCT && (tokens[tokens_i]->punct == "+" || tokens[tokens_i]->punct == "-")){
        if(tokens[tokens_i]->punct == "+"){
            NodeAdd* current_add = new NodeAdd();
            current_add->lhs = result;
            current_add->punct = tokens[tokens_i++]->punct;
            current_add->rhs = mul();
            result = current_add;
        }
        else{
            assert(tokens[tokens_i]->punct == "-");
            NodeSub* current_sub = new NodeSub();
            current_sub->lhs = result;
            current_sub->punct = tokens[tokens_i++]->punct;
            current_sub->rhs = mul();
            result = current_sub;
            //call Sub?
            ;
        }
    }
    return result;
}

Node* abstract_parse() {
    return expr();
}
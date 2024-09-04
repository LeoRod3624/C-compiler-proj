#include "leocc.hpp"
#include <cassert>  
/* 
Current CFG, with regex:
expr = equality
equality = relational ("==" relational | "!=" relational)*
relational = add ("<" add | "<=" add | ">" add | ">=" add)*
add = mul ("+" mul | "-" mul)*
mul = unary ("*" unary | "/" unary)*
unary = ("+" | "-") unary | primary
primary = "(" expr ")" | num
*/
NodeExpr* expr();
NodeExpr* primary();
NodeExpr* mul();
NodeNum* num();
NodeExpr* unary();
NodeExpr* equality();
NodeExpr* relational();
NodeExpr* add();

NodeExpr* expr(){
    return equality();
}

NodeExpr* equality(){
    NodeExpr* result = relational();
    while( tokens[tokens_i]->kind == TK_PUNCT &&
        (tokens[tokens_i]->punct == "==" || tokens[tokens_i]->punct == "!=")){
            if(tokens[tokens_i]->punct == "=="){
                NodeEE* ee = new NodeEE();
                ee->lhs = result;
                tokens_i++;
                ee->rhs = relational();
                result = ee;
            }
            else{
                NodeNE* ne = new NodeNE();
                ne->lhs = result;
                tokens_i++;
                ne->rhs = relational();
                result = ne;;
            }
    }
    return result;
}

NodeExpr* relational(){
    NodeExpr* result = add();
    while( tokens[tokens_i]->kind == TK_PUNCT &&
        (tokens[tokens_i]->punct == "<" || tokens[tokens_i]->punct == ">" || tokens[tokens_i]->punct == ">=" || tokens[tokens_i]->punct == "<=")){
            if(tokens[tokens_i]->punct == "<"){
                NodeLT* lt = new NodeLT();
                lt->lhs = result;
                tokens_i++;
                lt->rhs = add();
                result = lt;
            }
            else if(tokens[tokens_i]->punct == ">"){
                NodeGT* gt = new NodeGT();
                gt->lhs = result;
                tokens_i++;
                gt->rhs = add();
                result = gt;
            }
            else if(tokens[tokens_i]->punct == "<="){
                NodeLTE* lte = new NodeLTE();
                lte->lhs = result;
                tokens_i++;
                lte->rhs = add();
                result = lte;
            }
            else if(tokens[tokens_i]->punct == ">="){
                NodeGTE* gte = new NodeGTE();
                gte->lhs = result;
                tokens_i++;
                gte->rhs = add();
                result = gte;
            }
        }

    return result;
}

NodeExpr* unary(){
    if( tokens[tokens_i]->kind == TK_PUNCT &&
        (tokens[tokens_i]->punct == "+" || tokens[tokens_i]->punct == "-")){
        if(tokens[tokens_i]->punct == "+"){
            tokens_i++;
            return unary();
        }
        else{
            assert(tokens[tokens_i]->punct == "-");
            tokens_i++;
            
            NodeMul* result = new NodeMul();
            NodeNum* minus_one = new NodeNum();
            minus_one->num_literal = -1;
            result->lhs = minus_one;
            result->rhs = unary();
            return result;
            
            
        }
    }
    return primary();
}

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
// mul = unary ("*" unary | "/" unary)*     //CHANGED
NodeExpr* mul() {
    NodeExpr* result = unary();

    while(tokens[tokens_i]->kind == TK_PUNCT && (tokens[tokens_i]->punct == "*" || tokens[tokens_i]->punct == "/")){
        if(tokens[tokens_i]->punct == "*"){
            NodeMul* current_mul = new NodeMul();
            current_mul->lhs = result;
            current_mul->punct = tokens[tokens_i++]->punct;
            current_mul->rhs = unary();
            result = current_mul;
        }
        else{
            assert(tokens[tokens_i]->punct == "/");
            NodeDiv* current_div = new NodeDiv();
            current_div->lhs = result;
            current_div->punct = tokens[tokens_i++]->punct;
            current_div->rhs = unary();
            result = current_div;
        }
    }
    return result;
}
NodeExpr* add() {
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
#include "leocc.hpp"
#include <cassert>
// Current CFG, with regex:
// expr = mul ("+" mul | "-" mul)*
// mul = primary ("*" primary | "/" primary)*
// primary = "(" expr ")" | num
//   |
// "1"
// -> expr 
// -> mul ("+" mul | "-" mul)* 
// -> primary ("*" primary | "/" primary)* ("+" mul | "-" mul)* 
// -> num ("*" primary | "/" primary)* ("+" mul | "-" mul)* 
// -> 1 ("*" primary | "/" primary)* ("+" mul | "-" mul)* 
// -> 1 ("+" mul | "-" mul)* 
// -> 1 
//     |
// "1+2"
// -> expr 
// -> mul ("+" mul | "-" mul)* 
// -> primary ("*" primary | "/" primary)* ("+" mul | "-" mul)* 
// -> num ("*" primary | "/" primary)* ("+" mul | "-" mul)* 
// -> 1 ("*" primary | "/" primary)* ("+" mul | "-" mul)* 
// -> 1 ("+" mul | "-" mul)* 
// -> 1 "+" mul ("+" mul | "-" mul)* 
// -> 1 "+" primary ("*" primary | "/" primary)* ("+" mul | "-" mul)* 
// -> 1 "+" num ("*" primary | "/" primary)* ("+" mul | "-" mul)* 
// -> 1 "+" 2 ("*" primary | "/" primary)* ("+" mul | "-" mul)* 
// -> 1 "+" 2 ("+" mul | "-" mul)* 
// -> 1 "+" 2
//       |
// "1+2+3"
// -> expr 
// -> mul ("+" mul | "-" mul)* 
// -> primary ("*" primary | "/" primary)* ("+" mul | "-" mul)* 
// -> num ("*" primary | "/" primary)* ("+" mul | "-" mul)* 
// -> 1 ("*" primary | "/" primary)* ("+" mul | "-" mul)* 
// -> 1 ("+" mul | "-" mul)* 
// -> 1 "+" mul ("+" mul | "-" mul)* 
// -> 1 "+" primary ("*" primary | "/" primary)* ("+" mul | "-" mul)* 
// -> 1 "+" num ("*" primary | "/" primary)* ("+" mul | "-" mul)* 
// -> 1 "+" 2 ("*" primary | "/" primary)* ("+" mul | "-" mul)* 
// -> 1 "+" 2 ("+" mul | "-" mul)* 
// -> 1 "+" 2 "+" mul ("+" mul | "-" mul)* 
// -> 1 "+" 2 "+" primary ("*" primary | "/" primary)* ("+" mul | "-" mul)* 
// -> 1 "+" 2 "+" num ("*" primary | "/" primary)* ("+" mul | "-" mul)* 
// -> 1 "+" 2 "+" 3 ("*" primary | "/" primary)* ("+" mul | "-" mul)* 
// -> 1 "+" 2 "+" 3 ("+" mul | "-" mul)* 
// -> 1 "+" 2 "+" 3
// Current CFG, with regex:
// expr = mul ("+" mul | "-" mul)*
// mul = primary ("*" primary | "/" primary)*
// primary = "(" expr ")" | num
NodeExpr* expr();
NodeExpr* primary();
NodeExpr* mul();
NodeNum* num();
// "1"
// -> expr 
// -> mul ("+" mul | "-" mul)* 
// -> primary ("*" primary | "/" primary)* ("+" mul | "-" mul)* 
// -> num ("*" primary | "/" primary)* ("+" mul | "-" mul)* 
// -> 1 ("*" primary | "/" primary)* ("+" mul | "-" mul)* 
// -> 1 ("+" mul | "-" mul)* 
// -> 1 

NodeNum* num() {
    assert(tokens[tokens_i]->kind == TK_NUM && "NOT A NUMBER, MUST BE A NUMBER");
    NodeNum* result = new NodeNum();
    result->num_literal = tokens[tokens_i++]->num;
    return result;
}
// primary = "(" expr ")" | num
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
    //still need to implement the other rule 
    // "(" expr ")"
}

// mul = primary ("*" primary | "/" primary)*
NodeExpr* mul() {
    return primary();
    // still need to implement 
    // ("*" primary | "/" primary)*
    // part of the rule
}
// expr = mul ("+" mul | "-" mul)*
NodeExpr* expr() {
    return mul();
    // still need to implement 
    // ("+" mul | "-" mul)* 
    // part of the rule
}

Node* abstract_parse() {
    return expr();
}
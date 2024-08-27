#include "leocc.hpp"
#include <cassert>
// CFG in ebnf
// expr = mul ("+" mul | "-" mul)*
// mul = primary ("*" primary | "/" primary)*
// primary = "(" expr ")" | num

Node* abstract_parse();
static Node* expr();
static Node* primary();
static Node* num();

Node* abstract_parse() {
    return expr();
}

Node* expr() {
    Node* _expr = primary();

    while(current_tok->kind != TK_EOF) {
        assert(current_tok->kind == TK_PUNCT);

        if(current_tok->punct == "+") {
            current_tok = tokens[++tokens_i];
            Node* _prim = primary();

            _expr = new Node(ND_ADD, _expr, _prim);
        }
        else if(current_tok->punct == "-") {
            current_tok = tokens[++tokens_i];
            Node* _prim = primary();

            _expr = new Node(ND_SUB, _expr, _prim);
        }
        else {
            assert(false && "unreachable");
        }
    }


    assert(current_tok->kind == TK_EOF);
    return _expr;
}

Node* primary() {
    return num();
}

Node* num() {
    assert(current_tok->kind == TK_NUM);
    int n = current_tok->num;
    current_tok = tokens[++tokens_i];
    return new Node(ND_NUM, n);
}

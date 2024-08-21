#include <iostream>
#include <fstream>
#include <vector>
#include <sstream>
#include <cassert>
#include <list>
#include "cst.hpp"
#include "main.hpp"

using namespace std;

/*
int main() {
    int a;
    return a + 5;
}

<kw: int>
<id: main>
<punct: (>
<punct: )>
<punct: {>
<kw: main> 
<id: a>
<kw: return>
<id: a>
<punct: +>
<num: 5>
*/

// 42
// 42 + 15 - 10
// [
// <num: 42>, 
// <punct: +>,
// <num: 15>,
// <punct: ->,
// <num: 10>
// ]

// expr = mul ("+" mul | "-" mul)*
// mul  = num ("*" num | "/" num)*

// expr = mul optional_mul_list
// optional_mul_list =  "" 
//                    | "+" mul optional_mul_list 
//                    | "-" mul optional_mul_list
// mul = num optional_num_list
// optional_num_list = "" 
//                   | "*" num optional_num_list 
//                   | "/" num optional_num_list
// we want match the string "1*2+3"
// which symbols are nonterminals?
// expr, optional_mul_list, mul, optional_num_list
// which symbols are terminals? 
// num, "*", "/", "+", "-"
// what is the start symbol? this has to be given to you 
// expr
// -> expr 
// -> mul   optional_mul_list
// -> num  optional_num_list  optional_mul_list 
                                            // (replacing num with 1, move pointer forward)
// -> 1  optional_num_list  optional_mul_list 
// -> 1 * num optional_num_list optional_mul_list
// -> 1 * 2 optional_num_list optinal_mul_list
// -> 1 * 2 optional_mul_list
// -> 1 * 2 + mul optional_mul_list
// -> 1 * 2 + num optional_num_list optional_mul_list
// -> 1 * 2 + 3 optional_num_list optional_mul+list
// -> 1 * 2 + 3 optional_mul+list
// -> 1 * 2 + 3 

// -> expr 
// -expr
// -> mul   optional_mul_list
// -expr 
// |-mul
// |-optional_mul_list
// -> num  optional_num_list  optional_mul_list 
                                            // (replacing num with 1, move pointer forward)
// -expr 
// |-mul
//  |-num 
//  |-optional_num_list
// |-optional_mul_list
// -> 1  optional_num_list  optional_mul_list 
// -expr 
// |-mul
//  |-num 
//   |-1
//  |-optional_num_list
// |-optional_mul_list
// -> 1 * num optional_num_list optional_mul_list
// -expr 
// |-mul
//  |-num 
//   |-1
//  |-optional_num_list
//   |-*
//   |-num
//   |-optional_num_list
// |-optional_mul_list
// -> 1 * 2 optional_num_list optinal_mul_list
// // -expr 
// |-mul
//  |-num 
//   |-1
//  |-optional_num_list
//   |-*
//   |-num
//    |-2
//   |-optional_num_list
// |-optional_mul_list
// -> 1 * 2 optional_mul_list
//-expr 
// |-mul
//  |-num 
//   |-1
//  |-optional_num_list
//   |-*
//   |-num
//    |-2
//   |-optional_num_list
//    |-NULL
// |-optional_mul_list
// -> 1 * 2 + mul optional_mul_list
//-expr 
// |-mul
//  |-num 
//   |-1
//  |-optional_num_list
//   |-*
//   |-num
//    |-2
//   |-optional_num_list
//    |-NULL
// |-optional_mul_list
//  |-+
//  |-mul
//  |-optional_mul_list
// -> 1 * 2 + num optional_num_list optional_mul_list
//-expr 
// |-mul
//  |-num 
//   |-1
//  |-optional_num_list
//   |-*
//   |-num
//    |-2
//   |-optional_num_list
//    |-NULL
// |-optional_mul_list
//  |-+
//  |-mul
//   |-num
//   |-optional_num_list
//  |-optional_mul_list
// -> 1 * 2 + 3 optional_num_list optional_mul+list
//-expr 
// |-mul
//  |-num 
//   |-1
//  |-optional_num_list
//   |-*
//   |-num
//    |-2
//   |-optional_num_list
//    |-NULL
// |-optional_mul_list
//  |-+
//  |-mul
//   |-num
//   |-3
//   |-optional_num_list
//  |-optional_mul_list
// -> 1 * 2 + 3 optional_mul+list
//-expr 
// |-mul
//  |-num 
//   |-1
//  |-optional_num_list
//   |-*
//   |-num
//    |-2
//   |-optional_num_list
//    |-NULL
// |-optional_mul_list
//  |-+
//  |-mul
//   |-num
//   |-3
//   |-optional_num_list
//    |-NULL
//  |-optional_mul_list
// -> 1 * 2 + 3 
//-expr 
// |-mul
//  |-num 
//   |-1
//  |-optional_num_list
//   |-*
//   |-num
//    |-2
//   |-optional_num_list
//    |-NULL
// |-optional_mul_list
//  |-+
//  |-mul
//   |-num
//   |-3
//   |-optional_num_list
//    |-NULL
//  |-optional_mul_list
//   |-NULL

// the above is a CONCRETE syntax tree which matches the input and rules used exactly,
// every step of the way.
// reading the leaf nodes in order corresponds to the input exactly
// compare this to the ABSTRACT syntax tree, which is what we were implementing.
// that throws away everything we don't need and just keeps the meaningful tree structure.
// we also used the * rules and massaged/set up the tree the way we wanted. 
// 1 * 2 + 3
// |-+
// |-*
//  |-1
//  |-2
// |-3

// enum TokenKind;

// enum TokenKind {
//     TK_NUM = 0,
//     TK_PUNCT,
//     TK_EOF
// };

// class Token;
// class Token {
// public:
//     TokenKind kind;

//     Token(TokenKind kind, long val) : kind(kind), num(val) {

//     };
//     Token(TokenKind kind, string p) : kind(kind), punct(p) {

//     };
//     Token(TokenKind kind) : kind(kind) {}

//     void print() {
//         if(kind == TK_NUM) {
//             cout << "<";
//             cout << "TK_NUM: " << num;
//             cout << ">" << endl;
//         }
//         else if(kind == TK_PUNCT) {
//             cout << "<";
//             cout << "TK_PUNCT: " << punct;
//             cout << ">" << endl;
//         }
//         else if(kind == TK_EOF) {
//             cout << "<";
//             cout << "TK_EOF " << punct;
//             cout << ">" << endl;
//         }
//     }

//     // if TK_PUNCT
//     string punct;

//     // if TK_NUM
//     int num;
// };

vector<Token*> tokens;
Token* current_tok;
int tokens_i;


void tokenize(char* p) {
    while(*p) {
        if(isspace(*p)) {
            p++;// do nothing 
        }
        else if(isdigit(*p)) {
            char* q = p;
            long val = strtol(p, &p, 10);
            tokens.push_back(new Token(TK_NUM, val));
        }
        else if((*p == '+') || (*p == '-')){
            // cout << "*p is " << *p << endl;
            string s = "";
            s.push_back(*p);
            // cout << "s is " << s << endl;
            tokens.push_back(new Token(TK_PUNCT, s));
            p++;
        }
        else {
            assert(false && "shouldn't reach here");
        }
    }

    tokens.push_back(new Token(TK_EOF));
}

enum NodeKind {
    ND_NUM,
    ND_ADD,
    ND_SUB
};

class Node {
public:
    NodeKind kind;

    // fields for ND_NUM
    int num;

    //fields for ND_ADD or ND_SUB
    Node *lhs, *rhs;

    // ND_NUM ctor
    Node(NodeKind kind, int n) : kind(kind), num(n) {}

    // ND_ADD or ND_SUB ctor
    Node(NodeKind kind, Node* l, Node* r) : kind(kind), lhs(l), rhs(r) {}
};

Node* expr();
Node* primary();
Node* num();

// 4+2-6 |
// expr := primary ("+" primary | "-" primary)*
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
// <TK_NUM: "42">
// num := <any integer>
Node* num() {
    assert(current_tok->kind == TK_NUM);
    int n = current_tok->num;
    current_tok = tokens[++tokens_i];
    return new Node(ND_NUM, n);
}

void codegen(Node* _expr) {
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

// expected input: <some number>
int main(int argc, char* argv[]){
    if((argc != 2) && (argc != 3)) {
        printf("wrong number of arguments\n");
        exit(1);
    }

    char* p;

    if(argc == 2) {
        p = argv[1];
    }
    else if(argc == 3) {
        p = argv[2];
    }


    

    tokenize(p);

    tokens_i = 0;
    current_tok = tokens[tokens_i];
    //usage: ./leocc --print-cst "program"
    if((argc == 3) && (std::string(argv[1]) == "--print-cst")) {
        CNode* concrete_tree = concrete_parse();
        concrete_tree->print_cst(0);
        
        std::cout << "exiting from --print-cst option in main\n";
        exit(0);
    }
    


    for(int i = 0; i < tokens.size(); i++) {
        //tokens[i]->print();
    }

        
    cout << ".global _main" << endl;
    cout << "_main:" << endl;


    tokens_i = 0;
    current_tok = tokens[tokens_i];

    Node* top_expr = expr();
    
    codegen(top_expr);

    cout << "ret" << endl;
    //4+2-6
    
    return 0;
}
#ifndef MAIN_H

#include <vector>
#include <string>
#include <iostream>

using namespace std;

enum TokenKind {
    TK_NUM = 0,
    TK_PUNCT,
    TK_EOF
};

class Token {
public:
    TokenKind kind;

    Token(TokenKind kind, long val) : kind(kind), num(val) {

    };
    Token(TokenKind kind, string p) : kind(kind), punct(p) {

    };
    Token(TokenKind kind) : kind(kind) {}

    void print() {
        if(kind == TK_NUM) {
            cout << "<";
            cout << "TK_NUM: " << num;
            cout << ">" << endl;
        }
        else if(kind == TK_PUNCT) {
            cout << "<";
            cout << "TK_PUNCT: " << punct;
            cout << ">" << endl;
        }
        else if(kind == TK_EOF) {
            cout << "<";
            cout << "TK_EOF " << punct;
            cout << ">" << endl;
        }
    }

    // if TK_PUNCT
    string punct;

    // if TK_NUM
    int num;
};

extern vector<Token*> tokens;
extern Token* current_tok;
extern int tokens_i;

// ConcreteNode
class CNode;
class CExpr;
class CMul;
class CNum;
class CMulList;
class CNumList;



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
//    |-3
//   |-optional_num_list
//    |-NULL
//  |-optional_mul_list
//   |-NULL

class CNode {
    public:
    virtual void print_cst(int depth) = 0;
};

class CExpr : public CNode {
public:
    // void print_tree(int depth) {}
    CMul* mul = nullptr;
    CMulList* mul_list = nullptr;
    void print_cst(int depth);
};

class CMul : public CNode {
    public:
    CNum* num;
    CNumList* num_list;
    void print_cst(int depth) override;
    // void print_tree(int depth) {}
        
};

class CPunct : public CNode {
public:
    string punct;
    void print_cst(int depth) override;
};

class CMulList : public CNode {
    public:
    CPunct* plus_or_minus = nullptr;
    CMul* mul = nullptr;
    CMulList* mul_list = nullptr;
    void print_cst(int depth) override;

};

class CNum : public CNode { 
    public:
    unsigned int num_literal;
    void print_cst(int depth)override;
};

class CNumList : public CNode {
    public:
    CNode* times_or_divides;
    CNum* num;
    CNumList* num_list;
    void print_cst(int depth) override;
};

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
    num->print_cst(depth+1);
    num_list->print_cst(depth+1);
}

//CNode* times_or_divides;
//CNum* num;
//CNumList* num_list;
void CNumList::print_cst(int depth){
    for(int i = 0; i < depth; i++){
        cout << " ";
    }
    cout << "|-num_list" << endl;
    
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
        num->print_cst(depth+1);
        num_list->print_cst(depth+1);
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

// grammar without any tricks like * lists
// expr = mul mul_list
// mul = num num_list
// mul_list =  "" 
//           | "+" mul mul_list 
//           | "-" mul mul_list
// num_list = "" 
//          | "*" num num_list 
//          | "/" num num_list
CExpr* C_expr();
CMul* C_mul();
CMulList* C_mul_list();
CNum* C_num(){
    CNum* result = new CNum();
    
    assert(tokens[tokens_i]->kind == TK_NUM && "WRONG SOMETHING WENT WRONG, NEED NUMBER");
    
    result->num_literal = tokens[tokens_i++]->num;
    return result;
}
// num_list = "" 
//          | "*" num num_list 
//          | "/" num num_list
CNumList* C_num_list(){
    CNumList* result = new CNumList();
    // since our test case is 1+2+3, and doesn't use
    // "*" or "/", we can implement the rest later.
    return result;
}


// mul = num num_list
CMul* C_mul() {
    CMul* result = new CMul();
    result->num = C_num();
    result->num_list = C_num_list();
    return result;
}
// mul_list =  "" 
//           | "+" mul mul_list 
//           | "-" mul mul_list
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

CNode* concrete_parse() {
    // cout << "called concrete_parse\n";
    // for(int i = 0; i < tokens.size(); i++) {
    //     tokens[i]->print();
    // }
    //USED TO DEBUGG^^^^^
    CExpr* expr = C_expr();
    return expr;
}
//     | 
//1+2+3
// expr = mul mul_list
// -> expr 
// -> mul mul_list
// -> num num_list mul_list
// -> 1 num_list mul_list
// -> 1 + num num_list  mul_list
// -> 1 + 2 num_list  mul_list
// -> 1 + 2 mul_list
// -> 1 + 2 + mul mul_list
// -> 1 + 2 + num num_list mul_list
// -> 1 + 2 + 3 num_list mul_list
// -> 1 + 2 + 3 mul_list
// -> 1 + 2 + 3
// |
// 1+2+3
// expr = mul mul_list
CExpr* C_expr() {
    CExpr* result = new CExpr();
    result->mul = C_mul();
    result->mul_list = C_mul_list();
    return result;
}

#endif 
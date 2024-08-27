#include <iostream>
#include <vector>

using namespace std;
//START TOKENIZER
class Token; 

enum TokenKind {
    TK_NUM = 0,
    TK_PUNCT,
    TK_EOF
};

class Token {
public:
    TokenKind kind;

    Token(TokenKind kind, long val);
    Token(TokenKind kind, string p);
    Token(TokenKind kind);

    void print();

    // if TK_PUNCT
    string punct;

    // if TK_NUM
    int num;
};

extern vector<Token*> tokens;
extern Token* current_tok;
extern int tokens_i;

void tokenize(char* p);
//END TOKENIZER 

//START CONCRETE SYNTAX TREE 
class CNode;
class CExpr;
class CMul;
class CNum;
class CMulList;
class CNumList;

class CNode {
    public:
    virtual void print_cst(int depth) = 0;
};

class CExpr : public CNode {
public:
    CMul* mul = nullptr;
    CMulList* mul_list = nullptr;
    void print_cst(int depth);
};

class CMul : public CNode {
    public:
    CNum* num;
    CNumList* num_list;
    void print_cst(int depth) override;
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

CNode* concrete_parse();
//END CONCRETE SYNTAX TREE

//START ABSTRACT SYNTAX TREE
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

Node* abstract_parse();

//END ABSTRACT SYNTAX TREE

//START CODEGEN
void do_codegen(Node* _expr);
//END CODEGEN

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
class CPrimary;
class CPrimaryList;
class CPunct;
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
    CPrimary* primary;
    CPrimaryList* primary_list;
    void print_cst(int depth) override;
};

class CPrimary : public CNode{
    public:
    CPunct* leftParenthesis;
    CExpr* expr;
    CPunct* rightParenthesis;
    CNum* num;
    void print_cst(int depth) override;
};

class CPrimaryList : CNode{
    public:
    CPunct* times_or_divides;
    CPrimary* primary;
    CPrimaryList* primary_list;
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
class Node {
public:
    virtual void codegen() = 0;
};
class NodeExpr : public Node {
public:
    virtual void codegen() = 0;
};
class NodeBinOp : public NodeExpr {
public:
    string punct;
    NodeExpr* lhs;
    NodeExpr* rhs;
    virtual void codegen() = 0;
};
class NodeLT : public NodeBinOp{
    public:
    void codegen();
};

class NodeGT : public NodeBinOp{
    public:
    void codegen();
};

class NodeLTE : public NodeBinOp{
    public:
    void codegen();
};

class NodeGTE : public NodeBinOp{
    public:
    void codegen();
};

class NodeEE : public NodeBinOp{
    public:
    void codegen();
};
    
class NodeNE : public NodeBinOp{
    public:
    void codegen();
};

class NodeAdd : public NodeBinOp {
public:
    void codegen();
};
class NodeSub : public NodeBinOp {
public:
    void codegen();
};
class NodeMul : public NodeBinOp {
public:
    void codegen();
};
class NodeDiv : public NodeBinOp {
public:
    void codegen();
};
class NodeNum : public NodeExpr {
    public:
    int num_literal;
    void codegen();
};
Node* abstract_parse();

//END ABSTRACT SYNTAX TREE

//START CODEGEN
void do_codegen(Node* _expr);
//END CODEGEN

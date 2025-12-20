#include <iostream>
#include <vector>
#include <map>

using namespace std;

// START TOKENIZER
class Token;

enum TokenKind {
    TK_NUM = 0,
    TK_PUNCT,
    TK_ID,
    TK_KW,
    TK_EOF
};

enum KeywordKind {
    KW_NONE,
    KW_RET,
    KW_WHILE,
    KW_FOR,
    KW_INT
};

class Token {
public:
    TokenKind kind;
    KeywordKind kw_kind = KW_NONE;

    Token(TokenKind kind, long val);
    Token(TokenKind kind, string p);
    Token(TokenKind kind);

    void print();

    // if TK_PUNCT
    string punct;
    // if TK_ID
    string id;
    // if TK_NUM
    int num;
};

extern vector<Token*> tokens;
extern Token* current_tok;
extern int tokens_i;

void tokenize(char* p);
// END TOKENIZER

// START ABSTRACT SYNTAX TREE
class CType {
public:
    uint size;
    virtual ~CType() = 0;
    virtual bool isIntType();
    virtual bool isPtrType();
};

class CIntType : public CType {
public:
    bool isIntType() override;
    CIntType();
};

class CPtrType : public CType {
public:
    bool isPtrType() override;
    CType* referenced_type;
    CPtrType(CType* r);
};

class object {
public:
    CType* c_type = nullptr;
    static int counter;
    int offSet;
    object();
};

// Function-scoped variable map
extern map<string, map<string, object*>> var_map;
extern string current_function;

class Node {
public:
    virtual void codegen() = 0;
    virtual bool is_NodeId();
    virtual bool is_NodeAssign();
    virtual bool is_NodeAddressOf();
    virtual bool is_NodeDereference();
    Node* parent;
};

class NodeStmt : public Node {
public:
    virtual void codegen() = 0;
};

class NodeExpr : public Node {
public:
    CType* c_type = nullptr;
    virtual void codegen() = 0;
};

class NodeFunctionCall : public NodeExpr {
public:
    string functionName;
    vector<NodeExpr*> args;
    NodeFunctionCall(const string& name, const vector<NodeExpr*>& args);
    void codegen() override;
};

class NodeDereference : public NodeExpr {
public:
    NodeExpr* _expr;
    NodeDereference(NodeExpr* e);
    void codegen() override;
    bool is_NodeDereference() override;
};

class NodeId : public NodeExpr {
public:
    string id;
    void codegen() override;
    bool is_NodeId() override;
    NodeId(string);
};

class NodeAddressOf : public NodeExpr {
public:
    NodeExpr* _expr;
    NodeAddressOf(NodeExpr* e);
    void codegen() override;
    bool is_NodeAddressOf() override;
};

class NodeDecl : public Node {
public:
    std::string varName;
    CType* c_type = nullptr;
    int pointerDepth;
    NodeExpr* initializer;

    NodeDecl(std::string name, int depth, NodeExpr* init = nullptr);

    void codegen() override;
};

class NodeDeclList : public NodeStmt {
public:
    std::vector<NodeDecl*> decls;

    NodeDeclList(std::vector<NodeDecl*> decls);

    void codegen() override;
};

class NodeForStmt : public NodeStmt {
public:
    static int counter;
    NodeStmt* Init;
    NodeStmt* Cond;
    NodeExpr* Increment;
    NodeStmt* Body;
    void codegen() override;
    NodeForStmt(NodeStmt* Init, NodeStmt* Cond, NodeExpr* Increment, NodeStmt* Body);
};

class NodeWhileStmt : public NodeStmt {
public:
    static int counter;
    NodeExpr* _expr;
    NodeStmt* _stmt;
    NodeWhileStmt(NodeExpr* e, NodeStmt* s);
    void codegen() override;
};

class NodeNullStmt : public NodeStmt {
public:
    NodeNullStmt();
    void codegen() override;
};

class NodeBlockStmt : public NodeStmt {
public:
    vector<NodeStmt*> stmt_list;
    NodeBlockStmt(vector<NodeStmt*> s);
    void codegen() override;
};

class NodeFunctionDef : public Node {
public:
    std::string declspec;
    std::string declarator;
    NodeBlockStmt* body;
    vector<NodeDecl*> params;

    NodeFunctionDef(std::string declspec, std::string declarator, NodeBlockStmt* body, vector<NodeDecl*> params);

    void codegen() override;
};

class NodeProgram : public Node {
public:
    std::vector<NodeFunctionDef*> func_defs;

    NodeProgram(std::vector<NodeFunctionDef*> func_defs);

    void codegen() override;
};

class NodeExprStmt : public NodeStmt {
public:
    NodeExpr* _expr;
    void codegen() override;
    NodeExprStmt(NodeExpr* e);
};

class NodeReturnStmt : public NodeStmt {
public:
    NodeExpr* _expr;
    NodeReturnStmt(NodeExpr* e);
    void codegen() override;
};

class NodeBinOp : public NodeExpr {
public:
    string punct;
    NodeExpr* lhs;
    NodeExpr* rhs;
    virtual void codegen() = 0;
    NodeBinOp(NodeExpr* l, NodeExpr* r, string p);
};

class NodeAssign : public NodeBinOp {
public:
    void codegen() override;
    NodeAssign(NodeExpr* lhs, NodeExpr* rhs);
    bool is_NodeAssign() override;
};

class NodeLT : public NodeBinOp {
public:
    void codegen() override;
    NodeLT(NodeExpr* lhs, NodeExpr* rhs);
};

class NodeGT : public NodeBinOp {
public:
    void codegen() override;
    NodeGT(NodeExpr* lhs, NodeExpr* rhs);
};

class NodeLTE : public NodeBinOp {
public:
    void codegen() override;
    NodeLTE(NodeExpr* lhs, NodeExpr* rhs);
};

class NodeGTE : public NodeBinOp {
public:
    void codegen() override;
    NodeGTE(NodeExpr* lhs, NodeExpr* rhs);
};

class NodeEE : public NodeBinOp {
public:
    void codegen() override;
    NodeEE(NodeExpr* lhs, NodeExpr* rhs);
};

class NodeNE : public NodeBinOp {
public:
    void codegen() override;
    NodeNE(NodeExpr* lhs, NodeExpr* rhs);
};

class NodeAdd : public NodeBinOp {
public:
    void codegen() override;
    NodeAdd(NodeExpr* lhs, NodeExpr* rhs);
};

class NodeSub : public NodeBinOp {
public:
    void codegen() override;
    NodeSub(NodeExpr* lhs, NodeExpr* rhs);
};

class NodeMul : public NodeBinOp {
public:
    void codegen() override;
    NodeMul(NodeExpr* lhs, NodeExpr* rhs);
};

class NodeDiv : public NodeBinOp {
public:
    void codegen() override;
    NodeDiv(NodeExpr* lhs, NodeExpr* rhs);
};

class NodeNum : public NodeExpr {
public:
    int num_literal;
    NodeNum(int n);
    void codegen() override;
};

Node* abstract_parse();
// END ABSTRACT SYNTAX TREE

void do_codegen(Node* _expr);
void do_codegen_llvm(Node* _expr);


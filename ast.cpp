#include "leocc.hpp"
#include <cassert>  
#include <map>
#include <string>
/*
// Main program structure
program = stmt*
block-stmt = (declaration | stmt)* "}"
stmt = "return" expr ";"
     | "if" "(" expr ")" stmt ("else" stmt)?
     | "for" "(" expr-stmt expr-stmt expr? ")" stmt
     | "while" "(" expr ")" stmt
     | "{" block-stmt
     | expr-stmt
declaration = declspec declarator ("=" expr)? ("," declarator ("=" expr)?)* ";"
declspec = "int"
declarator = "*"* id
expr-stmt = expr? ";"
expr = assign
assign = equality ("=" assign)?
equality = relational ("==" relational | "!=" relational)*
relational = add ("<" add | "<=" add | ">" add | ">=" add)*
add = mul ("+" mul | "-" mul)*
mul = unary ("*" unary | "/" unary | "&")*
unary = ("+" | "-" | "*" | "&") unary | primary
primary = "(" expr ")" | num | id
terminals:
num = <any number>
id = <any string of id>
*/


map<string, object*> var_map;
int object::counter = 0;
NodeBlockStmt* block_stmt();
NodeProgram* program();
NodeStmt* stmt();
NodeStmt* expr_stmt();
NodeExpr* assign();
NodeExpr* expr();
NodeExpr* primary();
NodeExpr* mul();
NodeNum* num();
NodeExpr* unary();
NodeExpr* equality();
NodeExpr* relational();
NodeExpr* add();
NodeDeclList* declaration();

object::object(){
    offSet=++counter*8;
}

CType::~CType() {}

NodeDeclList::NodeDeclList(std::vector<NodeDecl*> decls) {
    this->decls = decls;
}

NodeDecl::NodeDecl(std::string name, int depth, NodeExpr* init) {
    this->varName = name;
    this->pointerDepth = depth;
    this->initializer = init;
}

NodeBinOp::NodeBinOp(NodeExpr* l, NodeExpr* r, string p) {
    lhs = l;
    rhs = r;
    punct = p;
    lhs->parent = this;
    rhs->parent = this;
}

NodeAssign::NodeAssign(NodeExpr* _lhs, NodeExpr* _rhs) : NodeBinOp(_lhs, _rhs, "=") {
    if(lhs->is_NodeId()) {
        lhs->c_type = rhs->c_type;
        var_map[((NodeId*)(lhs))->id]->c_type = lhs->c_type;
    }
    c_type = rhs->c_type;
}

NodeDiv::NodeDiv(NodeExpr* l, NodeExpr* r) : NodeBinOp(l, r, "/") {
    c_type = new CIntType();
}

NodeSub::NodeSub(NodeExpr* l, NodeExpr* r) : NodeBinOp(l, r, "-") {
    if(rhs->c_type->isIntType() && lhs->c_type->isPtrType()) {
        c_type = new CPtrType(new CIntType());
    }
    else{
        c_type = new CIntType();
    }
}

NodeAdd::NodeAdd(NodeExpr* l, NodeExpr* r) : NodeBinOp(l, r, "+") {
    if(lhs->c_type->isPtrType() && rhs->c_type->isIntType()) {
        c_type = new CPtrType(new CIntType());
    }
    else if(lhs->c_type->isIntType() && rhs->c_type->isPtrType()){//ptr_arith is canonocolized so that the pointer
        NodeExpr* temp = lhs;                                       // arith always follows "lhs = ptr| rhs = INT"
        lhs = rhs;
        rhs = temp;
        c_type = new CPtrType(new CIntType());
    }
    else{
        c_type = new CIntType();
    }
}

NodeMul::NodeMul(NodeExpr* l, NodeExpr* r) : NodeBinOp(l, r, "*"){
    c_type = new CIntType();
}

NodeEE::NodeEE(NodeExpr* l, NodeExpr* r) : NodeBinOp(l, r, "=="){
    c_type = new CIntType();
}

NodeGTE::NodeGTE(NodeExpr* l, NodeExpr* r) : NodeBinOp(l, r, ">="){
    c_type = new CIntType();
}

NodeGT::NodeGT(NodeExpr* l, NodeExpr* r) : NodeBinOp(l, r, ">"){
    c_type = new CIntType();
}

NodeLTE::NodeLTE(NodeExpr* l, NodeExpr* r) : NodeBinOp(l, r, "<="){
    c_type = new CIntType();
}

NodeLT::NodeLT(NodeExpr* l, NodeExpr* r) : NodeBinOp(l, r, "<"){
    c_type = new CIntType();
}

NodeNE::NodeNE(NodeExpr* l, NodeExpr* r) : NodeBinOp(l, r, "!="){
    //c_type = new CIntType();
}

NodeExprStmt::NodeExprStmt(NodeExpr* e){
    _expr = e;
    _expr->parent = this;
};

CIntType::CIntType(){
    size = 8;
};

CPtrType::CPtrType(CType* r){
    referenced_type = r;
    size = 8;
};

NodeNum::NodeNum(int n){
    num_literal = n;
    c_type = new CIntType();
}

NodeAddressOf::NodeAddressOf(NodeExpr* e){
    _expr = e;
    _expr->parent = this;
    c_type = new CPtrType(_expr->c_type);
}

NodeDereference::NodeDereference(NodeExpr* e) {
    _expr = e;
    _expr->parent = this;
};

NodeForStmt::NodeForStmt(NodeStmt* s1, NodeStmt* s2, NodeExpr* e, NodeStmt* s){
    Init = s1;
    Cond = s2;
    Increment = e;
    Body = s;
    Init->parent = this;
    Cond->parent = this;
    Increment->parent = this;
    Body->parent = this;
};

NodeWhileStmt::NodeWhileStmt(NodeExpr* e, NodeStmt* s){
    _expr = e;
    _stmt = s;
    _expr->parent = this;
    _stmt->parent = this;
};

NodeNullStmt::NodeNullStmt(){
    ;
}

NodeBlockStmt::NodeBlockStmt(vector<NodeStmt*> _stmts){
    stmt_list = _stmts;
    for(NodeStmt* s:stmt_list){
        s->parent = this;
    }
}

NodeProgram::NodeProgram(vector<NodeStmt*> _stmts){
    stmts = _stmts;
    for( NodeStmt* s:stmts){
        s->parent = this;
    }   
}

NodeId::NodeId(std::string _id) : id(_id) {
    if (var_map.find(id) == var_map.end()) {
        cerr << "Error: Variable '" << id << "' used without declaration." << endl;
        exit(1);
    }
    c_type = var_map[id]->c_type;
}

NodeReturnStmt::NodeReturnStmt(NodeExpr* e) { 
    _expr = e;
    _expr->parent = this;
}

NodeProgram* program() {
    vector<NodeStmt*> stmts;
    while(tokens[tokens_i]->kind != TK_EOF){
        stmts.push_back(stmt());
    }
    NodeProgram* Node_Program = new NodeProgram(stmts);
    return Node_Program;
}

NodeDeclList* declaration() {
    assert(tokens[tokens_i]->kind == TK_KW && tokens[tokens_i]->kw_kind == KW_INT && "Expected a type specifier (e.g., 'int')");
    tokens_i++; // Consume variable decl

    vector<NodeDecl*> decls;

    do {
        int pointerDepth = 0;
        while (tokens[tokens_i]->kind == TK_PUNCT && tokens[tokens_i]->punct == "*") {
            pointerDepth++;
            tokens_i++;
        }

        assert(tokens[tokens_i]->kind == TK_ID && "Expected an identifier");
        string varName = tokens[tokens_i++]->id;

        NodeExpr* initializer = nullptr;
        if (tokens[tokens_i]->kind == TK_PUNCT && tokens[tokens_i]->punct == "=") {
            tokens_i++; // Consume '='
            initializer = expr();
        }

        if (var_map.find(varName) != var_map.end()) {
            cerr << "Error: Variable '" << varName << "' is already declared." << endl;
            exit(1);
        }

        var_map[varName] = new object();

        decls.push_back(new NodeDecl(varName, pointerDepth, initializer));

    } while (tokens[tokens_i]->kind == TK_PUNCT && tokens[tokens_i]->punct == "," && tokens_i++);

    assert(tokens[tokens_i]->kind == TK_PUNCT && tokens[tokens_i]->punct == ";" && "Expected ';' at the end of the declaration");
    tokens_i++; // Consume ';'

    return new NodeDeclList(decls);
}


NodeStmt* stmt() {
    if (tokens[tokens_i]->kind == TK_KW && tokens[tokens_i]->kw_kind == KW_RET) {
        tokens_i++;
        NodeReturnStmt* result = new NodeReturnStmt(expr());
        assert(tokens[tokens_i]->kind == TK_PUNCT && tokens[tokens_i]->punct == ";" && "Expected ';' after return expression");
        tokens_i++;
        return result;
    } 
    else if (tokens[tokens_i]->kind == TK_PUNCT && tokens[tokens_i]->punct == "{") {
        tokens_i++;
        return block_stmt();
    } 
    else if (tokens[tokens_i]->kind == TK_KW && tokens[tokens_i]->kw_kind == KW_WHILE) {
        tokens_i++;
        assert(tokens[tokens_i]->kind == TK_PUNCT && tokens[tokens_i]->punct == "(" && "Expected '(' after 'while'");
        tokens_i++;
        NodeExpr* condition = expr();
        assert(tokens[tokens_i]->kind == TK_PUNCT && tokens[tokens_i]->punct == ")" && "Expected ')' after while condition");
        tokens_i++;
        NodeStmt* body = stmt();
        return new NodeWhileStmt(condition, body);
    } 
    else if (tokens[tokens_i]->kind == TK_KW && tokens[tokens_i]->kw_kind == KW_FOR) {
        tokens_i++;
        assert(tokens[tokens_i]->kind == TK_PUNCT && tokens[tokens_i]->punct == "(" && "Expected '(' after 'for'");
        tokens_i++;
        NodeStmt* init = expr_stmt();
        NodeStmt* condition = expr_stmt();
        NodeExpr* increment = nullptr;
        if (tokens[tokens_i]->kind != TK_PUNCT || tokens[tokens_i]->punct != ")") {
            increment = expr();
        }
        assert(tokens[tokens_i]->kind == TK_PUNCT && tokens[tokens_i]->punct == ")" && "Expected ')' after for loop clauses");
        tokens_i++;
        NodeStmt* body = stmt();
        return new NodeForStmt(init, condition, increment, body);
    } 
    else if (tokens[tokens_i]->kind == TK_KW && tokens[tokens_i]->kw_kind == KW_INT) {
        return declaration();
    } 
    else {
        return expr_stmt();
    }
}

    
NodeBlockStmt* block_stmt() {
    std::vector<NodeStmt*> stmtList;
    while (tokens[tokens_i]->kind != TK_PUNCT || tokens[tokens_i]->punct != "}") {
        if (tokens[tokens_i]->kind == TK_KW && tokens[tokens_i]->kw_kind == KW_INT) {
            stmtList.push_back(dynamic_cast<NodeStmt*>(declaration())); 
        } else {
            stmtList.push_back(stmt());
        }
    }
    tokens_i++; // Skip '}'
    return new NodeBlockStmt(stmtList);
}

NodeStmt* expr_stmt() {
    if((tokens[tokens_i]->kind == TK_PUNCT && tokens[tokens_i]->punct == ";" )){
        tokens_i++;
        return new NodeNullStmt();
    }
    NodeExprStmt* result = new NodeExprStmt(expr());
    assert(tokens[tokens_i]->kind == TK_PUNCT && "Should be a ';'");
    assert(tokens[tokens_i]->punct == ";" && "Should be a ';'");
    tokens_i++;
    return result;
}

NodeExpr* expr(){
    return assign();
}

NodeExpr* assign(){ 
    NodeExpr* result = equality();
    if(tokens[tokens_i]->kind == TK_PUNCT && tokens[tokens_i]->punct == "="){
        tokens_i++;
        NodeAssign* _assign = new NodeAssign(result, assign());
        assert((_assign->lhs->is_NodeId() || _assign->lhs->is_NodeDereference()) && "TYPECHECK: lhs of NodeAssign must be a NodeId");
        result = _assign;
    }
    return result;
}

NodeExpr* equality(){
    NodeExpr* result = relational();
    while( tokens[tokens_i]->kind == TK_PUNCT &&
        (tokens[tokens_i]->punct == "==" || tokens[tokens_i]->punct == "!=")){
            if(tokens[tokens_i]->punct == "=="){
                tokens_i++;
                result = new NodeEE(result, relational());
            }
            else{
                tokens_i++;
                result = new NodeNE(result,relational());
            }
    }
    return result;
}

NodeExpr* relational(){
    NodeExpr* result = add();
    while( tokens[tokens_i]->kind == TK_PUNCT &&
        (tokens[tokens_i]->punct == "<" || tokens[tokens_i]->punct == ">" || tokens[tokens_i]->punct == ">=" || tokens[tokens_i]->punct == "<=")){
            if(tokens[tokens_i]->punct == "<"){
                tokens_i++;
                result = new NodeLT(result, add());
            }
            else if(tokens[tokens_i]->punct == ">"){
                tokens_i++;
                result = new NodeGT(result, add());
            }
            else if(tokens[tokens_i]->punct == "<="){
                tokens_i++;
                result = new NodeLTE(result, add());
            }
            else if(tokens[tokens_i]->punct == ">="){
                tokens_i++;
                result = new NodeGTE(result, add());
            }
        }

    return result;
}

NodeExpr* unary(){
    if( tokens[tokens_i]->kind == TK_PUNCT &&
        (tokens[tokens_i]->punct == "+" || tokens[tokens_i]->punct == "-" || tokens[tokens_i]->punct == "*" || tokens[tokens_i]->punct == "&")){
        if(tokens[tokens_i]->punct == "+"){
            tokens_i++;
            return unary();
        }
        else if(tokens[tokens_i]->punct == "*"){
            tokens_i++;
            return new NodeDereference(unary());
        }
        else if(tokens[tokens_i]->punct == "&"){
            tokens_i++;
            return new NodeAddressOf(unary());
        }
        else{
            assert(tokens[tokens_i]->punct == "-");
            tokens_i++;
            return new NodeMul(new NodeNum(-1), unary());
            
        }
    }
    return primary();
}

NodeNum* num() {
    assert(tokens[tokens_i]->kind == TK_NUM && "NOT A NUMBER, MUST BE A NUMBER");
    return new NodeNum(tokens[tokens_i++]->num);
}

NodeId* id(){
    assert(tokens[tokens_i]->kind == TK_ID && "token was not an ID when it should have been");
    NodeId* result = new NodeId(tokens[tokens_i++]->id);
    return result;
}
NodeExpr* primary() {
    if(tokens[tokens_i]->kind == TK_NUM){
        return num();
    }
    else if(tokens[tokens_i]->kind == TK_ID){
        return id();
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
        cerr << "Error: Unexpected token in primary" << endl;
        exit(1);
    }
}

NodeExpr* mul() {
    NodeExpr* result = unary();

    while(tokens[tokens_i]->kind == TK_PUNCT && (tokens[tokens_i]->punct == "*" || tokens[tokens_i]->punct == "/")){
        if(tokens[tokens_i]->punct == "*"){
            tokens_i++;
            result = new NodeMul(result,unary());
        }
        else{
            assert(tokens[tokens_i]->punct == "/");
            tokens_i++;
            result = new NodeDiv(result, unary());   
        }
    }
    return result;
}

NodeExpr* add() {
    NodeExpr* result = mul();

    while(tokens[tokens_i]->kind == TK_PUNCT && (tokens[tokens_i]->punct == "+" || tokens[tokens_i]->punct == "-")){
        if(tokens[tokens_i]->punct == "+"){
            tokens_i++;
            result = new NodeAdd(result, mul());

        }
        else{
            assert(tokens[tokens_i]->punct == "-");
            tokens_i++;
            result = new NodeSub(result, mul());

        }
    }
    return result;
}

void reverse_offsets(){
    for(auto pair : var_map){
        int stackSize = 8*var_map.size();//keep in mind its 8 byte chunks of stack size;
        pair.second->offSet = stackSize - pair.second->offSet + 8;
    }   
}

Node* abstract_parse() {
    object::counter=0;
    NodeProgram* _program = program();
    reverse_offsets();
    return _program;
}

bool Node::is_NodeId() {
    return false;
}

bool NodeId::is_NodeId() {
    return true;
}

bool Node::is_NodeAssign(){
    return false;
}

bool Node::is_NodeAddressOf(){
    return false;
}

bool Node::is_NodeDereference(){
    return false;
}

bool NodeDereference::is_NodeDereference(){
    return true;
}

bool NodeAddressOf::is_NodeAddressOf(){
    return true;
}

bool NodeAssign::is_NodeAssign(){
    return true;
}

bool CType::isIntType(){
    return false;
}

bool CIntType::isIntType(){
    return true;
}

bool CType::isPtrType(){
    return false;
}

bool CPtrType::isPtrType(){
    return true;
}
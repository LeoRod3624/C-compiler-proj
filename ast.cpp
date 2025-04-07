#include "leocc.hpp"
#include <cassert>  
#include <map>
#include <string>
/*
// Main program structure
program = stmt*
program = func-def*
func-def = declspec declarator "{" block-stmt
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
declarator = "*"* id type-suffix
type-suffix = ("(" func-params? ")")? //doing func-defs without arguments, so for now we can use this rule
func-params = param ("," param)*
param       = declspec declarator
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

NodeFunctionDef::NodeFunctionDef(std::string declspec, std::string declarator, NodeBlockStmt* body, vector<NodeDecl*> params)
    : declspec(std::move(declspec)), declarator(std::move(declarator)), body(body), params(std::move(params)) {}


NodeFunctionCall::NodeFunctionCall(const string& functionName, const vector<NodeExpr*>& args) {
    this->functionName = functionName;
    this->args = args;
    c_type = new CIntType();//This is only assumed for now since we are only dealing with functions 
    //of return type INT. PLEASE FOR THE LOVE OF GOD fix this when the time comes.
}

NodeDeclList::NodeDeclList(std::vector<NodeDecl*> decls) {
    this->decls = decls;
}

NodeDecl::NodeDecl(std::string name, int depth, NodeExpr* init) {
    this->varName = name;
    this->pointerDepth = depth;
    this->initializer = init;
    if (var_map.find(varName) != var_map.end()) {
        cerr << "Error: Variable '" << varName << "' is already declared." << endl;
        exit(1);
    }

    if (pointerDepth > 0) {
        c_type = new CPtrType(new CIntType());
        for (int i = 1; i < pointerDepth; ++i) {
            c_type = new CPtrType(c_type);
        }
    } else {
        c_type = new CIntType();
    }
    var_map[varName] = new object();
    var_map[varName]->c_type = c_type;
}

NodeBinOp::NodeBinOp(NodeExpr* l, NodeExpr* r, string p) {
    lhs = l;
    rhs = r;
    punct = p;
    lhs->parent = this;
    rhs->parent = this;
}

NodeAssign::NodeAssign(NodeExpr* _lhs, NodeExpr* _rhs) : NodeBinOp(_lhs, _rhs, "=") {
    if (lhs->is_NodeId()) {
        lhs->c_type = rhs->c_type;
        var_map[((NodeId*)(lhs))->id]->c_type = lhs->c_type;
    }
    c_type = rhs->c_type;
}

NodeDiv::NodeDiv(NodeExpr* l, NodeExpr* r) : NodeBinOp(l, r, "/") {
    c_type = new CIntType();
}

NodeSub::NodeSub(NodeExpr* l, NodeExpr* r) : NodeBinOp(l, r, "-") {
    if (!lhs->c_type || !rhs->c_type) {
        std::cerr << "TYPE ERROR in NodeSub: null c_type on lhs or rhs" << std::endl;
        exit(1);
    }
    if(rhs->c_type->isIntType() && lhs->c_type->isPtrType()) {
        c_type = new CPtrType(new CIntType());
    } else {
        c_type = new CIntType();
    }
}

NodeAdd::NodeAdd(NodeExpr* l, NodeExpr* r) : NodeBinOp(l, r, "+") {
    if (!lhs->c_type || !rhs->c_type) {
        std::cerr << "TYPE ERROR in NodeAdd: null c_type on lhs or rhs" << std::endl;
        exit(1);
    }
    std::cerr << "[NodeAdd] lhs type: " << (lhs->c_type->isPtrType() ? "ptr" : "int")
              << ", rhs type: " << (rhs->c_type->isPtrType() ? "ptr" : "int") << std::endl;

    if(lhs->c_type->isPtrType() && rhs->c_type->isIntType()) {
        c_type = new CPtrType(new CIntType());
    }
    else if(lhs->c_type->isIntType() && rhs->c_type->isPtrType()) {
        NodeExpr* temp = lhs;
        lhs = rhs;
        rhs = temp;
        c_type = new CPtrType(new CIntType());
    }
    else if(lhs->c_type->isIntType() && rhs->c_type->isIntType()) {
        c_type = new CIntType();
    }
    else {
        std::cerr << "TYPE ERROR in NodeAdd: unsupported type combination" << std::endl;
        exit(1);
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
    if (!_expr->c_type || !_expr->c_type->isPtrType()) {
        std::cerr << "TYPE ERROR: Cannot dereference non-pointer type or null type" << std::endl;
        exit(1);
    }
    c_type = ((CPtrType*)_expr->c_type)->referenced_type;
}

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

NodeProgram::NodeProgram(std::vector<NodeFunctionDef*> func_defs) {
    this->func_defs = std::move(func_defs);
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

NodeFunctionDef* func_def() {
    assert(tokens[tokens_i]->kind == TK_KW && tokens[tokens_i]->kw_kind == KW_INT && "Expected a type specifier");
    tokens_i++; // Skip `int`

    assert(tokens[tokens_i]->kind == TK_ID && "Expected an identifier for the function name");
    string functionName = tokens[tokens_i++]->id;
    assert(tokens[tokens_i]->kind == TK_PUNCT && tokens[tokens_i]->punct == "(" && "Expected '(' after function name");
    tokens_i++; // Skip `(`

    // Parse parameters
    vector<NodeDecl*> params;
    if (tokens[tokens_i]->kind != TK_PUNCT || tokens[tokens_i]->punct != ")") {
        do {
            // Parse parameter
            assert(tokens[tokens_i]->kind == TK_KW && tokens[tokens_i]->kw_kind == KW_INT && "Expected parameter type");
            tokens_i++; // Skip `int`

            int pointerDepth = 0;
            while (tokens[tokens_i]->kind == TK_PUNCT && tokens[tokens_i]->punct == "*") {
                pointerDepth++;
                tokens_i++;
            }
            // Parse parameter name
            assert(tokens[tokens_i]->kind == TK_ID && "Expected parameter name");
            std::string paramName = tokens[tokens_i++]->id;
            // Add to parameter list
            params.push_back(new NodeDecl(paramName, pointerDepth, nullptr));
        } while (tokens[tokens_i]->kind == TK_PUNCT && tokens[tokens_i]->punct == "," && tokens_i++);
    }

    assert(tokens[tokens_i]->kind == TK_PUNCT && tokens[tokens_i]->punct == ")" && "Expected closing ')' for function parameters");
    tokens_i++; // Skip `)`

    assert(tokens[tokens_i]->kind == TK_PUNCT && tokens[tokens_i]->punct == "{" && "Expected '{' to start function body");
    tokens_i++; // Skip `{`

    NodeBlockStmt* body = block_stmt();

    return new NodeFunctionDef("int", functionName, body, params);
}

NodeProgram* program() {
    vector<NodeFunctionDef*> functions;

    while (tokens[tokens_i]->kind != TK_EOF) {
        if (tokens[tokens_i]->kind == TK_KW && tokens[tokens_i]->kw_kind == KW_INT) {
            // Parse function definition
            functions.push_back(func_def());
        } else {
            cerr << "Error: Unexpected token in program" << endl;
            exit(1);
        }
    }

    return new NodeProgram(functions);
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
    if (tokens[tokens_i]->kind == TK_NUM) {
        return num();
    } else if (tokens[tokens_i]->kind == TK_ID) {
        string functionName = tokens[tokens_i]->id;

        // Check if this is a function call
        if (tokens[tokens_i + 1]->kind == TK_PUNCT && tokens[tokens_i + 1]->punct == "(") {
            tokens_i += 2; // Skip `id` and `(`

            // Parse function arguments
            vector<NodeExpr*> args;
            if (!(tokens[tokens_i]->kind == TK_PUNCT && tokens[tokens_i]->punct == ")")) {
                args.push_back(assign());
                while (tokens[tokens_i]->kind == TK_PUNCT && tokens[tokens_i]->punct == ",") {
                    tokens_i++; // Skip `,`
                    args.push_back(assign());
                }
            }

            // Ensure we close the function call with `)`
            assert(tokens[tokens_i]->kind == TK_PUNCT && tokens[tokens_i]->punct == ")" && "Expected closing ')'");
            tokens_i++; // Skip `)`

            return new NodeFunctionCall(functionName, args);
        }

        // Otherwise, it's a variable reference
        return id();
    } else if (tokens[tokens_i]->kind == TK_PUNCT && tokens[tokens_i]->punct == "(") {
        tokens_i++;
        NodeExpr* _expr = expr();
        assert(tokens[tokens_i]->kind == TK_PUNCT && tokens[tokens_i]->punct == ")" && "Expected closing ')'");
        tokens_i++;
        return _expr;
    } else {
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

void reverse_offsets() {
    int total_vars = var_map.size();
    int stackSize = 8*var_map.size();//keep in mind its 8 byte chunks of stack size;

    int i = 0;
    for (auto& pair : var_map) {
        // Assign offsets from top down so that newer vars are at lower memory
        int offset = (total_vars - i) * 8;
        pair.second->offSet = offset;
        std::cerr << "[offset] " << pair.first << " -> -" << offset << std::endl;
        ++i;
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
#include "leocc.hpp"
#include <cassert>
#include <llvm/IR/Module.h>
#include <llvm/IR/IRBuilder.h>
#include <fstream>
int NodeWhileStmt::counter = 0;
int NodeForStmt::counter = 0;
static string return_reg = "x0";
static string accum_reg = "x9"; 
static string scratch_reg = "x10";

static void emit_push(string reg) {
    cout << "  str " << reg << ", [sp, -16]!" << endl;
    return;
}

static void emit_pop(string reg){
    cout << "  ldr " << reg << ", [sp], 16" << endl;
    return;
}

static void emit_mul() {//we are not using arguments because we know to always use the accum_reg nd scratch_reg anyways for any multiply direction
    cout << "  mul " << accum_reg << ", " << scratch_reg << ", " << accum_reg << endl;
    return;
}

static void emit_div() {//we are not using arguments because we know to always use the accum_reg nd scratch_reg anyways for any multiply direction
    cout << "  sdiv " << accum_reg << ", " << scratch_reg << ", " << accum_reg << endl;
    return;
}

static void emit_div_for_special_case() {
    cout << "  sdiv " << accum_reg << ", " << accum_reg << ", " << scratch_reg << endl;
}

static void emit_lt() {
    cout << "  subs " << accum_reg << ", " << scratch_reg << ", " << accum_reg << endl;    
    cout << "  cset " << accum_reg << ", lt" << endl;
    cout << "  and " << accum_reg << ", " << accum_reg << ", #0x1" << endl;
}

static void emit_gt() {
    cout << "  subs " << accum_reg << ", " << scratch_reg << ", " << accum_reg << endl;    
    cout << "  cset " << accum_reg << ", gt" << endl;
    cout << "  and " << accum_reg << ", " << accum_reg << ", #0x1" << endl;
}

static void emit_lte() {
    cout << "  subs " << accum_reg << ", " << scratch_reg << ", " << accum_reg << endl;    
    cout << "  cset " << accum_reg << ", le" << endl;
    cout << "  and " << accum_reg << ", " << accum_reg << ", #0x1" << endl;
}

static void emit_gte() {
    cout << "  subs " << accum_reg << ", " << scratch_reg << ", " << accum_reg << endl;    
    cout << "  cset " << accum_reg << ", ge" << endl;
    cout << "  and " << accum_reg << ", " << accum_reg << ", #0x1" << endl;
}

static void emit_ee() {
    cout << "  subs " << accum_reg << ", " << scratch_reg << ", " << accum_reg << endl;    
    cout << "  cset " << accum_reg << ", eq" << endl;
    cout << "  and " << accum_reg << ", " << accum_reg << ", #0x1" << endl;
}

static void emit_ne() {
    cout << "  subs " << accum_reg << ", " << scratch_reg << ", " << accum_reg << endl;    
    cout << "  cset " << accum_reg << ", ne" << endl;
    cout << "  and " << accum_reg << ", " << accum_reg << ", #0x1" << endl;
}

static void emit_label(const string& label) {
    cout << label << ":" << endl;
}

static void emit_jump(const string& label) {
    cout << "  b " << label << endl;
}

static void emit_cond_jump(const string& label) {
    cout << "  cbz " << accum_reg << ", " << label << endl;
}

static void emit_store_to_stack(const string& reg, int offset) {
    cout << "  str " << reg << ", [fp, -" << offset << "]" << endl;
}

static void emit_store_to_memory(const string& addr_reg, const string& value_reg) {
    cout << "  str " << value_reg << ", [" << addr_reg << "]" << endl;
}

static void emit_move(const string& dest_reg, const string& src_reg) {
    cout << "  mov " << dest_reg << ", " << src_reg << endl;
}

static void emit_function_call(const string& function_name) {
    cout << "  bl " << function_name << endl;
}

static void emit_stack_push(const string& reg) {
    cout << "  str " << reg << ", [sp, -16]!" << endl;
}

static void emit_add_to_fp(const string& dest_reg, int offset) {
    cout << "  add " << dest_reg << ", fp, -" << offset << endl;
}

static void emit_load_from_address(const string& dest_reg, const string& addr_reg) {
    cout << "  ldr " << dest_reg << ", [" << addr_reg << "]" << endl;
}

static void emit_dereference(const string& reg) {
    cout << "  ldr " << reg << ", [" << reg << "]" << endl;
}

static void emit_add(const string& dest, const string& src1, const string& src2) {
    cout << "  add " << dest << ", " << src1 << ", " << src2 << endl;
}

static void emit_sub(const string& dest_reg, const string& lhs_reg, const string& rhs_reg) {
    cout << "  sub " << dest_reg << ", " << lhs_reg << ", " << rhs_reg << endl;
}

void NodeAddressOf::codegen() {
    _expr->codegen();
}

void NodeDereference::codegen() {
    _expr->codegen();
    if (!parent->is_NodeAssign()) {
        emit_dereference(accum_reg);
    }
}

void NodeNum::codegen() {
    emit_move(accum_reg, to_string(num_literal));
}

void NodeAdd::codegen() {
    lhs->codegen();
    emit_push(accum_reg);  // Push the result of `lhs` to the stack
    rhs->codegen();
    
    if (lhs->c_type->isPtrType() && rhs->c_type->isIntType()) {
        emit_move(scratch_reg, to_string(lhs->c_type->size));  // Move the size of the pointer to the scratch register
        emit_mul();  // Multiply the RHS value by the pointer size
    }
    
    emit_pop(scratch_reg);  // Pop the LHS value into the scratch register
    emit_add(accum_reg, accum_reg, scratch_reg);

}

void NodeSub::codegen(){
    lhs->codegen();
    emit_push(accum_reg);
    rhs->codegen();
    if(lhs->c_type->isPtrType() && rhs->c_type->isIntType()) {
        emit_move(scratch_reg, "8");
        emit_mul();
    }
    emit_pop(scratch_reg);//cout << "  ldr x1, [sp], 16" << endl;
    emit_sub(accum_reg, scratch_reg, accum_reg);
    if(lhs->c_type->isPtrType() && rhs->c_type->isPtrType()) {
        emit_move(scratch_reg, "8");
        emit_div_for_special_case();
    }
}

void NodeMul::codegen(){
    lhs->codegen();
    emit_push(accum_reg);
    rhs->codegen();
    emit_pop(scratch_reg);
    emit_mul();
}

void NodeDiv::codegen(){
    lhs->codegen();
    emit_push(accum_reg);
    rhs->codegen();
    emit_pop(scratch_reg);
    emit_div();
}

void NodeLT::codegen(){
    lhs->codegen();
    emit_push(accum_reg);//cout << "  str x0, [sp, -16]!" << endl;
    rhs->codegen();
    emit_pop(scratch_reg);// cout << "  ldr x1, [sp], 16" << endl;
    emit_lt();
    // cout << "  subs x0, x1, x0" << endl;    
    // cout << "  cset x0, lt" << endl;
    // cout << "  and x0, x0, #0x1" << endl;
}

void NodeGT::codegen(){
    lhs->codegen();
    emit_push(accum_reg);//cout << "  str x0, [sp, -16]!" << endl;
    rhs->codegen();
    emit_pop(scratch_reg);//cout << "  ldr x1, [sp], 16" << endl;
    emit_gt();//cout << "  subs x0, x1, x0" << endl;    
    //cout << "  cset x0, gt" << endl;
    //cout << "  and x0, x0, #0x1" << endl;
}

void NodeLTE::codegen(){
    lhs->codegen();
    emit_push(accum_reg);
    rhs->codegen();
    emit_pop(scratch_reg);
    emit_lte();
}

void NodeGTE::codegen(){
    lhs->codegen();
    emit_push(accum_reg);
    rhs->codegen();
    emit_pop(scratch_reg);
    emit_gte();
}

void NodeEE::codegen(){
    lhs->codegen();
    emit_push(accum_reg);
    rhs->codegen();
    emit_pop(scratch_reg);
    emit_ee();
}

void NodeNE::codegen(){
    lhs->codegen();
    emit_push(accum_reg);
    rhs->codegen();
    emit_pop(scratch_reg);
    emit_ne();
}

static NodeFunctionDef* current_func_def_codegen;
void NodeProgram::codegen(){
    for (NodeFunctionDef* func_def : func_defs) {
        current_func_def_codegen = func_def;
        func_def->codegen();
    }
}

void NodeExprStmt::codegen(){
    _expr->codegen();
}

void NodeAssign::codegen() {
    rhs->codegen();  // Generate code for the right-hand side expression
    emit_push(accum_reg);  // Save the value of the RHS temporarily
    lhs->codegen();  // Generate code for the left-hand side
    emit_pop(scratch_reg);  // Restore the RHS value to a temporary register
    emit_store_to_memory(accum_reg, scratch_reg);  // Store the RHS value in the LHS address
    emit_move(accum_reg, scratch_reg);  // Move RHS value back to accum_reg
}

int round16(int n){
    if((n % 16) != 0){return (16-(n%16)) + n;}
    return n;
}

void NodeId::codegen() {
    int byte_number = var_map[current_function][id]->offSet;
    emit_add_to_fp(accum_reg, byte_number);  // Calculate address relative to frame pointer

    if (!(parent && (parent->is_NodeAssign() || parent->is_NodeAddressOf()))) {
        emit_load_from_address(accum_reg, accum_reg);  // Load value from the address
    }
}

void NodeReturnStmt::codegen() {
    _expr->codegen();          // Generate code for the return expression
    emit_jump(".L.return." + current_func_def_codegen->declarator);
}

void NodeForStmt::codegen() {
    string cond = ".L.FCOND_" + to_string(counter);
    string after = ".L.FAFTER_" + to_string(counter++);
    Init->codegen();  // Initialization
    emit_label(cond);
    Cond->codegen();  // Condition
    emit_cond_jump(after);  // Jump if false
    Body->codegen();  // Loop body
    if (Increment) {
        Increment->codegen();  // Increment step
    }
    emit_jump(cond);  // Loop back
    emit_label(after);  // Exit point
}


void NodeWhileStmt::codegen() {
    string cond = ".L.WCOND_" + to_string(counter);
    string after = ".L.WAFTER_" + to_string(counter);
    counter++;

    emit_label(cond);
    _expr->codegen();  // Evaluate the condition
    emit_cond_jump(after);  // Jump if false
    _stmt->codegen();  // Body of the loop
    emit_jump(cond);  // Loop back
    emit_label(after);  // Exit point
}


void NodeBlockStmt::codegen(){
    for(NodeStmt* stmt : stmt_list) {
        stmt->codegen();
    }
}

void NodeDecl::codegen() {
    if (initializer) {
        initializer->codegen();  // Generate code for the initializer expression
        int offset = var_map[current_function][varName]->offSet;
        emit_store_to_stack(accum_reg, offset);  // Store the value on the stack
    }
}

void NodeDeclList::codegen() {
    for(NodeDecl* decl : decls){
        decl->codegen();
    }
}

void NodeFunctionCall::codegen() {
    for (size_t i = 0; i < args.size(); i++) {
        args[i]->codegen();  // Generate code for the argument
        emit_push(accum_reg);  // Push the argument onto the stack
    }
    // Pop arguments into the appropriate registers (x0 to x7)
    for (int i = args.size() - 1; i >= 0; i--) {
        emit_pop("x" + to_string(i));
    }
    emit_function_call(functionName);  // Branch to the function
    // Ensure the return value in x0 is moved to accum_reg (x9)
    emit_move(accum_reg, "x0");
}

void NodeNullStmt::codegen(){
    ;
}

static void emit_prologue() {
    cout << "  stp x29, x30, [sp, -16]!" << endl;
    cout << "  mov x29, sp" << endl;

    // Allocate stack space for local variables
    int stackSize = round16(var_map[current_function].size() * 8);
    cout << "  sub sp, sp, #" << stackSize << endl;
}

static void emit_epilogue() {
    cout << ".L.return." << current_func_def_codegen->declarator << ":" << endl;
    cout << "  add sp, sp, #" << round16(var_map[current_function].size() * 8) << endl;
    cout << "  ldp x29, x30, [sp], 16" << endl;
    cout << "  mov " << return_reg << ", " << accum_reg << endl;;
    cout << "  ret" << endl;
}

void NodeFunctionDef::codegen() {
    current_function = declarator;

    cout << ".global " << declarator << endl;
    cout << declarator << ":" << endl;

    emit_prologue();

    // Defensive parameter handling
    for (size_t i = 0; i < params.size(); ++i) {
        NodeDecl* param = params[i];
        if (!param) {
            cerr << "[ERROR] Null param at index " << i << " in function " << declarator << endl;
            exit(1);
        }

        const string& name = param->varName;
        if (var_map[current_function].count(name) == 0) {
            cerr << "[ERROR] Parameter '" << name << "' not found in var_map for function '" << current_function << "'" << endl;
            exit(1);
        }

        int offset = var_map[current_function][name]->offSet;
        if (i < 8) {
            emit_store_to_stack("x" + to_string(i), offset);
        }
        // Skipping spill case (i >= 8) for now
    }

    if (body) body->codegen();
    emit_epilogue();
}

void build_LLVM_IR(llvm::Module* module, llvm::IRBuilder<>* llvmBuilder) {
    // Placeholder for LLVM IR generation logic
    // This function should traverse the AST and generate LLVM IR using the provided module
    module->getOrInsertFunction("main", llvm::FunctionType::get(llvm::Type::getInt32Ty(module->getContext()), false));
    llvm::Function* mainFunc = module->getFunction("main");
    llvm::BasicBlock* entry = llvm::BasicBlock::Create(module->getContext(), "entry", mainFunc);
    llvmBuilder->SetInsertPoint(entry);
    llvmBuilder->CreateRet(llvm::ConstantInt::get(llvm::Type::getInt32Ty(module->getContext()), 7));
}

void do_codegen(Node* root) {
    // root->codegen();  // Generate code for the program's AST
    llvm::LLVMContext* llvmContext = new llvm::LLVMContext();
    llvm::Module* llvmModule = new llvm::Module("my_module", *llvmContext);
    llvm::IRBuilder<>* builder = new llvm::IRBuilder<>(*llvmContext);

    build_LLVM_IR(llvmModule, builder);

    std::ofstream llFile("output.ll");
    std::string llContents;
    llvm::raw_string_ostream ostr(llContents);
    llvmModule->print(ostr, nullptr);

    llFile << llContents;
    llFile.close();

    delete builder;
    delete llvmModule;
    delete llvmContext;
}


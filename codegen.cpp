#include "leocc.hpp"
#include <cassert>
#include <llvm/IR/Module.h>
#include <llvm/IR/IRBuilder.h>
#include <fstream>
#include <cstdlib>
#include <unordered_map>
#include <llvm/IR/Verifier.h>
#include <llvm/Support/raw_ostream.h>
#include <llvm/Support/Host.h>
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

namespace leocc_llvm {

// Small codegen context for LLVM
struct CG {
  llvm::LLVMContext* ctx;
  llvm::Module* mod;
  llvm::IRBuilder<>* irb;
  llvm::Function* curFn = nullptr;

  // locals: name -> alloca
  std::unordered_map<std::string, llvm::AllocaInst*> locals;
};

static llvm::Type* LEO_i64(llvm::LLVMContext& ctx) {
  return llvm::Type::getInt64Ty(ctx);
}

static llvm::AllocaInst* createEntryAlloca(CG& cg, const std::string& name) {
  llvm::IRBuilder<> tmp(&cg.curFn->getEntryBlock(),
                        cg.curFn->getEntryBlock().begin());
  return tmp.CreateAlloca(llvm::Type::getInt64Ty(*cg.ctx), nullptr, name);
}

static llvm::Value* as_i64(CG& cg, llvm::Value* v) {
  if (v->getType()->isIntegerTy(64)) return v;
  if (v->getType()->isIntegerTy(32))
    return cg.irb->CreateSExt(v, llvm::Type::getInt64Ty(*cg.ctx));
  return v;
}

static llvm::Value* bool_i1_to_i64(CG& cg, llvm::Value* v) {
  return cg.irb->CreateZExt(v, llvm::Type::getInt64Ty(*cg.ctx), "bool64");
}

// Turn any i64 into an i1 "is nonzero?"
static llvm::Value* as_bool_i1(CG& cg, llvm::Value* v) {
  v = as_i64(cg, v);
  auto* zero = llvm::ConstantInt::get(LEO_i64(*cg.ctx), 0);
  return cg.irb->CreateICmpNE(v, zero, "tobool");
}

// Forward decls
static llvm::Value* genR(CG& cg, NodeExpr* e);
static void gen(CG& cg, NodeStmt* s);

// Program → emit each function
static void gen(CG& cg, NodeProgram* p) {
  for (auto* f : p->func_defs) {
    cg.locals.clear();

    auto* fnTy = llvm::FunctionType::get(LEO_i64(*cg.ctx), /*isVarArg=*/false);
    auto* fn   = llvm::Function::Create(fnTy, llvm::Function::ExternalLinkage,
                                        f->declarator, cg.mod);

    cg.curFn = fn;
    auto* entry = llvm::BasicBlock::Create(*cg.ctx, "entry", fn);
    cg.irb->SetInsertPoint(entry);

    // Body
    if (f->body) {
      for (auto* st : f->body->stmt_list) gen(cg, st);
    }

    // Default return 0 if none
    if (!entry->getTerminator()) {
      cg.irb->CreateRet(llvm::ConstantInt::get(LEO_i64(*cg.ctx), 0));
    }

    llvm::verifyFunction(*fn);
    cg.curFn = nullptr;
  }
}

// Stmt → return / block / null / decls / expr-stmt
static void gen(CG& cg, NodeStmt* s) {
  if (auto* r = dynamic_cast<NodeReturnStmt*>(s)) {
    llvm::Value* v = genR(cg, r->_expr);
    cg.irb->CreateRet(v);
    return;
  }
  if (auto* b = dynamic_cast<NodeBlockStmt*>(s)) {
    for (auto* st : b->stmt_list) gen(cg, st);
    return;
  }
  if (dynamic_cast<NodeNullStmt*>(s)) return;

  if (auto* dl = dynamic_cast<NodeDeclList*>(s)) {
    for (auto* d : dl->decls) {
      const std::string& name = d->varName;
      auto* slot = createEntryAlloca(cg, name);
      cg.locals[name] = slot;
      if (d->initializer) {
        llvm::Value* init = genR(cg, d->initializer);
        cg.irb->CreateStore(init, slot);
      } else {
        cg.irb->CreateStore(llvm::ConstantInt::get(LEO_i64(*cg.ctx), 0), slot);
      }
    }
    return;
  }

  if (auto* es = dynamic_cast<NodeExprStmt*>(s)) {
    (void)genR(cg, es->_expr);
    return;
  }

  // while (expr) stmt
  if (auto* w = dynamic_cast<NodeWhileStmt*>(s)) {
    auto* fn = cg.curFn;

    auto* condBB  = llvm::BasicBlock::Create(*cg.ctx, "while.cond", fn);
    auto* bodyBB  = llvm::BasicBlock::Create(*cg.ctx, "while.body");
    auto* afterBB = llvm::BasicBlock::Create(*cg.ctx, "while.after");

    // jump to condition
    cg.irb->CreateBr(condBB);

    // cond:
    cg.irb->SetInsertPoint(condBB);
    llvm::Value* condV = as_bool_i1(cg, genR(cg, w->_expr));
    cg.irb->CreateCondBr(condV, bodyBB, afterBB);

    // body:
    fn->getBasicBlockList().push_back(bodyBB);
    cg.irb->SetInsertPoint(bodyBB);
    gen(cg, w->_stmt);
    if (!cg.irb->GetInsertBlock()->getTerminator())
        cg.irb->CreateBr(condBB);  // loop back

    // after:
    fn->getBasicBlockList().push_back(afterBB);
    cg.irb->SetInsertPoint(afterBB);
    return;
  }

  // for (Init; Cond; Increment) Body
  if (auto* f = dynamic_cast<NodeForStmt*>(s)) {
    auto* fn = cg.curFn;

    auto* condBB  = llvm::BasicBlock::Create(*cg.ctx, "for.cond", fn);
    auto* bodyBB  = llvm::BasicBlock::Create(*cg.ctx, "for.body");
    auto* incBB   = llvm::BasicBlock::Create(*cg.ctx, "for.inc");
    auto* afterBB = llvm::BasicBlock::Create(*cg.ctx, "for.after");

    // init
    if (f->Init) gen(cg, f->Init);

    // to cond
    cg.irb->CreateBr(condBB);

    // cond:
    cg.irb->SetInsertPoint(condBB);
    llvm::Value* condV = nullptr;
    if (auto* es = dynamic_cast<NodeExprStmt*>(f->Cond)) {
        condV = es->_expr ? as_bool_i1(cg, genR(cg, es->_expr)) : llvm::ConstantInt::getTrue(*cg.ctx);
    } else if (dynamic_cast<NodeNullStmt*>(f->Cond)) {
        condV = llvm::ConstantInt::getTrue(*cg.ctx); // empty = true
    } else {
        condV = llvm::ConstantInt::getTrue(*cg.ctx); // fallback
    }
    cg.irb->CreateCondBr(condV, bodyBB, afterBB);

    // body:
    fn->getBasicBlockList().push_back(bodyBB);
    cg.irb->SetInsertPoint(bodyBB);
    gen(cg, f->Body);
    if (!cg.irb->GetInsertBlock()->getTerminator())
        cg.irb->CreateBr(incBB);

    // inc:
    fn->getBasicBlockList().push_back(incBB);
    cg.irb->SetInsertPoint(incBB);
    if (f->Increment) (void)genR(cg, f->Increment);
    cg.irb->CreateBr(condBB);

    // after:
    fn->getBasicBlockList().push_back(afterBB);
    cg.irb->SetInsertPoint(afterBB);
    return;
  }


  // Other statements (while/for/if) can be added next
}

// Exprs: id/assign/nums/arithmetic/comparisons
static llvm::Value* genR(CG& cg, NodeExpr* e) {
  // identifier load
  if (auto* id = dynamic_cast<NodeId*>(e)) {
    auto it = cg.locals.find(id->id);
    if (it == cg.locals.end()) {
      it = cg.locals.emplace(id->id, createEntryAlloca(cg, id->id)).first;
      cg.irb->CreateStore(llvm::ConstantInt::get(LEO_i64(*cg.ctx), 0), it->second);
    }
    return cg.irb->CreateLoad(LEO_i64(*cg.ctx), it->second, id->id + ".val");
  }

  // assignment (lhs = rhs) — only NodeId LHS for now
  if (auto* asn = dynamic_cast<NodeAssign*>(e)) {
    if (auto* lid = dynamic_cast<NodeId*>(asn->lhs)) {
      auto it = cg.locals.find(lid->id);
      llvm::AllocaInst* slot = nullptr;
      if (it == cg.locals.end()) {
        slot = createEntryAlloca(cg, lid->id);
        cg.locals[lid->id] = slot;
        cg.irb->CreateStore(llvm::ConstantInt::get(LEO_i64(*cg.ctx), 0), slot);
      } else {
        slot = it->second;
      }
      llvm::Value* rhs = as_i64(cg, genR(cg, asn->rhs));
      cg.irb->CreateStore(rhs, slot);
      return rhs; // assignment expression evaluates to assigned value
    }
  }

  // number literal
  if (auto* n = dynamic_cast<NodeNum*>(e)) {
    return llvm::ConstantInt::get(LEO_i64(*cg.ctx),
                                  (int64_t)n->num_literal,
                                  /*isSigned=*/true);
  }

  // a + b
  if (auto* a = dynamic_cast<NodeAdd*>(e)) {
    auto* L = as_i64(cg, genR(cg, a->lhs));
    auto* R = as_i64(cg, genR(cg, a->rhs));
    return cg.irb->CreateAdd(L, R, "addtmp");
  }

  // a - b
  if (auto* s = dynamic_cast<NodeSub*>(e)) {
    auto* L = as_i64(cg, genR(cg, s->lhs));
    auto* R = as_i64(cg, genR(cg, s->rhs));
    return cg.irb->CreateSub(L, R, "subtmp");
  }

  // a * b
  if (auto* m = dynamic_cast<NodeMul*>(e)) {
    auto* L = as_i64(cg, genR(cg, m->lhs));
    auto* R = as_i64(cg, genR(cg, m->rhs));
    return cg.irb->CreateMul(L, R, "multmp");
  }

  // a / b (signed)
  if (auto* d = dynamic_cast<NodeDiv*>(e)) {
    auto* L = as_i64(cg, genR(cg, d->lhs));
    auto* R = as_i64(cg, genR(cg, d->rhs));
    return cg.irb->CreateSDiv(L, R, "divtmp");
  }

  // comparisons -> icmp i1 -> zext i64
  if (auto* n = dynamic_cast<NodeLT*>(e)) {
    auto* L = as_i64(cg, genR(cg, n->lhs));
    auto* R = as_i64(cg, genR(cg, n->rhs));
    return bool_i1_to_i64(cg, cg.irb->CreateICmpSLT(L, R, "cmplt"));
  }
  if (auto* n = dynamic_cast<NodeLTE*>(e)) {
    auto* L = as_i64(cg, genR(cg, n->lhs));
    auto* R = as_i64(cg, genR(cg, n->rhs));
    return bool_i1_to_i64(cg, cg.irb->CreateICmpSLE(L, R, "cmple"));
  }
  if (auto* n = dynamic_cast<NodeGT*>(e)) {
    auto* L = as_i64(cg, genR(cg, n->lhs));
    auto* R = as_i64(cg, genR(cg, n->rhs));
    return bool_i1_to_i64(cg, cg.irb->CreateICmpSGT(L, R, "cmpgt"));
  }
  if (auto* n = dynamic_cast<NodeGTE*>(e)) {
    auto* L = as_i64(cg, genR(cg, n->lhs));
    auto* R = as_i64(cg, genR(cg, n->rhs));
    return bool_i1_to_i64(cg, cg.irb->CreateICmpSGE(L, R, "cmpge"));
  }
  if (auto* n = dynamic_cast<NodeEE*>(e)) {
    auto* L = as_i64(cg, genR(cg, n->lhs));
    auto* R = as_i64(cg, genR(cg, n->rhs));
    return bool_i1_to_i64(cg, cg.irb->CreateICmpEQ(L, R, "cmpeq"));
  }
  if (auto* n = dynamic_cast<NodeNE*>(e)) {
    auto* L = as_i64(cg, genR(cg, n->lhs));
    auto* R = as_i64(cg, genR(cg, n->rhs));
    return bool_i1_to_i64(cg, cg.irb->CreateICmpNE(L, R, "cmpne"));
  }

  // fallback
  return llvm::UndefValue::get(LEO_i64(*cg.ctx));
}

// Entry point for LLVM backend (uses AST root)
void build_LLVM_IR(Node* root, llvm::Module* module, llvm::IRBuilder<>* llvmBuilder) {
  module->setTargetTriple(llvm::sys::getDefaultTargetTriple());

  CG cg{ &module->getContext(), module, llvmBuilder };
  if (auto* prog = dynamic_cast<NodeProgram*>(root)) {
    gen(cg, prog);
  } else {
    std::cerr << "LLVM backend: root is not a NodeProgram\n";
    std::exit(1);
  }
}

} // namespace leocc_llvm

// ========================================================
// ================== BACKEND SWITCHER ====================
// ========================================================
void do_codegen(Node* root) {
  const char* which = std::getenv("LEO_BACKEND");
  bool use_llvm = (which && std::string(which) == "llvm");

  if (!use_llvm) {
    // ARM64 path: print assembly to stdout (your existing tests)
    root->codegen();
    return;
  }

  // LLVM path: write output.ll
  auto* llvmContext = new llvm::LLVMContext();
  auto* llvmModule  = new llvm::Module("my_module", *llvmContext);
  auto* builder     = new llvm::IRBuilder<>(*llvmContext);

  leocc_llvm::build_LLVM_IR(root, llvmModule, builder);

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
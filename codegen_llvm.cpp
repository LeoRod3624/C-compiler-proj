#include "leocc.hpp"
#include <cassert>
#include <llvm/IR/Module.h>
#include <llvm/IR/IRBuilder.h>
#include <fstream>
#include <cstdlib>
#include <unordered_map>
#include <llvm/IR/Verifier.h>
#include <llvm/Support/raw_ostream.h>
#include <llvm/TargetParser/Host.h>

// Small codegen context for LLVM
struct CG {
  llvm::LLVMContext* ctx;
  llvm::Module* mod;
  llvm::IRBuilder<>* irb;
  llvm::Function* curFn = nullptr;

  // locals: name -> alloca
  std::unordered_map<std::string, llvm::AllocaInst*> locals;

  // functions by name (for calls, multi-pass, etc.)
  std::unordered_map<std::string, llvm::Function*> functions;
};

static llvm::Type* leoType(CG& cg, CType* ct) {
  // int -> i64
  if (ct && ct->isIntType())
    return llvm::Type::getInt64Ty(*cg.ctx);
  // pointer 
  if (ct->isPtrType()) {
    CType* inner = ((CPtrType*)ct)->referenced_type;
    llvm::Type* elemTy = leoType(cg, inner);   // RECURSE
    return llvm::PointerType::get(elemTy, 0); 
  }

  // Fallback: just use i64
  return llvm::Type::getInt64Ty(*cg.ctx);
}

static llvm::Type* LEO_i64(llvm::LLVMContext& ctx) {
  return llvm::Type::getInt64Ty(ctx);
}

static llvm::AllocaInst* createEntryAlloca(CG& cg, llvm::Type* ty, const std::string& name) { llvm::IRBuilder<> tmp(&cg.curFn->getEntryBlock(), cg.curFn->getEntryBlock().begin());
  return tmp.CreateAlloca(ty, nullptr, name);
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
  // ---------- Pass 1: create all functions with correct signatures ----------
  for (auto* f : p->func_defs) {
    // For now: every param is i64 (matches your int semantics)
    std::vector<llvm::Type*> paramTypes;
    paramTypes.reserve(f->params.size());
    for (auto* param : f->params) {
      (void)param; // currently unused, but later you can inspect c_type
      paramTypes.push_back(LEO_i64(*cg.ctx));
    }

    auto* fnTy = llvm::FunctionType::get(LEO_i64(*cg.ctx),paramTypes,/*isVarArg=*/false);

    auto* fn = llvm::Function::Create(fnTy,llvm::Function::ExternalLinkage,f->declarator,cg.mod);

    cg.functions[f->declarator] = fn;

    // Give arguments nice names based on the AST params
    unsigned idx = 0;
    for (auto& arg : fn->args()) {
      arg.setName(f->params[idx++]->varName);
    }
  }

  // ---------- Pass 2: generate bodies ----------
  for (auto* f : p->func_defs) {
    cg.locals.clear();
    auto* fn = cg.functions.at(f->declarator);
    cg.curFn = fn;

    // Create entry block and set insertion point
    auto* entry = llvm::BasicBlock::Create(*cg.ctx, "entry", fn);
    cg.irb->SetInsertPoint(entry);

    // Create allocas for parameters and store incoming values into them
    unsigned idx = 0;
    for (auto& arg : fn->args()) {
      NodeDecl* paramDecl = f->params[idx];
      const std::string& name = paramDecl->varName;

      // Use your C type to pick the right LLVM type (int / pointer / etc.)
      llvm::Type* ty = leoType(cg, paramDecl->c_type);

      auto* slot = createEntryAlloca(cg, ty, name);
      cg.locals[name] = slot;

      cg.irb->CreateStore(&arg, slot);

      ++idx;
    }

    // Body statements
    if (f->body) {
      for (auto* st : f->body->stmt_list)
        gen(cg, st);
    }

    // Default return 0 if no terminator in the current block
    if (!cg.irb->GetInsertBlock()->getTerminator()) {
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

    // Pick LLVM type based on C type (int / pointer / pointer-to-pointer, etc.)
    llvm::Type* ty = leoType(cg, d->c_type);

    auto* slot = createEntryAlloca(cg, ty, name);
    cg.locals[name] = slot;

    if (d->initializer) {
      llvm::Value* init = genR(cg, d->initializer);
      // If needed you can cast init here; for now we assume types match.
      cg.irb->CreateStore(init, slot);
    } else {
      // Default-init ints to 0, pointers to null
      if (ty->isIntegerTy(64)) {
        cg.irb->CreateStore(
          llvm::ConstantInt::get(ty, 0),
          slot
        );
      } else if (ty->isPointerTy()) {
        cg.irb->CreateStore(
          llvm::ConstantPointerNull::get(
            llvm::cast<llvm::PointerType>(ty)
          ),
          slot
        );
      }
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

    bodyBB->insertInto(fn);
    cg.irb->SetInsertPoint(bodyBB);
    gen(cg, w->_stmt);
    if (!cg.irb->GetInsertBlock()->getTerminator())
        cg.irb->CreateBr(condBB);  // loop back

    // after:
    afterBB->insertInto(fn);
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
    bodyBB->insertInto(fn);
    cg.irb->SetInsertPoint(bodyBB);
    gen(cg, f->Body);
    if (!cg.irb->GetInsertBlock()->getTerminator())
        cg.irb->CreateBr(incBB);

    // inc:
    incBB->insertInto(fn);
    cg.irb->SetInsertPoint(incBB);
    if (f->Increment) (void)genR(cg, f->Increment);
    cg.irb->CreateBr(condBB);

    // after:
    afterBB->insertInto(fn);
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
    std::cerr << "LLVM backend: unknown local '" << id->id << "'\n";
    std::exit(1);
  }

  llvm::AllocaInst* slot = it->second;
  llvm::Type* elemTy = slot->getAllocatedType();
  return cg.irb->CreateLoad(elemTy, slot, id->id + ".val");
  }

  // &expr  (for now: only &id)
if (auto* a = dynamic_cast<NodeAddressOf*>(e)) {
  if (auto* id = dynamic_cast<NodeId*>(a->_expr)) {
    auto it = cg.locals.find(id->id);
    if (it == cg.locals.end()) {
      std::cerr << "LLVM backend: & of unknown local '" << id->id << "'\n";
      std::exit(1);
    }
    llvm::AllocaInst* slot = it->second;
    // alloca has type T*; &x is just that pointer
    return slot;
  }

  std::cerr << "LLVM backend: &expr currently only supports &id\n";
  std::exit(1);
}


// *expr
if (auto* d = dynamic_cast<NodeDereference*>(e)) {
  llvm::Value* ptr = genR(cg, d->_expr);  // should be some T*
  auto* ptrTy = llvm::dyn_cast<llvm::PointerType>(ptr->getType());
  if (!ptrTy) {
    std::cerr << "LLVM backend: dereference of non-pointer value\n";
    std::exit(1);
  }
  llvm::Type* elemTy = leoType(cg, d->c_type);
  return cg.irb->CreateLoad(elemTy, ptr);

}


  // assignment (lhs = rhs)
if (auto* asn = dynamic_cast<NodeAssign*>(e)) {
  // Case 1: simple x = rhs;
  if (auto* lid = dynamic_cast<NodeId*>(asn->lhs)) {
    auto it = cg.locals.find(lid->id);
    if (it == cg.locals.end()) {
      std::cerr << "LLVM backend: store to unknown local '" << lid->id << "'\n";
      std::exit(1);
    }
    llvm::AllocaInst* slot = it->second;
    llvm::Type* elemTy = slot->getAllocatedType();

    llvm::Value* rhs = genR(cg, asn->rhs);
    if (rhs->getType() != elemTy) {
      // You can add casts here later; for now, assume well-typed
    }
    cg.irb->CreateStore(rhs, slot);
    return rhs;
  }

  // Case 2: *p = rhs;
  if (auto* d = dynamic_cast<NodeDereference*>(asn->lhs)) {
    llvm::Value* addr = genR(cg, d->_expr); // pointer
    auto* ptrTy = llvm::dyn_cast<llvm::PointerType>(addr->getType());
    if (!ptrTy) {
      std::cerr << "LLVM backend: lhs of deref-assign is not a pointer\n";
      std::exit(1);
    }
    llvm::Type* elemTy = cg.irb->getInt64Ty();

    llvm::Value* rhs = genR(cg, asn->rhs);
    if (rhs->getType() != elemTy) {
      // Again, could add casts if needed
    }
    cg.irb->CreateStore(rhs, addr);
    return rhs;
  }

  std::cerr << "LLVM backend: unsupported assignment LHS kind\n";
  std::exit(1);
}


  // number literal
  if (auto* n = dynamic_cast<NodeNum*>(e)) {
    return llvm::ConstantInt::get(LEO_i64(*cg.ctx), (int64_t)n->num_literal,/*isSigned=*/true);
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

    // function call: f(args...)
  if (auto* call = dynamic_cast<NodeFunctionCall*>(e)) {
    // Look up the callee by name
    llvm::Function* callee = cg.mod->getFunction(call->functionName);
    if (!callee) {
      std::cerr << "LLVM backend: unknown function '"
                << call->functionName << "'\n";
      std::exit(1);
    }

    // Generate arguments
    std::vector<llvm::Value*> argVals;
    argVals.reserve(call->args.size());
    for (auto* argExpr : call->args) {
      llvm::Value* v = genR(cg, argExpr);
      argVals.push_back(as_i64(cg, v)); // ensure i64
    }

    // Emit the call
    return cg.irb->CreateCall(callee, argVals, "calltmp");
  }


  // fallback
  return llvm::UndefValue::get(LEO_i64(*cg.ctx));
}

// Entry point for LLVM backend (uses AST root)
void build_LLVM_IR(Node* root, llvm::Module* module, llvm::IRBuilder<>* llvmBuilder) {
  llvm::Triple triple(llvm::sys::getDefaultTargetTriple());
  module->setTargetTriple(triple);

  CG cg{ &module->getContext(), module, llvmBuilder };
  if (auto* prog = dynamic_cast<NodeProgram*>(root)) {
    gen(cg, prog);
  } else {
    std::cerr << "LLVM backend: root is not a NodeProgram\n";
    std::exit(1);
  }
}


void do_codegen_llvm(Node* root) {
  // LLVM path: write output.ll
  auto* llvmContext = new llvm::LLVMContext();
  auto* llvmModule  = new llvm::Module("my_module", *llvmContext);
  auto* builder     = new llvm::IRBuilder<>(*llvmContext);

  build_LLVM_IR(root, llvmModule, builder);

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
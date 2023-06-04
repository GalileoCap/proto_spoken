#include "codegen.h"
#include "ast.h"
#include "parser.h"

std::unique_ptr<llvm::LLVMContext> TheContext;
std::unique_ptr<llvm::Module> TheModule;
std::unique_ptr<llvm::IRBuilder<>> Builder;
std::list<NamedValues_t> NamedValues;

llvm::Value* LogErrorV(const char *str) {
  LogError(str);
  return nullptr;
}

llvm::Type* LogErrorT(const char *str) {
  LogError(str);
  return nullptr;
}

llvm::Type* getType(const std::string &typeStr) {
  if (typeStr == "int")
    return llvm::Type::getInt32Ty(*TheContext);
  //else if (typeStr == "double") // TODO: Support in NumberExprAST
    //return llvm::Type::getDoubleTy(*TheContext);
  else if (typeStr == "bool")
    return llvm::Type::getInt1Ty(*TheContext);
  else if (typeStr == "void")
    return llvm::Type::getVoidTy(*TheContext);
  else return LogErrorT("unknown type");
}

llvm::Value* NumberExprAST::codegen() {
  return llvm::ConstantInt::get(*TheContext, llvm::APInt(32, Val, true));
}

llvm::Value* VariableExprAST::codegen() {
  // Look this variable upstream in the scopes
 for (auto&& scope : NamedValues) {
    llvm::Value *V = scope[Name];
    if (V)
      return V;
  }

  return LogErrorV("unknown variable name");
}

//llvm::Value* ParamAST::codegen() {
  //auto type = getType(Type);
  //if (!type)
    //return nullptr;
//
  //return
//}

llvm::Value* binOpInt32(char op, llvm::Value *L, llvm::Value *R) {
  switch (op) {
  case '+':
    return Builder->CreateAdd(L, R, "addtmp");
  case '-':
    return Builder->CreateSub(L, R, "subtmp");
  case '*':
    return Builder->CreateMul(L, R, "multmp");
  case '<':
    return Builder->CreateICmpULT(L, R, "cmptmp");
  default:
    return LogErrorV("invalid binary operator");
  }
}

llvm::Value* binOpDouble(char op, llvm::Value *L, llvm::Value *R) {
  switch (op) {
  case '+':
    return Builder->CreateFAdd(L, R, "addtmp");
  case '-':
    return Builder->CreateFSub(L, R, "subtmp");
  case '*':
    return Builder->CreateFMul(L, R, "multmp");
  case '<':
    L = Builder->CreateFCmpULT(L, R, "cmptmp");
    // Convert bool 0/1 to double 0.0 or 1.0
    return Builder->CreateUIToFP(L, getType("double"), "booltmp");
  default:
    return LogErrorV("invalid binary operator");
  }
}

llvm::Value* BinaryExprAST::codegen() {
  llvm::Value *L = LHS->codegen(),
              *R = RHS->codegen();

  if (!L || !R || L->getType() != R->getType())
    return nullptr;

  if (L->getType() == getType("int"))
    return binOpInt32(Op, L, R);
  else if (L->getType() == getType("double"))
    return binOpDouble(Op, L, R);
  else
   return LogErrorV("invalid type on binary operator");
}

llvm::Value* CallExprAST::codegen() {
  // Look up the name in the global module table.
  llvm::Function *func = TheModule->getFunction(Callee);
  if (!func)
    return LogErrorV("Unknown function referenced");

  // If argument mismatch error.
  if (func->arg_size() != Args.size())
    return LogErrorV("Incorrect # arguments passed");

  std::vector<llvm::Value*> argsV;
  for (unsigned i = 0, e = Args.size(); i != e; ++i) {
    auto v =Args[i]->codegen();
    if (!v)
      return nullptr;
    argsV.push_back(v);
  }

  return Builder->CreateCall(func, argsV, "calltmp");
}

llvm::Function* PrototypeAST::codegen() {
  std::vector<llvm::Type*> types;
  for (auto&& param : Params) {
    auto type = getType(param->Type);
    if (!type)
      return nullptr;
    types.push_back(type);
    //auto v = param->codegen();
    //types.push_back(v->getType());
  }

  auto FT = llvm::FunctionType::get(getType(Type), types, false);

  auto F = llvm::Function::Create(FT, llvm::Function::ExternalLinkage, Name, TheModule.get());

  // Set names for all arguments.
  unsigned Idx = 0;
  for (auto &Arg : F->args())
    Arg.setName(Params[Idx++]->Name);

  return F;
}

llvm::Function* FunctionAST::codegen() {
  // First, check for an existing function from a previous declaration.
  auto TheFunction = Proto->codegen();

  if (!TheFunction)
    return nullptr;

  // Create a new basic block to start insertion into.
  auto BB = llvm::BasicBlock::Create(*TheContext, "entry", TheFunction);
  Builder->SetInsertPoint(BB);

  // Record the function arguments in the NamedValues map.
  NamedValues.emplace_back();
  for (auto &Arg : TheFunction->args())
    NamedValues.back()[std::string(Arg.getName())] = &Arg;

  bool success = false;
  if (auto RetVal = Body->codegen()) {
    // Finish off the function.
    Builder->CreateRet(RetVal);

    // Validate the generated code, checking for consistency.
    llvm::verifyFunction(*TheFunction);

    success = true;
  }

  NamedValues.pop_back(); // Remove params from NamedValues
  if (success)
    return TheFunction;

  // Error reading body, remove function.
  TheFunction->eraseFromParent();
  return nullptr;
}

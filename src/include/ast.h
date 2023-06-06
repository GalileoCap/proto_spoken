#pragma once

#include "utils.h"

// ExprAST - Base class for all expression nodes.
struct ExprAST {
  virtual ~ExprAST() = default;
  virtual llvm::Value* codegen() = 0;
};

// BoolExprAST - Expression class for boolean literals, "false" or "true"
struct BoolExprAST : ExprAST {
  bool Val;

  BoolExprAST(bool Val) : Val(Val) {}
  llvm::Value* codegen() override;
};

// IntExprAST - Expression class for integer literals like "1".
struct IntExprAST : ExprAST {
  int Val;

  IntExprAST(int Val) : Val(Val) {}
  llvm::Value* codegen() override;
};

// FloatExprAST - Expression class for floating point literals like "1.0".
struct FloatExprAST : ExprAST {
  double Val;

  FloatExprAST(double Val) : Val(Val) {}
  llvm::Value* codegen() override;
};

// VariableExprAST - Expression class for referencing a variable, like "a".
struct VariableExprAST : ExprAST {
  std::string Name;

  VariableExprAST(const std::string &Name) : Name(Name) {}
  llvm::Value* codegen() override;
};

// BinaryExprAST - Expression class for a binary operator.
struct BinaryExprAST : ExprAST {
  char Op;
  std::unique_ptr<ExprAST> LHS, RHS;

  BinaryExprAST(char Op, std::unique_ptr<ExprAST> LHS, std::unique_ptr<ExprAST> RHS)
    : Op(Op), LHS(std::move(LHS)), RHS(std::move(RHS)) {}
  llvm::Value* codegen() override;
};

// CallExprAST - Expression class for function calls.
struct CallExprAST : ExprAST {
  std::string Callee;
  std::vector<std::unique_ptr<ExprAST>> Args;

  CallExprAST(const std::string &Callee, std::vector<std::unique_ptr<ExprAST>> Args)
    : Callee(Callee), Args(std::move(Args)) {}
  llvm::Value* codegen() override;
};

// BlockAST - Class to define code blocks
struct BlockExprAST : ExprAST {
  std::vector<std::unique_ptr<ExprAST>> Body;

  BlockExprAST(std::vector<std::unique_ptr<ExprAST>> Body) : Body(std::move(Body)) {}
  llvm::Value* codegen();
};

// ReturnExprAST - Class to define returns
struct ReturnExprAST : ExprAST {
  std::unique_ptr<ExprAST> Expr;

  ReturnExprAST(std::unique_ptr<ExprAST> Expr) : Expr(std::move(Expr)) {}
  llvm::Value* codegen();
};

// ParamAST - Class to define params, like "int x".
struct ParamAST {
  std::string Name, Type;

  ParamAST(const std::string &Name, const std::string &Type) : Name(Name), Type(Type) {}
  //llvm::Value* codegen();
};

// PrototypeAST - This class represents the "prototype" for a function,
// which captures its name, and its parameters.
struct PrototypeAST {
  std::string Name, Type;
  std::vector<std::unique_ptr<ParamAST>> Params;

  PrototypeAST(const std::string &Name, const std::string &Type, std::vector<std::unique_ptr<ParamAST>> Params)
    : Name(Name), Type(Type), Params(std::move(Params)) {}
  llvm::Function* codegen();
};

// FunctionAST - This class represents a function definition itself.
struct FunctionAST {
  std::unique_ptr<PrototypeAST> Proto;
  std::unique_ptr<ExprAST> Body;

  FunctionAST(std::unique_ptr<PrototypeAST> Proto, std::unique_ptr<ExprAST> Body)
    : Proto(std::move(Proto)), Body(std::move(Body)) {}
  llvm::Function* codegen();
};

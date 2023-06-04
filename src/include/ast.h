#pragma once

#include "utils.h"

// ExprAST - Base class for all expression nodes.
struct ExprAST {
  virtual ~ExprAST() = default;
};

// NumberExprAST - Expression class for numeric literals like "1.0".
struct NumberExprAST : ExprAST {
  int Val;

  NumberExprAST(int Val) : Val(Val) {}
};

// VariableExprAST - Expression class for referencing a variable, like "a".
struct VariableExprAST : ExprAST {
  std::string Name;

  VariableExprAST(const std::string &Name) : Name(Name) {}
};

// BinaryExprAST - Expression class for a binary operator.
struct BinaryExprAST : ExprAST {
  char Op;
  std::unique_ptr<ExprAST> LHS, RHS;

  BinaryExprAST(char Op, std::unique_ptr<ExprAST> LHS, std::unique_ptr<ExprAST> RHS)
    : Op(Op), LHS(std::move(LHS)), RHS(std::move(RHS)) {}
};

// CallExprAST - Expression class for function calls.
struct CallExprAST : ExprAST {
  std::string Callee;
  std::vector<std::unique_ptr<ExprAST>> Args;

  CallExprAST(const std::string &Callee, std::vector<std::unique_ptr<ExprAST>> Args)
    : Callee(Callee), Args(std::move(Args)) {}
};

// ParamAST - Class to define params, like "int x".
struct ParamAST {
  std::string Name, Type;

  ParamAST(const std::string &Name, const std::string &Type) : Name(Name), Type(Type) {}
};

// PrototypeAST - This class represents the "prototype" for a function,
// which captures its name, and its parameters.
struct PrototypeAST {
  std::string Name, Type;
  std::vector<std::unique_ptr<ParamAST>> Params;

  PrototypeAST(const std::string &Name, const std::string &Type, std::vector<std::unique_ptr<ParamAST>> Params)
    : Name(Name), Type(Type), Params(std::move(Params)) {}
};

// FunctionAST - This class represents a function definition itself.
struct FunctionAST {
  std::unique_ptr<PrototypeAST> Proto;
  std::unique_ptr<ExprAST> Body;

  FunctionAST(std::unique_ptr<PrototypeAST> Proto, std::unique_ptr<ExprAST> Body)
    : Proto(std::move(Proto)), Body(std::move(Body)) {}
};

#pragma once

#include "ast.h"
#include "utils.h"

extern int CurrTok;
int getNextToken();

extern std::map<char, int> BinopPrecedence;
int getTokPrecedence();

std::unique_ptr<ExprAST> LogError(const char *str);
std::unique_ptr<ParamAST> LogErrorParam(const char *str);
std::unique_ptr<PrototypeAST> LogErrorProto(const char *str);

std::unique_ptr<ExprAST> ParseExpr();
std::unique_ptr<ExprAST> ParseBoolExpr();
std::unique_ptr<ExprAST> ParseIntExpr();
std::unique_ptr<ExprAST> ParseFloatExpr();
std::unique_ptr<ExprAST> ParseBlockExpr();
std::unique_ptr<ExprAST> ParseParenExpr();
std::unique_ptr<ExprAST> ParseReturnExpr();
std::unique_ptr<ExprAST> ParseWordExpr();
std::unique_ptr<ExprAST> ParsePrimary();
std::unique_ptr<ExprAST> ParseCallExpr(const std::string &callee);
std::unique_ptr<ExprAST> ParseBinOpRHS(int exprPrec, std::unique_ptr<ExprAST> LHS);

std::unique_ptr<ParamAST> ParseParam();
std::unique_ptr<PrototypeAST> ParsePrototype();
std::unique_ptr<FunctionAST> ParseFunction();
std::unique_ptr<FunctionAST> ParseTopLevelExpr();

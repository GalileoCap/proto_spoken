#include "parser.h"
#include "lexer.h"

int CurrTok = tok_eol;
std::map<char, int> BinopPrecedence;

int getNextToken() { return CurrTok = gettok(); }

int getTokPrecedence() {
  if (!isascii(CurrTok))
    return -1;

  // Make sure it's a declared binop.
  int tokPrec = BinopPrecedence[CurrTok];
  if (tokPrec <= 0)
    return -1;
  return tokPrec;
}

std::unique_ptr<ExprAST> LogError(const char *str) {
  std::cerr << "Error: " << str << std::endl;
  return nullptr;
}

std::unique_ptr<ParamAST> LogErrorParam(const char *str) {
  LogError(str);
  return nullptr;
}

std::unique_ptr<PrototypeAST> LogErrorProto(const char *str) {
  LogError(str);
  return nullptr;
}

std::unique_ptr<ExprAST> ParseBoolExpr() {
  auto res = std::make_unique<BoolExprAST>(BoolVal);
  getNextToken();
  return std::move(res);
}

std::unique_ptr<ExprAST> ParseIntExpr() {
  auto res = std::make_unique<IntExprAST>(IntVal);
  getNextToken();
  return std::move(res);
}

std::unique_ptr<ExprAST> ParseFloatExpr() {
  auto res = std::make_unique<IntExprAST>(FloatVal);
  getNextToken();
  return std::move(res);
}

std::unique_ptr<ExprAST> ParseParenExpr() {
  getNextToken(); // eat '('

  auto res = ParseExpr();
  if (!res)
    return nullptr;

  if (CurrTok != ')')
    return LogError("expected ')'");

  getNextToken(); // eat ')'
  return res;
}

std::unique_ptr<ExprAST> ParseBlockExpr() {
  getNextToken(); // eat '{'

  std::vector<std::unique_ptr<ExprAST>> body;
  while (CurrTok != '}') {
    auto expr = ParseExpr();
    if (!expr)
      return nullptr;

    body.push_back(std::move(expr));
    if (CurrTok == tok_eol)
      getNextToken(); // eat it
  }

  getNextToken(); // eat '}'
  return std::make_unique<BlockExprAST>(std::move(body));
}

std::unique_ptr<ExprAST> ParseWordExpr() {
  std::string word = LastWord;

  getNextToken();
  if (CurrTok == '(') 
    return ParseCallExpr(word);

  // Variable reference
  return std::make_unique<VariableExprAST>(word);
}

std::unique_ptr<ExprAST> ParseCallExpr(const std::string &callee) {
  getNextToken(); // eat '('

  std::vector<std::unique_ptr<ExprAST>> args;
  if (CurrTok != ')') {
    while (true) {
      if (auto arg = ParseExpr())
        args.push_back(std::move(arg));
      else
       return nullptr;

      if (CurrTok == ')')
        break;

      if (CurrTok != ',')
        return LogError("Expected ')' or ',' in argument list");
      getNextToken();
    }
  }

  getNextToken(); // eat ')'

  return std::make_unique<CallExprAST>(callee, std::move(args));
}

std::unique_ptr<ExprAST> ParsePrimary() {
  switch (CurrTok) {
  default:
    return LogError("unknown token when expecting an expression");

  case tok_word:
    return ParseWordExpr();

  case tok_bool:
    return ParseBoolExpr();
  case tok_int:
    return ParseIntExpr();
  case tok_float:
    return ParseFloatExpr();

  case '(':
    return ParseParenExpr();
  case '{':
    return ParseBlockExpr();
  }
}

std::unique_ptr<ExprAST> ParseBinOpRHS(int exprPrec, std::unique_ptr<ExprAST> LHS) {
  // If this is a binop, find its precedence.
  while (true) {
    int tokPrec = getTokPrecedence();

    // If this is a binop that binds at least as tightly as the current binop,
    // consume it, otherwise we are done.
    if (tokPrec < exprPrec)
      return LHS;

    // Okay, we know this is a binop.
    int binOp = CurrTok;
    getNextToken(); // eat binop

    // Parse the primary expression after the binary operator.
    auto RHS = ParsePrimary();
    if (!RHS)
      return nullptr;

    // If BinOp binds less tightly with RHS than the operator after RHS, let
    // the pending operator take RHS as its LHS.
    int nextPrec = getTokPrecedence();
    if (tokPrec < nextPrec) {
      RHS = ParseBinOpRHS(tokPrec + 1, std::move(RHS));
      if (!RHS)
        return nullptr;
    }

    // Merge LHS/RHS.
    LHS = std::make_unique<BinaryExprAST>(binOp, std::move(LHS), std::move(RHS));
  }
}

std::unique_ptr<ExprAST> ParseExpr() {
  auto LHS = ParsePrimary();
  if (!LHS)
    return nullptr;

  return ParseBinOpRHS(0, std::move(LHS));
}

std::unique_ptr<ParamAST> ParseParam() {
  std::string type, name;

  if (CurrTok != tok_word)
    return LogErrorParam("expected type");
  type = LastWord;

  if (getNextToken() != tok_word)
    return LogErrorParam("expected name");
  name = LastWord;
  getNextToken(); // eat the name

  return std::make_unique<ParamAST>(name, type);
}

std::unique_ptr<PrototypeAST> ParsePrototype() {
  if (CurrTok != tok_word || LastWord != "func") // TODO: Also sentences
    return LogErrorProto("expected func in prototype");
  getNextToken(); // eat func

  if (CurrTok != tok_word)
    return LogErrorProto("expected function name in prototype");

  std::string name = LastWord;
  getNextToken();

  if (CurrTok != '(')
    return LogErrorProto("expected '(' in prototype");
  getNextToken(); // eat '('

  std::vector<std::unique_ptr<ParamAST>> params;
  while (CurrTok == tok_word) {
    auto param = ParseParam();
    if (!param)
      return nullptr;
    params.push_back(std::move(param));

    if (CurrTok == ')') 
      break;

    if (CurrTok != ',')
      return LogErrorProto("expected ',' in proto");
    getNextToken(); // eat ','
  }

  if (getNextToken() != tok_word)
    return LogErrorProto("expected return type in prototype");

  std::string type = LastWord;
  getNextToken(); // eat type

  return std::make_unique<PrototypeAST>(name, type, std::move(params));
}

std::unique_ptr<FunctionAST> ParseFunction() {
  getNextToken(); // eat def
  auto proto = ParsePrototype();
  if (!proto)
    return nullptr;

  if (auto body = ParseExpr()) // TODO: Should I limit expressions accepted here?
    return std::make_unique<FunctionAST>(std::move(proto), std::move(body));
  return nullptr;
}

std::unique_ptr<FunctionAST> ParseTopLevelExpr() {
  if (auto expr = ParseExpr()) {
    // Make an anonymous proto.
    auto proto = std::make_unique<PrototypeAST>("__anon_expr", "void", std::vector<std::unique_ptr<ParamAST>>());
    return std::make_unique<FunctionAST>(std::move(proto), std::move(expr));
  }
  return nullptr;
}

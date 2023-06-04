#include "interpret.h"
#include "parser.h"
#include "lexer.h"

static void HandleDefinition() {
  if (ParseFunction()) {
    std::cerr << "Parsed a function definition" << std::endl;
  } else getNextToken(); // Skip token for error recovery.
}

static void HandleTopLevelExpression() {
  // Evaluate a top-level expression into an anonymous function.
  if (ParseTopLevelExpr()) {
    std::cerr << "Parsed a top-level expr" << std::endl;
  } else getNextToken(); // Skip token for error recovery.
}

void MainLoop() {
  std::cerr << "Welcome to SpokenLang!" << std::endl
            << "v0.1" << std::endl;

  while (true) {
    std::cerr << "> ";
    switch (CurrTok) {
    case tok_eof:
      return;

    case tok_eol:
      getNextToken();
      break;

    case tok_def:
      HandleDefinition();
      break;

    default:
      HandleTopLevelExpression();
      break;
    }
  }
}

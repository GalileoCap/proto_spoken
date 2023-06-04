#include "lexer.h"
#include "utils.h"

int CurrTok = tok_eol;

int getNextToken() {
  return CurrTok = gettok();
}

void HandleTopLevelExpression() {
  std::cerr << "Read: " << CurrTok << std::endl;
}

void MainLoop() {
  std::cerr << "Welcome to Spoken!" << std::endl << "v0.1" << std::endl;

  while (true) {
    std::cerr << "> ";
    switch (getNextToken()) {
    case tok_eof:
      return;

    case tok_eol:
      getNextToken();
      break;

    default:
      HandleTopLevelExpression();
      break;
    }
  }
}

int main() {
  MainLoop();
}

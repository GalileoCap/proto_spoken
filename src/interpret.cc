#include "interpret.h"
#include "codegen.h"
#include "parser.h"
#include "lexer.h"

void InitializeModule() {
  // Open a new context and module.
  TheContext = std::make_unique<llvm::LLVMContext>();
  TheModule = std::make_unique<llvm::Module>("spoken", *TheContext); // TODO: ModuleID

  // Create a new builder for the module.
  Builder = std::make_unique<llvm::IRBuilder<>>(*TheContext);
}

static void HandleDefinition() {
  if (auto FnAST = ParseFunction()) {
    if (auto FnIR = FnAST->codegen()) {
      std::cerr << "Read a function definition:" << std::endl;
      FnIR->print(llvm::errs());
      std::cerr << std::endl;
    }
  } else getNextToken(); // Skip token for error recovery.
}

static void HandleTopLevelExpression() {
  // Evaluate a top-level expression into an anonymous function.
  if (auto FnAST = ParseTopLevelExpr()) {
    if (auto FnIR = FnAST->codegen()) {
      std::cerr << "Read a top-level expr:" << std::endl;
      FnIR->print(llvm::errs());
      std::cerr << std::endl;

      FnIR->eraseFromParent();
    }
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

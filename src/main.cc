#include "interpret.h"
#include "parser.h"
#include "utils.h"

int main() {
	// Install standard binary operators.
  // 1 is lowest precedence.
  BinopPrecedence['<'] = 10;
  BinopPrecedence['+'] = 20;
  BinopPrecedence['-'] = 20;
  BinopPrecedence['*'] = 40; // highest.

  MainLoop();
}

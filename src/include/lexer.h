#pragma once
#include "utils.h"

enum Token {
  // Misc.
  tok_eof = 256,
  tok_eol, // end of line

  // primary
  tok_word,
  tok_bool,
  tok_int,
  tok_float,

  // Commands
  tok_def, // function definition
  tok_return, 
};

extern std::string LastWord;
extern bool BoolVal;
extern int IntVal;
extern double FloatVal;

int gettok();

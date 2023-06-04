#pragma once
#include "utils.h"

enum Token {
  // Misc.
  tok_eof = 256,
  tok_eol, // end of line

  // primary
  tok_word,
  tok_number,

  tok_int,
  tok_bool,

  // Commands
  tok_def, // function definition
};

extern std::string LastWord;
extern int NumVal;

int gettok();

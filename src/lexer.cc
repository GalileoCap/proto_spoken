#include "lexer.h"
#include <cstdio>

std::string LastWord;
bool BoolVal;
int IntVal;
double FloatVal;

static int LastChar = ' ';

static int getChar() {
  return LastChar = std::getchar();
}

static enum Token matchToken(const std::string &str) {
  if (str == "def")
    return tok_def;
  else if (str == "true" || str == "false") {
    BoolVal = str == "true";
    return tok_bool;
  } else if (str == "return")
    return tok_return;
  else return tok_word;
}

static enum Token getWord() {
  // Matches [a-zA-Z][a-zA-Z0-9]*
  LastWord = LastChar;
  while (std::isalnum(getChar()))
    LastWord += LastChar;

  return matchToken(LastWord);
}

static enum Token getNumber() {
  // Matches ([0-9]+|[0-9][0-9.]*)
  std::string NumStr;
  bool isFloat = false;
  do {
    NumStr += LastChar;
    isFloat = LastChar == '.';
    getChar();
  } while (isdigit(LastChar) || LastChar == '.');

  if (isFloat) {
    FloatVal = strtod(NumStr.c_str(), nullptr);
    return tok_float;
  } else {
    IntVal = strtoll(NumStr.c_str(), nullptr, 10);
    return tok_int;
  }
}

static void getComment() {
  // Comment until end of line.
  do
    getChar();
  while (LastChar != EOF && LastChar != '\n' && LastChar != '\r');
}

int gettok() {
  // Skip any whitespace.
  while (std::isspace(LastChar))
    getChar();

  if (std::isalpha(LastChar)) 
    return getWord();

  if (isdigit(LastChar))
    return getNumber();

  if (LastChar == '#') {
    getComment();
    if (LastChar != EOF)
      return gettok();
  }

  if (LastChar == ';') {
    getChar();
    return tok_eol;
  }

  // Check for end of file. Don't eat the EOF.
  if (LastChar == EOF)
    return tok_eof;

  // Otherwise, just return the character as its ascii value.
  int ThisChar = LastChar;
  getChar();
  return ThisChar;
}

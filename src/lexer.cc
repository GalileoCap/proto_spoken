#include "lexer.h"
#include <cstdio>

std::string LastWord;
int NumVal;
static int LastChar = ' ';

static enum Token matchToken(const std::string &str) {
  if (str == "def")
    return tok_def;
  else if (str == "int")
    return tok_int;
  else if (str == "bool")
    return tok_bool;
  else return tok_word;
}

static enum Token getWord() {
  // Matches [a-zA-Z][a-zA-Z0-9]*
  LastWord = LastChar;
  while (std::isalnum((LastChar = getchar())))
    LastWord += LastChar;

  return matchToken(LastWord);
}

static enum Token getNumber() {
  // Matches [0-9]+
  std::string NumStr;
  do {
    NumStr += LastChar;
    LastChar = getchar();
  } while (isdigit(LastChar));

  NumVal = strtod(NumStr.c_str(), nullptr);
  return tok_number;
}

static void getComment() {
  // Comment until end of line.
  do
    LastChar = getchar();
  while (LastChar != EOF && LastChar != '\n' && LastChar != '\r');
}

int gettok() {
  // Skip any whitespace.
  while (std::isspace(LastChar))
    LastChar = getchar();

  if (std::isalpha(LastChar)) 
    return getWord();

  if (isdigit(LastChar))
    return getNumber();

  if (LastChar == '#') {
    getComment();
    if (LastChar != EOF)
      return gettok();
  }

  // Check for end of file. Don't eat the EOF.
  if (LastChar == EOF)
    return tok_eof;

  // Otherwise, just return the character as its ascii value.
  int ThisChar = LastChar;
  LastChar = getchar();
  return ThisChar;
}

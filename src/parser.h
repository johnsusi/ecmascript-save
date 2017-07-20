#ifndef ECMASCRIPT_PARSER_H
#define ECMASCRIPT_PARSER_H

#include "ast.h"
#include "matcher.h"
#include "token.h"
#include "trace.h"
#include "util.h"

#include <iostream>
#include <memory>
#include <sstream>
#include <vector>

class SyntaxError : public std::runtime_error {
public:
  SyntaxError(std::string what) : std::runtime_error(what)
  {
  }
};

class Parser {

public:
  Parser(Lexer);
  Program parse();

  struct SyntacticGrammar;

private:
  Lexer                             lexer;
  std::shared_ptr<SyntacticGrammar> grammar;
};

#endif
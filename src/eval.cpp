#include "eval.h"
#include "ast.h"
#include "lexer.h"
#include "logger.h"
#include "parser.h"

#include <iostream>

void eval(const std::string& source, Visitor& visitor, bool verbose)
{
  return eval(convert_utf8_to_utf16(source), visitor, verbose);
}

void eval(const std::u16string& source, Visitor& visitor, bool verbose)
{
  auto logger = make_standard_logger(std::cout);
  // auto lexer = make_lexer(gsl::make_span(buffer.getBuffer(),
  // buffer.length()));
  auto lexer = make_lexer(source);
  // if (verbose) lexer.verbose();
  auto tokens = lexer.tokens();
  auto parser = make_parser(tokens);
  if (verbose) parser.logger(logger.get());
  auto program = parser.parse();
  program.accept(visitor);
}

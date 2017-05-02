#include "eval.h"
#include "lexer.h"
#include "parser.h"

Program eval(const std::string &source)
{
  return eval(convert_utf8_to_utf16(source));
}

Program eval(const std::u16string &source)
{
  // auto lexer = make_lexer(gsl::make_span(buffer.getBuffer(),
  // buffer.length()));
  auto lexer  = make_lexer(source);
  auto tokens = lexer.tokens();
  auto parser = make_parser(tokens);
  return parser.parse();
}

#include "eval.h"
#include "ast.h"
#include "lexer.h"
#include "logger.h"
#include "parser.h"

#include <chrono>
#include <iostream>

void eval(
    const std::string& source, Visitor& visitor, bool verbose, bool parse,
    bool run)
{
  return eval(convert_utf8_to_utf16(source), visitor, verbose, parse, run);
}

void eval(
    const std::u16string& source, Visitor& visitor, bool verbose, bool parse,
    bool run)
{
  auto logger = make_standard_logger(std::cout);
  // auto lexer = make_lexer(gsl::make_span(buffer.getBuffer(),
  // buffer.length()));
  auto start = std::chrono::steady_clock::now();
  auto lexer = make_lexer(source);
  // if (verbose) lexer.verbose();
  auto tokens = lexer.tokens();
  if (verbose)
    std::cout << "Lexing took "
              << std::chrono::duration<double, std::milli>(
                     std::chrono::steady_clock::now() - start)
                     .count()
              << " ms\n";

  if (parse) {
    auto parser = make_parser(tokens);
    if (verbose)
      parser.logger(logger.get());
    start = std::chrono::steady_clock::now();

    auto program = parser.parse();
    if (verbose)
      std::cout << "Parsing took "
                << std::chrono::duration<double, std::milli>(
                       std::chrono::steady_clock::now() - start)
                       .count()
                << " ms\n";

    if (run) {
      start = std::chrono::steady_clock::now();
      program.accept(visitor);
      if (verbose)
        std::cout << "Evaluating took "
                  << std::chrono::duration<double, std::milli>(
                         std::chrono::steady_clock::now() - start)
                         .count()
                  << " ms\n";
    }
  }
}

#include "eval.h"
#include "ast.h"
#include "lexer.h"
#include "logger.h"
#include "parser.h"

#include <chrono>
#include <iostream>

struct Stopwatch {
  decltype(std::chrono::steady_clock::now()) start;

  Stopwatch()
  {
    reset();
  }

  void reset()
  {
    start = std::chrono::steady_clock::now();
  }

  void print(std::ostream& out) const
  {
    out << std::chrono::duration<double, std::milli>(
               std::chrono::steady_clock::now() - start)
               .count()
        << "ms";
  }
};

inline std::ostream& operator<<(std::ostream& out, const Stopwatch& value)
{
  value.print(out);
  return out;
}

// std::ostream& operator<<(std::ostream& out, const Options& v)
// {
//   out << "Options {\n";

//   auto it = v.begin();
//   if (it != v.end())
//     out << it->first << ": " << it->second;
//   while (++it != v.end())
//     out << ",\n" << it->first << ": " << it->second;
//   out << "\n}\n";
//   return out;
// }

void eval(const Source& source, Visitor& visitor, const Options& opts)
{
  auto verbose = opts.count("verbose");
  auto parse   = true;
  auto run     = true;

  auto watch = Stopwatch{};

  auto lexer = Lexer{source};

  if (verbose)
    std::cout << "Lexing took " << watch << "\n";

  if (parse) {
    auto parser = make_parser(lexer);
    watch.reset();

    auto program = parser.parse();
    if (verbose)
      std::cout << "Parsing took " << watch << "\n";

    if (run) {
      watch.reset();
      program.accept(visitor);
      if (verbose)
        std::cout << "Evaluating took " << watch << "\n";
    }
  }
}

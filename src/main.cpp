#include "ast_visitor.h"
#include "eval.h"
#include "eval_visitor.h"
#include "filesystem.h"
#include "json_visitor.h"
#include "lexer.h"
#include "trace.h"
#include "util.h"
#include "variant.h"

#include <iostream>
#include <string>
#include <unordered_map>
#include <vector>

// #include <boost/program_options.hpp>

using namespace std::literals::string_literals;

constexpr auto usage = R"(
  Usage:

    ECMAScript [options] file1 file2...
    ECMAScript [options] -e script

  Options:

    -h [ --help      ]             Print this message
    -v [ --verbose   ]             Be verbose
    -c [ --compile   ]             Only run lexer and compiler
    -e [ --eval      ] <source>    Execute <source>
    -j [ --json      ]             Print AST as JSON (implies -p)
    -l [ --lex       ]             Only run lexer
)";

auto parse_command_line(int argc, const char** argv)
{
  using result_type = variant<bool, std::string, std::vector<std::string>>;
  std::vector<std::string> input_files;
  std::unordered_map<std::string, result_type> result;
  for (int i = 1; i < argc; ++i) {
    if (argv[i][0] == '-') {
      if (argv[i] == "-h"s || argv[i] == "--help"s)
        result["help"] = true;
      else if (argv[i] == "-v"s || argv[i] == "--verbose"s)
        result["verbose"] = true;
      else if (argv[i] == "-c"s || argv[i] == "--compile"s)
        result["compile"] = true;
      else if (argv[i] == "-e"s || argv[i] == "--eval"s) {
        if (++i == argc)
          throw std::domain_error("Missing parameter for "s + argv[i - 1]);
        result["eval"] = std::string(argv[i]);
      }
      else if (argv[i] == "-j"s || argv[i] == "--json"s)
        result["json"] = true;
      else if (argv[i] == "-l"s || argv[i] == "--lex"s)
        result["lex"] = true;
      else
        throw std::domain_error("Invalid argument: '"s + argv[i] + "'");
    }
    else
      input_files.push_back(argv[i]);
  }

  if (!input_files.empty())
    result["input-file"] = std::move(input_files);

  return result;
}

int main(int argc, const char** argv)
{
  using namespace std;

  try {

    auto options = parse_command_line(argc, argv);

    bool verbose = options.count("verbose");

    auto run = [&](auto source) {

      if (options.count("json")) {
        JSONVisitor visitor;
        eval(source, visitor, verbose);
        std::cout << visitor.str() << std::endl;
      }
      else if (options.count("lex")) {
        BasicVisitor visitor;
        eval(source, visitor, verbose, false, false);
      }
      else if (options.count("compile")) {
        BasicVisitor visitor;
        eval(source, visitor, verbose, true, false);
      }
      else {
        EvalVisitor visitor;
        eval(source, visitor, verbose);
        std::cout << visitor.str() << std::endl;
      }
    };

    if (options.count("help"))
      cout << usage;
    else if (options.count("eval"))
      run(get<string>(options["eval"]));
    else if (options.count("input-file")) {
      auto filenames = get<vector<string>>(options["input-file"]);
      //.as<vector<string>>();
      for (const auto& filename : filenames) {
        if (verbose)
          std::cout << "Evaluating " << filename << std::endl;
        run(read_file(filename));
      }
    }
    else { // Try reading from stdin
      auto source = read_stdin();
      if (source.size() > 0)
        run(source);
      else
        std::cout << usage;
    }
  }
  catch (const std::exception& err) {
    cerr << err.what() << endl;
    for (auto line : stack_trace())
      cerr << line << endl;
    return -1;
  }
  return 0;
}
#include "ast_visitor.h"
#include "eval.h"
#include "eval_visitor.h"
#include "filesystem.h"
#include "json_visitor.h"
#include "trace.h"
#include "util.h"

#include <iostream>
#include <string>
#include <vector>

#include <boost/program_options.hpp>

using namespace std::literals::string_literals;

constexpr auto usage = R"(
  Usage:

    ECMAScript [options] file1 file2...
    ECMAScript [options] -e script

  Options:

    -h [ --help      ]             Print this message
    -v [ --verbose   ]             Be verbose
    -e [ --eval      ] <source>    Execute <source>
    -p [ --print-ast ]             Pretty print the resulting AST
    -l [ --lex       ]             Only perform lexing on the input
    -j [ --json      ]             Print AST as JSON (implies -p)
)";

auto parse_command_line(int argc, const char** argv)
{
  using namespace std;
  using namespace boost::program_options;

  options_description desc("Options");

  desc.add_options()("help,h", "print usage message");
  desc.add_options()("verbose,v", "be verbose");
  desc.add_options()("eval,e", value<string>(), "string");
  desc.add_options()("ast,p", "print AST to stdout");
  desc.add_options()("lex,l", "Perform lexing only");
  desc.add_options()("json,j", "Prist AST as JSON (implies -p)");

  options_description hidden("Hidden options");
  hidden.add_options()("input-file", value<vector<string>>(), "input file");

  options_description cmdline_options;
  cmdline_options.add(desc).add(hidden);

  positional_options_description p;
  p.add("input-file", -1);

  variables_map vm;
  store(
      command_line_parser(argc, argv)
          .options(cmdline_options)
          .positional(p)
          .run(),
      vm);
  notify(vm);
  return vm;
}

int main(int argc, const char** argv)
{
  using namespace std;

  try {

    auto vm = parse_command_line(argc, argv);

    bool verbose = vm.count("verbose");
    bool ast     = vm.count("json");

    auto run = [&](auto source) {

      if (vm.count("json")) {
        JSONVisitor visitor;
        eval(source, visitor, verbose);
        std::cout << visitor.str() << std::endl;
      }
      else if (vm.count("ast")) {
        SimplifiedYAMLVisitor visitor;
        eval(source, visitor, verbose);
        std::cout << visitor.str() << std::endl;
      }
      else {
        EvalVisitor visitor;
        eval(source, visitor, verbose);
        std::cout << visitor.str() << std::endl;
      }
    };

    if (vm.count("help")) {
      std::cout << usage;
    }
    else if (vm.count("eval")) {
      run(vm["eval"].as<string>());
    }
    else if (vm.count("input-file")) {
      auto filenames = vm["input-file"].as<vector<string>>();
      for (const auto& filename : filenames) {
        if (verbose) std::cout << "Evaluating " << filename << std::endl;
        run(read_file(filename));
      }
    }
    else { // Try reading from stdin
      auto source = read_stdin();
      if (source.size() > 0) {
        run(source);
      }
      else {
        std::cout << usage;
      }
    }
  }
  catch (const std::exception& err) {
    cerr << err.what() << endl;
    for (auto line : stack_trace()) {
      cerr << line << endl;
    }
    return -1;
  }
  return 0;
}
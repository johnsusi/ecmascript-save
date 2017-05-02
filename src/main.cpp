#include "eval.h"
#include "lexer.h"
#include "parser.h"
#include "util.h"

#include <algorithm>
#include <iostream>
#include <iterator>
#include <string>
#include <vector>

#include <boost/filesystem.hpp>
#include <boost/program_options.hpp>

using namespace std::literals::string_literals;

namespace fs = boost::filesystem;
namespace po = boost::program_options;

template <class T>
std::ostream &operator<<(std::ostream &os, const std::vector<T> &v)
{
  std::copy(v.begin(), v.end(), std::ostream_iterator<T>(os, " "));
  return os;
}

int main(int argc, const char **argv)
{
  using namespace std;

  try {

    const auto usage = left_align_text(
        R"(
        Usage:

          ECMAScript [options] file1 file2...
          ECMAScript [options] -e script

        Options:

          -h [ --help      ]             Print this message
          -v [ --verbose   ]             Be verbose
          -e [ --eval      ] <source>    Execute <source>
          -p [ --print-ast ]             Pretty print the resulting AST
          -l [ --lex       ]             Only perform lexing on the input
      )");

    po::options_description desc("Options");

    desc.add_options()("help,h", "print usage message");
    desc.add_options()("verbose,v", "be verbose");
    desc.add_options()("eval,e", po::value<string>(), "string");
    desc.add_options()("ast,p", "print AST to stdout");
    desc.add_options()("lex,l", "Perform lexing only");

    po::options_description hidden("Hidden options");
    hidden.add_options()("input-file", po::value<vector<string>>(),
                         "input file");

    po::options_description cmdline_options;
    cmdline_options.add(desc).add(hidden);

    po::positional_options_description p;
    p.add("input-file", -1);

    po::variables_map vm;
    po::store(po::command_line_parser(argc, argv)
                  .options(cmdline_options)
                  .positional(p)
                  .run(),
              vm);
    po::notify(vm);
    bool verbose = vm.count("verbose");

    auto run = [&](auto source) {
      auto program = eval(source);
      if (vm.count("ast")) std::cout << to_string(program);
    };

    if (vm.count("help")) {
      std::cout << usage;
    }
    else if (vm.count("eval")) {
      run(vm["eval"].as<string>());
    }
    else if (vm.count("input-file")) {
      auto filenames = vm["input-file"].as<vector<string>>();
      for (const auto &filename : filenames) {
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
  catch (const std::exception &err) {
    cerr << err.what() << endl;
    return -1;
  }
  return 0;
}
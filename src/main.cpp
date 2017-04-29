#include "lexer.h"
#include "parser.h"
#include "eval.h"

#include <algorithm>
#include <iostream>
#include <iterator>
#include <string>
#include <vector>

#include <gsl/gsl>

#include <boost/filesystem.hpp>
#include <boost/program_options.hpp>

using namespace std::literals::string_literals;

namespace fs = boost::filesystem;
namespace po = boost::program_options;

template<class T>
std::ostream& operator<<(std::ostream& os, const std::vector<T>& v)
{
    std::copy(v.begin(), v.end(), std::ostream_iterator<T>(os, " "));
    return os;
}

std::string read_file_into_string(const fs::path& filename)
{
    std::ifstream ifs(filename.c_str());
    if (ifs.fail()) throw std::runtime_error("Error reading '"s + filename.string() + "'");
    return std::string((std::istreambuf_iterator<char>(ifs)),
                       (std::istreambuf_iterator<char>()));
}

std::string read_stdin_into_string()
{
  return std::string((std::istreambuf_iterator<char>(std::cin)),
                     (std::istreambuf_iterator<char>()));
}

int main(int argc, const char** argv)
{
  using namespace std;

  try {

    po::options_description desc("Allowed options");
    desc.add_options()
      ("help,h", "print usage message")
      ("verbose,v", "be verbose")
      ("eval,e",po::value<string>(), "string")
      ("ast,p", "print AST to stdout")
      ("lex,l", "Perform lexing only")
    ;

    po::options_description hidden("Hidden options");
    hidden.add_options()
      ("input-file", po::value< vector<string> >(), "input file")
    ;

    po::options_description cmdline_options;
    cmdline_options.add(desc).add(hidden);

    po::positional_options_description p;
    p.add("input-file", -1);

    po::variables_map vm;
    po::store(po::command_line_parser(argc, argv).
      options(cmdline_options).positional(p).run(), vm);
    po::notify(vm);
    bool verbose = vm.count("verbose");
    if (vm.count("help")) {
      std::cout << desc << "\n";
    }
    else if (vm.count("eval")) {
      auto source = vm["eval"].as<string>();
      auto program = eval(source);
      if (vm.count("ast")) std::cout << to_string(program);
    }
    else if (vm.count("input-file")) {
      auto filenames = vm["input-file"].as< vector<string> >();
      for (const auto& filename : filenames) {
        if (verbose) std::cout << "Evaluating " << filename << std::endl;
        auto source = read_file_into_string(filename);
        auto program = eval(source);
        if (vm.count("ast")) std::cout << to_string(program);
      }
    }
    else { // Try reading from stdin
      auto source = read_stdin_into_string();
      if (source.size() > 0) {
        auto program = eval(source);
        if (vm.count("ast")) std::cout << to_string(program);
      }
      else {
        std::cout << desc << "\n";
      }
    }
  }
  catch(const std::exception& err) {
    cerr << err.what() << endl;
    return -1;
  }
  return 0;
}
#include "lexer.h"
#include "parser.h"

#include <algorithm>
#include <iostream>
#include <iterator>
#include <string>
#include <vector>

#include <gsl/gsl>

#include <boost/filesystem.hpp>
#include <boost/program_options.hpp>

#include <unicode/errorcode.h>
#include <unicode/normalizer2.h>
#include <unicode/unistr.h>

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

// UnicodeString icu::UnicodeString::fromUTF8(StringPiece utf8)
// {
//   int32_t destLength;
//   UErrorCode err = U_ZERO_ERROR;
//   if (U_FAILURE(err)) throw std::runtime_error(u_errorName(err));
//   u_strFromUTF8(nullptr, 0, &destLength, utf8.data(), utf8.size(), &err);
//   auto buffer = std::make_unique<UChar*>(new UChar[destLength]);
//   err = U_ZERO_ERROR;
//   u_strFromUTF8(*buffer, destLength, nullptr, utf8.data(), utf8.size(), &err);
//   if (U_FAILURE(err)) throw std::runtime_error(u_errorName(err));
//   UnicodeString result(*buffer, destLength);
//   return result;
// }

UnicodeString convert_string_to_utf16(const std::string& source)
{
  ErrorCode err;
  auto buffer = UnicodeString::fromUTF8(source);
  auto normalizer = Normalizer2::getNFCInstance(err);
  if (err.isFailure()) throw std::runtime_error(err.errorName());
  auto result = normalizer->normalize(buffer, err);
  if (err.isFailure()) throw std::runtime_error(err.errorName());
  return result;
}

void eval(const std::string& source, bool dump_ast = false)
{
  auto buffer = convert_string_to_utf16(source);
  auto lexer = make_lexer(gsl::make_span(buffer.getBuffer(), buffer.length()));
  // auto lexer = make_lexer(source);
  std::cout << lexer.tokens().size() << std::endl;
  auto tokens = lexer.tokens();
  auto parser = make_parser(tokens);
  auto program = parser.parse();
  if (dump_ast) {
    std::cout << to_string(program);
  }

}

int main(int argc, const char** argv)
{
  using namespace std;

  try {

    po::options_description desc("Allowed options");
      desc.add_options()
        ("help,h", "print usage message")
        ("eval,e",po::value<string>(), "string")
        ("ast,p", "print AST to stdout")
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

    if (vm.count("help")) {
      std::cout << desc << "\n";
    }
    else if (vm.count("eval")) {
      auto source = vm["eval"].as<string>();
      eval(source, vm.count("ast"));
    }
    else if (vm.count("input-file")) {
      for (const auto& filename : vm["input-file"].as< vector<string> >()) {
        auto source = read_file_into_string(filename);
        eval(source, vm.count("ast"));
      }
    }
    else {
      std::cout << desc << "\n";
    }

  }
  catch(const std::exception& err) {
    cerr << err.what() << endl;
    return -1;
  }
  return 0;
}
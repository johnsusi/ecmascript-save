#include "util.h"

#include <algorithm>
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>

#include <boost/algorithm/string.hpp>
#include <boost/core/demangle.hpp>

#include <unicode/uchar.h>

/*!

  Left aligns \a text by first finding the left-most non-whitespace
  character and then removing that many leading white-space characters from
  each line. The result is then indented using the optional \a indent and
  joined into a string.

*/
std::string left_align_text(const std::string &text, const std::string &indent)
{
  using namespace std;
  using namespace boost;
  vector<string> rows;
  split(rows, text, is_any_of("\n"), token_compress_off);

  auto it         = rows.begin();
  int  leading_ws = 0;
  for (; it != rows.end(); ++it) {
    auto n = it->find_first_not_of(' ');
    if (n != it->npos) {
      leading_ws = n;
      break;
    }
  }

  stringstream result;

  for (; it != rows.end(); ++it) {
    auto n = it->find_first_not_of(' ');
    if (n == it->npos) {
      result << "\n";
      continue;
    }
    result << indent << it->substr(leading_ws) << "\n";
  }
  return result.str();
}

/*!

  Returns the general category (short) name for the code point. If the category
  is not known '??' is returned. Always returns a zero terminated char array of
  length 3.

*/
const char *u_charCategory(int c)
{
  switch (u_charType(c)) {
  case U_GENERAL_OTHER_TYPES: return "Cn";
  case U_UPPERCASE_LETTER: return "Lu";
  case U_LOWERCASE_LETTER: return "Ll";
  case U_TITLECASE_LETTER: return "Lt";
  case U_MODIFIER_LETTER: return "Lm";
  case U_OTHER_LETTER: return "Lo";
  case U_NON_SPACING_MARK: return "Mn";
  case U_ENCLOSING_MARK: return "Me";
  case U_COMBINING_SPACING_MARK: return "Mc";
  case U_DECIMAL_DIGIT_NUMBER: return "Nd";
  case U_LETTER_NUMBER: return "Nl";
  case U_OTHER_NUMBER: return "No";
  case U_SPACE_SEPARATOR: return "Zs";
  case U_LINE_SEPARATOR: return "Zl";
  case U_PARAGRAPH_SEPARATOR: return "Zp";
  case U_CONTROL_CHAR: return "Cc";
  case U_FORMAT_CHAR: return "Cf";
  case U_PRIVATE_USE_CHAR: return "Co";
  case U_SURROGATE: return "Cs";
  case U_DASH_PUNCTUATION: return "Pd";
  case U_START_PUNCTUATION: return "Ps";
  case U_END_PUNCTUATION: return "Pe";
  case U_CONNECTOR_PUNCTUATION: return "Pc";
  case U_OTHER_PUNCTUATION: return "Po";
  case U_MATH_SYMBOL: return "Sm";
  case U_CURRENCY_SYMBOL: return "Sc";
  case U_MODIFIER_SYMBOL: return "Sk";
  case U_OTHER_SYMBOL: return "So";
  case U_INITIAL_PUNCTUATION: return "Pi";
  default: return "??";
  }
}

/*

  Returns a string with the contents of \a filename.

  Throws std::runtime_error on failure.

*/
std::string read_file(const std::string &filename)
{
  std::ifstream ifs(filename);
  if (ifs.fail()) throw std::runtime_error("Error reading '" + filename + "'");
  using it = std::istreambuf_iterator<char>;
  return std::string(it(ifs), it());
}

/*

  Returns a string with the contents of \a stdin.

  Note: Might wait for input in some cases. For instance, when not piping a file
        to the program.

*/
std::string read_stdin()
{
  using it = std::istreambuf_iterator<char>;
  return std::string(it(std::cin), it());
}

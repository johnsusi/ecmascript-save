#include "util.h"

#include <algorithm>
#include <fstream>
#include <iostream>
#include <numeric>
#include <sstream>
#include <string>
#include <vector>

#include <boost/algorithm/string.hpp>
#include <boost/core/demangle.hpp>

#include <unicode/errorcode.h>
#include <unicode/normalizer2.h>
#include <unicode/uchar.h>
#include <unicode/unistr.h>

/*!

  Converts \a source from UTF-8 to UTF-16 and normalised to Unicode
  Normalization Form C.

  Throws \a std::runtime_error on failure.

*/
std::u16string convert_utf8_to_utf16(const std::string& source)
{
  ErrorCode err;
  auto      buffer     = UnicodeString::fromUTF8(source);
  auto      normalizer = Normalizer2::getNFCInstance(err);
  if (err.isFailure()) throw std::runtime_error(err.errorName());
  auto result = normalizer->normalize(buffer, err);
  if (err.isFailure()) throw std::runtime_error(err.errorName());
  return std::u16string(
      reinterpret_cast<const std::u16string::value_type*>(buffer.getBuffer()),
      buffer.length());
}

/*!

  Converts \a source from UTF-16 to UTF-8.

*/
std::string convert_utf16_to_utf8(const std::u16string& source)
{

  std::string result;

  // NOTE: ICU59 will use char16_t* instead of UChar
  UnicodeString(reinterpret_cast<const UChar*>(source.data()), source.size())
      .toUTF8String(result);

  return result;
}

#ifdef U_DISABLE_RENAMING
UnicodeString icu::UnicodeString::fromUTF8(StringPiece utf8)
{
  int32_t    destLength;
  UErrorCode err = U_ZERO_ERROR;
  if (U_FAILURE(err)) throw std::runtime_error(u_errorName(err));
  u_strFromUTF8(nullptr, 0, &destLength, utf8.data(), utf8.size(), &err);
  auto buffer = std::make_unique<UChar*>(new UChar[destLength]);
  err         = U_ZERO_ERROR;
  u_strFromUTF8(*buffer, destLength, nullptr, utf8.data(), utf8.size(), &err);
  if (U_FAILURE(err)) throw std::runtime_error(u_errorName(err));
  UnicodeString result(*buffer, destLength);
  return result;
}
#endif

/*!

  Left aligns \a text by first finding the left-most non-whitespace
  character and then removing that many leading white-space characters from
  each line. The result is then indented using the optional \a indent and
  joined into a string.

*/
std::string left_align_text(const std::string& text, const std::string& indent)
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
const char* u_charCategory(int c)
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
std::string read_file(const std::string& filename)
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

static char to_hex_digit(int x)
{
  switch (x) {
  case 0: return '0';
  case 1: return '1';
  case 2: return '2';
  case 3: return '3';
  case 4: return '4';
  case 5: return '5';
  case 6: return '6';
  case 7: return '7';
  case 8: return '8';
  case 9: return '9';
  case 10: return 'a';
  case 11: return 'b';
  case 12: return 'c';
  case 13: return 'd';
  case 14: return 'e';
  case 15: return 'f';
  default: return '?';
  }
}

static std::string& encode(std::string& str, int cp)
{
  switch (cp) {
  case '\"': str += "\\\""; break;
  case '\\': str += "\\\\"; break;
  case '/': str += "\\/"; break;
  case '\b': str += "\\b"; break;
  case '\f': str += "\\f"; break;
  case '\n': str += "\\n"; break;
  case '\r': str += "\\r"; break;
  case '\t': str += "\\t"; break;
  default:
    if (cp >= 0 && cp <= 0x0F) {
      str += "\\u000";
      str += to_hex_digit(cp & 0x0F);
    }
    else if (cp >= 0x10 && cp <= 0x1F) {
      str += "\\u00";
      str += to_hex_digit((cp / 16) & 0x0F);
      str += to_hex_digit(cp & 0x0F);
    }
    else
      str += cp;
    break;
  }
  return str;
}

/*

  Returns a json encoded string.

*/
std::string stringify(const std::u16string& str)
{
  return accumulate(str.begin(), str.end(), std::string("\""), encode) + "\"";
}

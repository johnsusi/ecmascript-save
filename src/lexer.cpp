#include "lexer.h"
#include "util.h"

#include <unicode/uchar.h>

// 7.2
bool is_white_space(int cp) {
  switch (cp) {
  case 0x0009: // Tab             <TAB>
  case 0x000B: // Vertical tab    <VT>
  case 0x000C: // Form feed       <FF>
  case 0x0020: // Space           <SP>
  case 0x00A0: // No-break space  <NBSP>
  case 0xFEFF: // byte order mark <BOM>
    return true;
  default:
    switch (u_charType(cp)) {
    case U_SPACE_SEPARATOR: // Any other Unicode "space separator" <USP>
      return true;
    default:
      return false;
    }
  }
}

// 7.3
// bool is_line_terminator(int cp)
// {
//   switch (cp) {
//   case 0x000A: // Line Feed <LF>
//   case 0x000D: // Carriage Return <CR>
//   case 0x2028: // Line separator <LS>
//   case 0x2029: // Paragraph separator <PS>
//     return true;
//   default: return false;
//   }
// }

// 7.8.3
bool is_non_zero_digit(int cp) { return cp >= '1' && cp <= '9'; }

bool is_decimal_digit(int cp) { return cp == '0' || is_non_zero_digit(cp); }

bool is_hex_digit(int cp) {
  return is_decimal_digit(cp) || (cp >= 'a' && cp <= 'f') ||
         (cp >= 'A' && cp <= 'F');
}

bool is_exponent_indicator(int cp) {
  switch (cp) {
  case 'e':
    return true;
  case 'E':
    return true;
  default:
    return false;
  }
}

// 7.6
bool is_unicode_letter(int cp) {
  switch (u_charType(cp)) {
  case U_UPPERCASE_LETTER: // Lu
  case U_LOWERCASE_LETTER: // Li
  case U_TITLECASE_LETTER: // Lt
  case U_MODIFIER_LETTER:  // Lm
  case U_OTHER_LETTER:     // Lo
  case U_LETTER_NUMBER:    // Nl
    return true;
  default:
    return false;
  }
}

bool is_unicode_combining_mark(int cp) {
  switch (u_charType(cp)) {
  case U_NON_SPACING_MARK:       // Mn
  case U_COMBINING_SPACING_MARK: // Mc
    return true;
  default:
    return false;
  }
}

bool is_unicode_digit(int cp) {
  switch (u_charType(cp)) {
  case U_DECIMAL_DIGIT_NUMBER:
    return true;
  default:
    return false;
  }
}

bool is_unicode_connector_punctuation(int cp) {
  switch (u_charType(cp)) {
  case U_CONNECTOR_PUNCTUATION:
    return true;
  default:
    return false;
  }
}

bool is_single_escape_character(int cp) {
  switch (cp) {
  case '\'':
  case '"':
  case '\\':
  case 'b':
  case 'f':
  case 'n':
  case 'r':
  case 't':
  case 'v':
    return true;
  default:
    return false;
  }
}

// 7.6.1.1
bool is_keyword(const std::string &token) {
  std::vector<std::string> keywords{
      "break",  "case",  "catch",      "continue", "debugger", "default",
      "delete", "do",    "else",       "finally",  "for",      "function",
      "if",     "in",    "instanceof", "new",      "return",   "switch",
      "this",   "throw", "try",        "typeof",   "var",      "void",
      "while",  "with"};
  return std::binary_search(keywords.begin(), keywords.end(), token);
}

// 7.6.1.2
bool is_future_reserved_word(const std::string &token) {
  static std::vector<std::string> keywords = {
      "class",  "const",     "enum",  "export",  "extends", "implements",
      "import", "interface", "let",   "package", "private", "protected",
      "public", "static",    "super", "yield"};
  return std::binary_search(keywords.begin(), keywords.end(), token);
}

// 7.7
bool is_punctuator(const std::string &token) {
  static std::vector<std::string> punctuators = {
      "!",  "!=", "!==", "%",   "%=",   "&",  "&&", "&=", "(",   ")",
      "*",  "*=", "+",   "++",  "+=",   ",",  "-",  "--", "-=",  ".",
      ":",  ";",  "<",   "<<",  "<<=",  "<=", "=",  "==", "===", ">",
      ">=", ">>", ">>=", ">>>", ">>>=", "?",  "[",  "]",  "^",   "^=",
      "{",  "|",  "|=",  "||",  "}",    "~"};
  return std::binary_search(punctuators.begin(), punctuators.end(), token);
}

bool is_div_punctuator(const std::string &token) {
  return token == "/" || token == "/=";
}

// 7.8.1
bool is_null_literal(const std::string &str) {
  return std::string{"null"} == str;
}

// 7.8.2
bool is_boolean_literal(const std::string &str) {
  return std::string{"true"} == str || std::string{"false"} == str;
}

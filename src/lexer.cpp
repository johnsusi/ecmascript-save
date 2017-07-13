#include "lexer.h"
#include "lexical_grammar.h"
#include "trace.h"
#include "util.h"

#include <algorithm>
#include <iostream>

#include <unicode/uchar.h>

using namespace std;

struct DebugInfoImpl : Lexer::DebugInfo {

  using It = std::u16string::const_iterator;

  It  s, at, to;
  int col, row;

  DebugInfoImpl(It s, It at, It to, int col, int row)
      : s(s), at(at), to(to), col(col), row(row)
  {
  }

  std::string syntax_error_at() const final
  {
    auto it = at;
    while (!is_line_terminator(*it) && it != s) {
      --it;
    }
    std::string line1{it, at};
    std::string line2(line1.size() - 1, '.');
    return line1 + std::string{at, to} + "\n" + line2 + "^\n";
  }

  std::string loc() const final
  {
    return "[" + std::to_string(col) + ":" + std::to_string(row) + "]";
  }
};

static bool reg_exp_allowed(const Token& token)
{
  return token.any_of(
      "return", "new", "delete", "throw", "else", "case", "in", "instanceof",
      "typeof", "new", "void", "delete", "+", "-", "!", "~", "&", "|", "^", "*",
      "/", "%", ">>", "<<", ">>>", "<", ">",
      "<=", ">=", "==", "===", "!=", "!==", "?", "=", "+=", "-=", "/=", "*=",
      "%=", ">>=", "<<=", ">>>=", "|=", "^=", "&=", "&&", "||", "[", "{", "(",
      ",", ";", ":");
}

static void
lex(const Source& source, vector<Token>& tokens, vector<DebugInfoImpl>& debug)
{
  trace();

  LexicalGrammar<char16_t> grammar{source.begin(), source.end()};

  bool lt  = false;
  int  col = 0, row = 0;
  bool re = true;
  auto s  = grammar.match.mark();
  auto m  = grammar.match.mark();
  try {
    while (auto input_element = re ? grammar.input_element_reg_exp()
                                   : grammar.input_element_div()) {

      if (input_element.is_white_space())
        col++;
      else if (input_element.has_line_terminator()) {
        lt  = true;
        col = 0;
        row++;
      }
      else if (auto token = input_element.to_token()) {
        if (lt)
          token->set_preceded_by_line_terminator();
        tokens.push_back(*token);
        debug.emplace_back(s, m + 1, grammar.match.mark(), col, row);

        re = reg_exp_allowed(*token);
        lt = false;
        col += std::distance(m, grammar.match.mark());
      }
      m = grammar.match.mark();
    }
  }
  catch (const exception& err) {
    DebugInfoImpl di{s, m + 1, grammar.match.mark(), col, row};
    throw std::runtime_error(string{err.what()} + "\n" + di.syntax_error_at());
  }
}

struct Lexer::Private {
  Source                source;
  vector<Token>         tokens;
  vector<DebugInfoImpl> debug_infos;
};

Lexer::Lexer(vector<Token> tokens) : pimpl(make_shared<Private>())
{
  pimpl->tokens = move(tokens);
}

Lexer::Lexer(Source source) : pimpl(make_shared<Private>())
{
  pimpl->source = move(source);
  lex(pimpl->source, pimpl->tokens, pimpl->debug_infos);
}

const std::vector<Token>& Lexer::tokens() const
{
  return pimpl->tokens;
}

Lexer::DebugInfo* Lexer::lookup(const vector<Token>::const_iterator& it) const
{
  auto offset = distance(begin(), it);
  if (offset < pimpl->debug_infos.size())
    return &pimpl->debug_infos.at(offset);
  return nullptr;
}

bool operator==(const Lexer& lhs, const Lexer& rhs)
{
  return std::equal(lhs.begin(), lhs.end(), rhs.begin(), rhs.end());
}

bool operator!=(const Lexer& lhs, const Lexer& rhs)
{
  return !(lhs == rhs);
}

std::ostream& operator<<(std::ostream& out, const Lexer& lexer)
{
  out << "[";
  auto it = lexer.begin();
  out << *it++;
  while (it != lexer.end())
    out << ", " << *it++;
  out << "]";
  return out;
}

// 7.2
bool is_white_space(int cp)
{
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
    default: return false;
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
bool is_non_zero_digit(int cp)
{
  return cp >= '1' && cp <= '9';
}

bool is_decimal_digit(int cp)
{
  return cp == '0' || is_non_zero_digit(cp);
}

bool is_hex_digit(int cp)
{
  return is_decimal_digit(cp) || (cp >= 'a' && cp <= 'f')
         || (cp >= 'A' && cp <= 'F');
}

bool is_exponent_indicator(int cp)
{
  switch (cp) {
  case 'e': return true;
  case 'E': return true;
  default: return false;
  }
}

// 7.6
bool is_unicode_letter(int cp)
{
  switch (u_charType(cp)) {
  case U_UPPERCASE_LETTER: // Lu
  case U_LOWERCASE_LETTER: // Li
  case U_TITLECASE_LETTER: // Lt
  case U_MODIFIER_LETTER:  // Lm
  case U_OTHER_LETTER:     // Lo
  case U_LETTER_NUMBER:    // Nl
    return true;
  default: return false;
  }
}

bool is_unicode_combining_mark(int cp)
{
  switch (u_charType(cp)) {
  case U_NON_SPACING_MARK:       // Mn
  case U_COMBINING_SPACING_MARK: // Mc
    return true;
  default: return false;
  }
}

bool is_unicode_digit(int cp)
{
  switch (u_charType(cp)) {
  case U_DECIMAL_DIGIT_NUMBER: return true;
  default: return false;
  }
}

bool is_unicode_connector_punctuation(int cp)
{
  switch (u_charType(cp)) {
  case U_CONNECTOR_PUNCTUATION: return true;
  default: return false;
  }
}

bool is_single_escape_character(int cp)
{
  switch (cp) {
  case '\'':
  case '"':
  case '\\':
  case 'b':
  case 'f':
  case 'n':
  case 'r':
  case 't':
  case 'v': return true;
  default: return false;
  }
}

// 7.6.1.1
bool is_keyword(const std::string& token)
{
  std::vector<std::string> keywords{
      "break",  "case",  "catch",      "continue", "debugger", "default",
      "delete", "do",    "else",       "finally",  "for",      "function",
      "if",     "in",    "instanceof", "new",      "return",   "switch",
      "this",   "throw", "try",        "typeof",   "var",      "void",
      "while",  "with"};
  return std::binary_search(keywords.begin(), keywords.end(), token);
}

// 7.6.1.2
bool is_future_reserved_word(const std::string& token)
{
  static std::vector<std::string> keywords = {
      "class",  "const",     "enum",  "export",  "extends", "implements",
      "import", "interface", "let",   "package", "private", "protected",
      "public", "static",    "super", "yield"};
  return std::binary_search(keywords.begin(), keywords.end(), token);
}

// 7.7
bool is_punctuator(const std::string& token)
{
  static std::vector<std::string> punctuators = {
      "!",  "!=", "!==", "%",   "%=",   "&",  "&&", "&=", "(",   ")",
      "*",  "*=", "+",   "++",  "+=",   ",",  "-",  "--", "-=",  ".",
      ":",  ";",  "<",   "<<",  "<<=",  "<=", "=",  "==", "===", ">",
      ">=", ">>", ">>=", ">>>", ">>>=", "?",  "[",  "]",  "^",   "^=",
      "{",  "|",  "|=",  "||",  "}",    "~"};
  return std::binary_search(punctuators.begin(), punctuators.end(), token);
}

bool is_div_punctuator(const std::string& token)
{
  return token == "/" || token == "/=";
}

// 7.8.1
bool is_null_literal(const std::string& str)
{
  return std::string{"null"} == str;
}

// 7.8.2
bool is_boolean_literal(const std::string& str)
{
  return std::string{"true"} == str || std::string{"false"} == str;
}

#include "lexer.h"
#include "input_element.h"
#include "matcher.h"
#include "token.h"
#include "trace.h"
#include "util.h"

#include <algorithm>
#include <cmath>
#include <iostream>
#include <iterator>
#include <string>
#include <vector>

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

template <typename... Args>
constexpr bool _any_of(const Token& token, Args&&... args)
{

  bool r[sizeof...(Args)] = {
      (token.type() == Token(std::forward<Args>(args)).type())... //
  };

  int sum = 0;
  for (std::size_t i = 0; i < sizeof...(Args); ++i) {
    if (r[i])
      ++sum;
  }
  return sum == 1; // _any_of(token, tokens);
}

static bool reg_exp_allowed(const Token& token)
{
  // switch (token) {
  // case Token("!"): return true;
  // case Token("!="): return true;
  // case Token("!=="): return true;
  // case Token("%"): return true;
  // case Token("%="): return true;
  // case Token("&"): return true;
  // case Token("&&"): return true;
  // case Token("&="): return true;
  // case Token("("): return true;
  // case Token("*"): return true;
  // case Token("*="): return true;
  // case Token("+"): return true;
  // case Token("+="): return true;
  // case Token(","): return true;
  // case Token("-"): return true;
  // case Token("-="): return true;
  // case Token("/"): return true;
  // case Token("/="): return true;
  // case Token(":"): return true;
  // case Token(";"): return true;
  // case Token("<"): return true;
  // case Token("<<"): return true;
  // case Token("<<="): return true;
  // case Token("<="): return true;
  // case Token("="): return true;
  // case Token("=="): return true;
  // case Token("==="): return true;
  // case Token(">"): return true;
  // case Token(">="): return true;
  // case Token(">>"): return true;
  // case Token(">>="): return true;
  // case Token(">>>"): return true;
  // case Token(">>>="): return true;
  // case Token("?"): return true;
  // case Token("["): return true;
  // case Token("^"): return true;
  // case Token("^="): return true;
  // case Token("case"): return true;
  // case Token("delete"): return true;
  // case Token("else"): return true;
  // case Token("in"): return true;
  // case Token("instanceof"): return true;
  // case Token("new"): return true;
  // case Token("return"): return true;
  // case Token("throw"): return true;
  // case Token("typeof"): return true;
  // case Token("void"): return true;
  // case Token("{"): return true;
  // case Token("|"): return true;
  // case Token("|="): return true;
  // case Token("||"): return true;
  // case Token("~"): return true;
  // default: return false;
  // }
  return token.regular_expression_allowed();
  // return _any_of(
  //     token, "return", "new", "delete", "throw", "else", "case", "in",
  //     "instanceof", "typeof", "void", "+", "-", "!", "~", "&", "|", "^", "*",
  //     "/", "%", ">>", "<<", ">>>", "<", ">",
  //     "<=", ">=", "==", "===", "!=", "!==", "?", "=", "+=", "-=", "/=", "*=",
  //     "%=", ">>=", "<<=", ">>>=", "|=", "^=", "&=", "&&", "||", "[", "{",
  //     "(",
  //     ",", ";", ":");
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

// struct Lexer::Private {
//   Source                        source;
//   vector<Token>                 tokens;
//   vector<DebugInfoImpl>         debug_infos;
//   vector<unique_ptr<u16string>> strings;

//   void lex();
// };

struct Lexer::LexicalGrammar {
  Source source;

  vector<Token> tokens;

  Matcher<char16_t, u16string::const_iterator> match;

  vector<DebugInfoImpl>              debug_infos;
  std::vector<unique_ptr<u16string>> strings;

  Token token_value = Token::null_literal();

  LexicalGrammar()
  {
  }

  LexicalGrammar(Source source)
      : source(move(source)), match(this->source.begin(), this->source.end())
  {
  }

  void syntax_error(const std::string& what = {})
  {
    throw std::runtime_error("Syntax Error: " + what);
  }
  // Lexical Grammar

  // 6
  bool source_character()
  {
    return match([](auto cp) { return true; });
  }

  // 7
  InputElement input_element_div()
  {
    trace();
    auto match_start = match.mark();
    if (white_space())
      return InputElement::white_space();
    if (line_terminator())
      return InputElement::line_terminator();
    if (comment())
      return InputElement::comment(std::u16string{match_start, match.mark()});
    if (token())
      return InputElement::token(std::move(token_value));
    if (div_punctuator())
      return InputElement::token(std::string{match_start, match.mark()}.data());
    return InputElement::empty();
  }

  InputElement input_element_reg_exp()
  {
    trace();
    auto match_start = match.mark();
    if (white_space())
      return InputElement::white_space();
    if (line_terminator())
      return InputElement::line_terminator();
    if (comment())
      return InputElement::comment(std::u16string{match_start, match.mark()});
    if (token() || regular_expression_literal())
      return InputElement::token(std::move(token_value));
    return InputElement::empty();
  }

  // 7.2 White Space
  bool white_space()
  {
    trace();
    return match(is_white_space);
  }

  // 7.3 Line Terminators
  bool line_terminator()
  {
    trace();
    return match(is_line_terminator);
  }

  bool line_terminator_sequence()
  {
    trace();
    return match([&](auto cp) {
      switch (cp) {
      case 0x000D: // Carriage Return <CR>
        match(0x000A);
      case 0x000A: // Line Feed <LF>
      case 0x2028: // Line separator <LS>
      case 0x2029: // Paragraph separator <PS>
        cv = ' ';
        return true;
      default: return false;
      }
    });
  }

  // 7.4 Comments
  bool comment()
  {
    trace();
    return multi_line_comment() || single_line_comment();
  }

  bool multi_line_comment()
  {
    trace();
    return match([&] {
      return match('/') && match('*') && multi_line_comment_chars_opt()
             && match('*') && match('/');
    });
  }

  bool multi_line_comment_chars()
  {
    trace();
    if (multi_line_not_asterisk_char()) {
      multi_line_comment_chars();
      return true;
    }

    if (!match.lookahead('/') && match('*')) {
      post_asterisk_comment_chars();
      return true;
    }

    return false;
  }

  bool multi_line_comment_chars_opt()
  {
    trace();
    multi_line_comment_chars();
    return true;
  }

  bool post_asterisk_comment_chars()
  {
    trace();
    if (multi_line_not_forward_slash_or_asterisk_char()) {
      multi_line_comment_chars();
      return true;
    }
    if (!match.lookahead('/') && match('*')) {
      post_asterisk_comment_chars();
      return true;
    }

    return false;
  }

  bool multi_line_not_asterisk_char()
  {
    trace();
    return (!match.peek('*') && source_character());
  }

  bool multi_line_not_forward_slash_or_asterisk_char()
  {
    trace();
    return (!match.peek('/') && !match.peek('*') && source_character());
  }

  bool single_line_comment()
  {
    trace();
    return match([this] {
      return match('/') && match('/') && single_line_comment_chars();
    });
  }

  bool single_line_comment_chars()
  {
    trace();
    while (single_line_comment_char())
      ;
    return true;
  }

  bool single_line_comment_char()
  {
    trace();
    return match([this] { return !line_terminator() && source_character(); });
  }

  // 7.5 Tokens
  bool token()
  {
    trace();
    token_value = Token("null");
    auto i      = match.mark();
    sv          = {};
    if (identifier_name()) { // std::u16string{i, match.mark()}
      strings.push_back(std::make_unique<std::u16string>(sv));
      token_value = Token::identifier_name(strings.back().get()->data());
      return true;
    }
    if (punctuator()) {
      token_value = Token(std::string{i, match.mark()}.data());
      return true;
    }
    if (numeric_literal()) {
      token_value = Token::numeric_literal(mv);
      return true;
    }
    if (string_literal()) {
      strings.push_back(std::make_unique<std::u16string>(sv));
      token_value = Token::string_literal(strings.back().get()->data());
      return true;
    }
    return false;
  }

  bool identifier_name()
  {
    trace();
    if (identifier_start()) {
      while (identifier_part())
        ;
      return true;
    }
    return false;
  }

  bool identifier_start()
  {
    trace();
    if (unicode_letter() || match.any_of('$', '_')) {
      sv += *match;
      return true;
    }
    if (match([this] { return match('\\') && unicode_escape_sequence(); })) {
      sv += cv;
      return true;
    }
    return false;
  }

  bool identifier_part()
  {
    trace();
    if (identifier_start())
      return true;
    if (unicode_combining_mark() || unicode_digit()
        || unicode_connector_punctuation()
        || match.any_of(0x200C, 0x200D)) {
      sv += *match;

      return true;
    }
    return false;
  }

  bool unicode_letter()
  {
    trace();
    return match(is_unicode_letter);
  }

  bool unicode_combining_mark()
  {
    trace();
    return match(is_unicode_combining_mark);
  }

  bool unicode_digit()
  {
    trace();
    return match(is_unicode_digit);
  }

  bool unicode_connector_punctuation()
  {
    trace();
    return match(is_unicode_connector_punctuation);
  }

  bool reserved_word()
  {
    trace();
    return keyword() || future_reserved_word() || null_literal()
           || boolean_literal();
  }

  bool keyword()
  {
    trace();
    return match.any_of(
        "break", "case", "catch", "continue", "debugger", "default", "delete",
        "do", "else", "finally", "for", "function", "if", "in", "instanceof",
        "new", "return", "switch", "this", "throw", "try", "typeof", "var",
        "void", "while", "with");
  }

  bool future_reserved_word()
  {
    trace();
    return match.any_of(
        "class", "enum", "extends", "super", "const", "export", "import",
        "implements", "let", "private", "public", "interface", "package",
        "protected", "static");
  }

  // 7.7
  bool punctuator()
  {
    trace();
    if (match.peek('.') && match.lookahead(is_decimal_digit))
      return false;
    return match.any_of(
        // Note: Must match longer strings first
        ">>>=", "<<=", ">>=", ">>>", "!==", "===", "!=", "%=", "&&",
        "&=", "*=", "*", "++", "+=", "--", "-=", "<<", "<=", "==", ">=", ">>",
        "^=", "|=", "||", "!", "%", "&", "?", "+", ",", "-", ".", ":", ";", "=",
        ">", "<", "(", ")", "[", "]", "^", "{", "|", "}", "~");
  }

  bool div_punctuator()
  {
    trace();
    return match.any_of("/=", "/");
  }

  // 7.8
  bool literal()
  {
    trace();
    return null_literal() || boolean_literal() || numeric_literal()
           || string_literal() || regular_expression_literal();
  }

  // 7.8.1
  bool null_literal()
  {
    trace();
    if (match("null")) {
      token_value = Token::null_literal();
      return true;
    }
    return false;
  }

  // 7.8.2
  bool boolean_literal()
  {
    trace();
    if (match("true")) {
      token_value = Token::boolean_literal(true);
      return true;
    }
    else if (match("false")) {
      token_value = Token::boolean_literal(false);
      return true;
    }
    else
      return false;
  }

  // 7.8.3
  double mv;

  bool numeric_literal()
  {
    trace();
    if (!hex_integer_literal() && !decimal_literal())
      return false;
    if (identifier_start() || decimal_digit())
      syntax_error();
    return true;
  }

  bool decimal_literal()
  {
    trace();
    auto s0 = match.mark();
    if (match('.')) {
      if (!decimal_digits())
        syntax_error();
    }
    else if (decimal_integer_literal()) {
      if (match('.')) {
        if (decimal_digits()) {
        }
      }
    }
    else
      return false;

    // auto s1 = match.mark();
    if (exponent_part())
      ;
    auto        s2 = match.mark();
    std::string s{s0, s2};
    // auto i = s.begin();
    // auto j = i + std::distance(s0, s1);

    // while (i != j && !is_non_zero_digit(*i))
    //   ++i;
    // while (i != j - 1 && !is_non_zero_digit(*(j - 1)))
    //   --j;

    // int count = 0;
    // for (auto ii = i; ii != j; ++ii) {
    //   if (is_decimal_digit(*ii)) {
    //     ++count;
    //     if (count > 20)
    //       *ii = '0';
    //   }
    // }
    mv = strtod(s.data(), nullptr);
    // int exp;
    // double m = std::frexp(mv, &exp);

    // if (m < (2 << 52))
    //   mv = m * std::pow(2, 1074 + exp) * std::pow(2, -1074);
    return true;
  }

  bool decimal_integer_literal()
  {
    trace();
    if (match('0')) {
      mv = 0;
      return true;
    }
    return decimal_digits();
  }

  bool decimal_digits()
  {
    trace();
    if (!decimal_digit())
      return false;
    auto s = mv;
    while (decimal_digit()) {
      s = s * 10 + mv;
    }
    mv = s;
    return true;
  }

  bool decimal_digit()
  {
    trace();
    return match([this](auto cp) {
      switch (cp) {
      case '0': mv = 0; return true;
      case '1': mv = 1; return true;
      case '2': mv = 2; return true;
      case '3': mv = 3; return true;
      case '4': mv = 4; return true;
      case '5': mv = 5; return true;
      case '6': mv = 6; return true;
      case '7': mv = 7; return true;
      case '8': mv = 8; return true;
      case '9': mv = 9; return true;
      default: return false;
      }
    });
  }

  bool exponent_part()
  {
    trace();
    if (!exponent_indicator())
      return false;
    if (!signed_integer())
      syntax_error();
    return true;
  }

  bool exponent_indicator()
  {
    trace();
    return match([](auto cp) { return cp == 'e' || cp == 'E'; });
  }

  bool signed_integer()
  {
    trace();

    if (match("+")) {
      if (!decimal_digits())
        syntax_error();
      return true;
    }
    if (match("-")) {
      if (!decimal_digits())
        syntax_error();
      mv = -mv;
      return true;
    }
    return decimal_digits();
  }

  bool hex_integer_literal()
  {
    trace();
    if (!match("0x") && !match("0X"))
      return false;
    if (!hex_digits())
      syntax_error();
    return true;
  }

  bool hex_digits()
  {
    trace();
    if (!hex_digit())
      return false;
    long s = mv;
    while (hex_digit()) {
      s = 16 * s + mv;
    }
    mv = s;
    return true;
  }

  bool hex_digit()
  {
    trace();
    return match([this](auto cp) {
      switch (cp) {
      case '0': mv = 0; return true;
      case '1': mv = 1; return true;
      case '2': mv = 2; return true;
      case '3': mv = 3; return true;
      case '4': mv = 4; return true;
      case '5': mv = 5; return true;
      case '6': mv = 6; return true;
      case '7': mv = 7; return true;
      case '8': mv = 8; return true;
      case '9': mv = 9; return true;
      case 'a': mv = 10; return true;
      case 'b': mv = 11; return true;
      case 'c': mv = 12; return true;
      case 'd': mv = 13; return true;
      case 'e': mv = 14; return true;
      case 'f': mv = 15; return true;
      case 'A': mv = 10; return true;
      case 'B': mv = 11; return true;
      case 'C': mv = 12; return true;
      case 'D': mv = 13; return true;
      case 'E': mv = 14; return true;
      case 'F': mv = 15; return true;
      default: return false;
      }
    });
  }

  // 7.8.4

  std::u16string sv;
  char16_t       cv = 0;

  bool string_literal()
  {
    trace();
    if (match('"')) {
      sv = {};
      double_string_characters();
      if (!match('"'))
        syntax_error("Expected \"");
      return true;
    }
    else if (match('\'')) {
      sv = {};
      single_string_characters();
      if (!match('\''))
        syntax_error("Expected '");
      return true;
    }
    else
      return false;
  }

  void double_string_characters()
  {
    trace();
    while (double_string_character())
      sv += cv;
  }

  void single_string_characters()
  {
    trace();
    while (single_string_character())
      sv += cv;
  }

  bool double_string_character()
  {
    trace();
    return match([this](auto cp) {
      switch (cp) {
      case '"': return false;
      case '\\':
        if (this->line_terminator_sequence())
          return this->double_string_character();
        return this->escape_sequence();
      default:
        if (is_line_terminator(cp))
          return false;
        cv = *match;
        return true;
      }
    });
  }

  bool single_string_character()
  {
    trace();
    return match([&](auto cp) {
      switch (cp) {
      case '\'': return false;
      case '\\':
        if (this->line_terminator_sequence())
          return this->single_string_character();
        return this->escape_sequence();
      default:
        if (is_line_terminator(cp))
          return false;
        cv = *match;
        return true;
      }
    });
  }

  bool escape_sequence()
  {
    trace();
    if (!match.lookahead(is_decimal_digit) && match('0')) {
      // The CV of EscapeSequence :: 0 [lookahead ∉ DecimalDigit] is a <NUL>
      // character (Unicode value 0000).
      cv = u'\u0000';
      return true;
    }
    return character_escape_sequence() || hex_escape_sequence()
           || unicode_escape_sequence();
  }

  bool character_escape_sequence()
  {
    trace();
    return single_escape_character() || non_escape_character();
  }

  static bool is_single_escape_character(int cp)
  {
    trace();
    switch (cp) {
    case 'b': return true;
    case 't': return true;
    case 'n': return true;
    case 'v': return true;
    case 'f': return true;
    case 'r': return true;
    case '"': return true;
    case '\'': return true;
    case '\\': return true;
    default: return false;
    }
  }

  bool single_escape_character()
  {
    trace();
    return match([this](auto cp) {
      switch (cp) {
      case 'b': cv  = u'\u0008'; return true;
      case 't': cv  = u'\u0009'; return true;
      case 'n': cv  = u'\u000A'; return true;
      case 'v': cv  = u'\u000B'; return true;
      case 'f': cv  = u'\u000C'; return true;
      case 'r': cv  = u'\u000D'; return true;
      case '"': cv  = u'\u0022'; return true;
      case '\'': cv = u'\u0027'; return true;
      case '\\': cv = u'\u005C'; return true;
      default: return false;
      }
    });
  }

  static bool is_escape_character(int cp)
  {
    trace();
    switch (cp) {
    case 'b': return true;
    case 't': return true;
    case 'n': return true;
    case 'v': return true;
    case 'f': return true;
    case 'r': return true;
    case '"': return true;
    case '\'': return true;
    case '\\': return true;
    case '0': return true;
    case '1': return true;
    case '2': return true;
    case '3': return true;
    case '4': return true;
    case '5': return true;
    case '6': return true;
    case '7': return true;
    case '8': return true;
    case '9': return true;
    case 'x': return true;
    case 'u': return true;
    default: return false;
    }
  }

  bool non_escape_character()
  {
    trace();
    if (match.peek(is_escape_character) || match.peek(is_line_terminator)
        || !source_character())
      return false;
    cv = *match;
    // The CV of CharacterEscapeSequence :: NonEscapeCharacter is the CV of
    // the NonEscapeCharacter.
    return true;
  }

  // bool escape_character() { return match(is_escape_character); }

  bool hex_escape_sequence()
  {
    trace();
    return match([this] {
      if (!match('x'))
        return false;
      if (!hex_digit())
        return false;
      cv = std::lround(mv);
      if (!hex_digit())
        return false;
      cv = cv * 16 + std::lround(mv);
      return true;
    });
  }

  bool unicode_escape_sequence()
  {
    trace();
    return match([this] {
      if (!match('u'))
        return false;
      if (!hex_digit())
        return false;
      cv = std::lround(mv);
      if (!hex_digit())
        return false;
      cv = cv * 16 + std::lround(mv);
      if (!hex_digit())
        return false;
      cv = cv * 16 + std::lround(mv);
      if (!hex_digit())
        return false;
      cv = cv * 16 + std::lround(mv);
      return true;
    });
  }

  // 7.8.5
  bool regular_expression_literal()
  {
    trace();
    auto m = match.mark();
    if (!match('/'))
      return false;
    if (!regular_expression_body() || !match('/')
        || !regular_expression_flags())
      syntax_error();

    sv = {m, match.mark()};
    strings.push_back(std::make_unique<std::u16string>(sv));
    token_value =
        Token::regular_expression_literal(strings.back().get()->data());
    return true;
  }

  bool regular_expression_body()
  {
    trace();
    return regular_expression_first_char() && regular_expression_chars();
  }

  bool regular_expression_chars()
  {
    trace();
    while (regular_expression_char())
      ;
    return true;
  }

  bool regular_expression_first_char()
  {
    trace();
    return match.any_of(
        [this] {
          return !match.any_of('*', '\\', '/', '[')
                 && regular_expression_non_terminator();
        },
        [this] { return regular_expression_backslash_sequence(); },
        [this] { return regular_expression_class(); });
  }

  bool regular_expression_char()
  {
    trace();
    return match.any_of(
        [this] {
          return !match.any_of('\\', '/', '[')
                 && regular_expression_non_terminator();
        },
        [this] { return regular_expression_backslash_sequence(); },
        [this] { return regular_expression_class(); });
  }

  bool regular_expression_backslash_sequence()
  {
    trace();
    return match(
        [this] { return match('\\') && regular_expression_non_terminator(); });
  }

  bool regular_expression_non_terminator()
  {
    trace();
    return match([this] { return !line_terminator() && source_character(); });
  }

  bool regular_expression_class()
  {
    trace();
    return match([this] {
      return match('[') && regular_expression_class_chars() && match(']');
    });
  }

  bool regular_expression_class_chars()
  {
    trace();
    while (regular_expression_class_char())
      ;
    return true;
  }

  bool regular_expression_class_char()
  {
    trace();
    return match.any_of(
        [this] {
          return !match.any_of(']', '\\')
                 && regular_expression_non_terminator();
        },
        [this] { return regular_expression_backslash_sequence(); });
  }

  bool regular_expression_flags()
  {
    trace();
    while (identifier_part())
      ;
    return true;
  }

  void lex()
  {
    trace();

    bool lt  = false;
    int  col = 0, row = 0;
    bool re = true;
    auto s  = match.mark();
    auto m  = match.mark();
    try {
      while (auto input_element =
                 re ? input_element_reg_exp() : input_element_div()) {

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
          debug_infos.emplace_back(s, m + 1, match.mark(), col, row);

          re = reg_exp_allowed(*token);
          lt = false;
          col += distance(m, match.mark());
        }
        m = match.mark();
      }
    }
    catch (const exception& err) {
      DebugInfoImpl di{s, m + 1, match.mark(), col, row};
      throw runtime_error(string{err.what()} + "\n" + di.syntax_error_at());
    }
  }
};

Lexer::Lexer() : grammar(make_shared<LexicalGrammar>())
{
}

Lexer::Lexer(initializer_list<Token> il) : Lexer(vector<Token>{il})
{
}

Lexer::Lexer(vector<Token> tokens) : Lexer()
{
  grammar->tokens = move(tokens);
}

Lexer::Lexer(Source source) : grammar(make_shared<LexicalGrammar>(move(source)))
{
  grammar->lex();
}

Lexer::Lexer(const char* source) : Lexer(Source::from_utf8(source))
{
}

Lexer::Lexer(const char16_t* source) : Lexer(Source::from_utf16(source))
{
}

Lexer::iterator Lexer::begin() const
{
  return grammar->tokens.begin();
}

Lexer::iterator Lexer::end() const
{
  return grammar->tokens.end();
}

Lexer::DebugInfo* Lexer::lookup(const iterator& it) const
{
  auto offset = distance(begin(), it);
  if (offset < grammar->debug_infos.size())
    return &grammar->debug_infos.at(offset);
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

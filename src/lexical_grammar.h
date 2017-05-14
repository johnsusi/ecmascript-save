#ifndef ECMASCRIPT_LEXICAL_GRAMMAR_H
#define ECMASCRIPT_LEXICAL_GRAMMAR_H

#include "input_element.h"
#include "matcher.h"
#include "token.h"

#include <algorithm>
#include <cmath>
#include <iterator>
#include <string>
#include <vector>

// 7.2
bool is_white_space(int cp);

// 7.3
bool is_line_terminator(int cp);

// 7.8.3
bool is_non_zero_digit(int cp);
bool is_decimal_digit(int cp);
bool is_hex_digit(int cp);

// 7.6
bool is_unicode_letter(int cp);
bool is_unicode_combining_mark(int cp);
bool is_unicode_digit(int cp);
bool is_unicode_connector_punctuation(int cp);
bool is_single_escape_character(int cp);

// 7.6.1.1
bool is_keyword(const std::string& token);

// 7.6.1.2
bool is_future_reserved_word(const std::string& token);

// 7.7
bool is_punctuator(const std::string& token);
bool is_div_punctuator(const std::string& token);

// 7.8.1
bool is_null_literal(const std::string& str);

// 7.8.2
bool is_boolean_literal(const std::string&);

constexpr double pow10(int n)
{
  switch (n) {
  case 0: return 1;
  case 1: return 1e1;
  case 2: return 1e2;
  case 3: return 1e3;
  case 4: return 1e4;
  case 5: return 1e5;
  case 6: return 1e6;
  case 7: return 1e7;
  case 8: return 1e8;
  case 9: return 1e9;
  case 10: return 1e10;
  case 11: return 1e11;
  case 12: return 1e12;
  case 13: return 1e13;
  case 14: return 1e14;
  case 15: return 1e15;
  case 16: return 1e16;
  case 17: return 1e17;
  case 18: return 1e18;
  case 19: return 1e19;
  case 20: return 1e20;
  case 21: return 1e21;
  case 22: return 1e22;
  default: return std::numeric_limits<double>::infinity();
  }
}

constexpr double mul_pow10(int s, int n)
{
  return n < 0 ? s / pow10(-n) : s * pow10(n);
}

template <typename T>
class LexicalGrammar {
public:
  std::u16string buffer;
  Matcher<T, std::u16string::const_iterator> match;

  Token token_value = Token::null_literal();

  template <typename It>
  LexicalGrammar(It begin, It end)
      : buffer(begin, end), match(buffer.begin(), buffer.end())
  {
  }

  void syntax_error() { throw std::runtime_error("Syntax Error"); }
  // Lexical Grammar

  // 6
  bool source_character()
  {
    return match([](auto cp) { return true; });
  }

  // 7
  InputElement input_element_div()
  {
    auto match_start = match.mark();
    if (white_space()) return InputElement::white_space();
    if (line_terminator()) return InputElement::line_terminator();
    if (comment())
      return InputElement::comment(std::u16string{match_start, match.mark()});
    if (token()) return InputElement::token(std::move(token_value));
    if (div_punctuator())
      return InputElement::token(
          Token::punctuator({match_start, match.mark()}));
    return InputElement::empty();
  }

  InputElement input_element_reg_exp()
  {
    auto match_start = match.mark();
    if (white_space()) return InputElement::white_space();
    if (line_terminator()) return InputElement::line_terminator();
    if (comment())
      return InputElement::comment(std::u16string{match_start, match.mark()});
    if (token() || regular_expression_literal())
      return InputElement::token(std::move(token_value));
    return InputElement::empty();
  }

  // 7.2 White Space
  bool white_space() { return match(is_white_space); }

  // 7.3 Line Terminators
  bool line_terminator() { return match(is_line_terminator); }

  bool line_terminator_sequence()
  {
    return match([&](auto cp) {
      switch (cp) {
      case 0x000D: // Carriage Return <CR>
        match(0x000A);
      case 0x000A: // Line Feed <LF>
      case 0x2028: // Line separator <LS>
      case 0x2029: // Paragraph separator <PS>
        return true;
      default: return false;
      }
    });
  }

  // 7.4 Comments
  bool comment() { return multi_line_comment() || single_line_comment(); }

  bool multi_line_comment()
  {
    return match([&] {
      return match('/') && match('*') && multi_line_comment_chars_opt()
             && match('*') && match('/');
    });
  }

  bool multi_line_comment_chars()
  {
    return match([this] {
      return (multi_line_not_asterisk_char() && multi_line_comment_chars_opt())
             || (match('*') && post_asterisk_comment_chars());
    });
  }

  bool multi_line_comment_chars_opt()
  {
    multi_line_comment_chars();
    return true;
  }

  bool post_asterisk_comment_chars()
  {
    return match.any_of(
        [this] { return match('*') && post_asterisk_comment_chars(); },
        [this] {
          return multi_line_not_forward_slash_or_asterisk_char()
                 && multi_line_comment_chars_opt();
        });
  }

  bool multi_line_not_asterisk_char()
  {
    return match([this] { return !match('*') && source_character(); });
  }

  bool multi_line_not_forward_slash_or_asterisk_char()
  {
    return match([](auto cp) {
      switch (cp) {
      case '/':
      case '*': return false;
      default: return true;
      }
    });
  }

  bool single_line_comment()
  {
    return match([this] {
      return match('/') && match('/') && single_line_comment_chars();
    });
  }

  bool single_line_comment_chars()
  {
    while (single_line_comment_char())
      ;
    return true;
  }

  bool single_line_comment_char()
  {
    return match([this] { return !line_terminator() && source_character(); });
  }

  // 7.5 Tokens
  bool token()
  {
    token_value = Token("null");
    auto i      = match.mark();
    if (identifier_name()) {
      token_value = Token::identifier_name(std::u16string{i, match.mark()});
      return true;
    }
    if (punctuator()) {
      token_value = Token::punctuator({i, match.mark()});
      return true;
    }
    if (numeric_literal()) {
      token_value = Token::numeric_literal(mv);
      return true;
    }
    if (string_literal()) {
      token_value = Token::string_literal(sv);
      return true;
    }
    return false;
  }

  bool identifier_name()
  {
    if (identifier_start()) {
      while (identifier_part())
        ;
      return true;
    }
    return false;
  }

  bool identifier_start()
  {
    return unicode_letter() || match('$') || match('_') || match([this] {
             return match('\\') && unicode_escape_sequence();
           });
  }

  bool identifier_part()
  {
    return identifier_start() || unicode_combining_mark() || unicode_digit()
           || unicode_connector_punctuation()
           || match(0x200C) /* <ZWNJ> */ || match(0x200D) /* <ZWJ> */;
  }

  bool unicode_letter() { return match(is_unicode_letter); }

  bool unicode_combining_mark() { return match(is_unicode_combining_mark); }

  bool unicode_digit() { return match(is_unicode_digit); }

  bool unicode_connector_punctuation()
  {
    return match(is_unicode_connector_punctuation);
  }

  bool reserved_word()
  {
    return keyword() || future_reserved_word() || null_literal()
           || boolean_literal();
  }

  bool keyword()
  {
    return match.any_of("break", "case", "catch", "continue", "debugger",
                        "default", "delete", "do", "else", "finally", "for",
                        "function", "if", "in", "instanceof", "new", "return",
                        "switch", "this", "throw", "try", "typeof", "var",
                        "void", "while", "with");
  }

  bool future_reserved_word()
  {
    return match.any_of("class", "enum", "extends", "super", "const", "export",
                        "import", "implements", "let", "private", "public",
                        "interface", "package", "protected", "static");
  }

  // 7.7
  bool punctuator()
  {
    return match.any_of(
        // Note: Must match longer strings first
        ">>>=", "<<=", ">>=", ">>>", "!==", "===", "!=", "%=", "&&",
        "&=", "*=", "*", "++", "+=", "--", "-=", "<<", "<=", "==", ">=", ">>",
        "^=", "|=", "||", "!", "%", "&", "?", "+", ",", "-", ".", ":", ";", "=",
        ">", "<", "(", ")", "[", "]", "^", "{", "|", "}", "~");
  }

  bool div_punctuator() { return match.any_of("/=", "/"); }

  // 7.8
  bool literal()
  {
    return null_literal() || boolean_literal() || numeric_literal()
           || string_literal() || regular_expression_literal();
  }

  // 7.8.1
  bool null_literal()
  {
    if (match("null")) {
      token_value = Token(nullptr);
      return true;
    }
    return false;
  }

  // 7.8.2
  bool boolean_literal()
  {
    if (match("true")) {
      token_value = Token(true);
      return true;
    }
    else if (match("false")) {
      token_value = Token(false);
      return true;
    }
    else
      return false;
  }

  // 7.8.3
  double mv;
  bool   numeric_literal()
  {
    if (hex_integer_literal() || decimal_literal()) {
      if (identifier_start() || decimal_digit()) syntax_error();
      return true;
    }
    return false;
  }

  bool decimal_literal()
  {
    long s = 0, n = 0;

    if (match('.')) {
      auto m = match.mark();
      if (!decimal_integer_literal()) syntax_error();
      n = match.distance(m);
      s = std::lround(mv);
    }
    else if (decimal_integer_literal()) {
      s = std::lround(mv);
      if (match('.')) {
        auto m = match.mark();
        if (decimal_integer_literal()) {
          n = match.distance(m);
          s = std::lround(mul_pow10(s, n)) + std::lround(mv);
        }
      }
    }
    else
      return false;

    if (exponent_part()) n -= std::lround(mv);

    mv = mul_pow10(s, -n);

    return true;
  }

  bool decimal_integer_literal()
  {
    if (match('0')) {
      mv = 0;
      return true;
    }
    return decimal_digits();
  }

  bool decimal_digits()
  {
    if (!decimal_digit()) return false;
    auto s = mv;
    while (decimal_digit()) {
      s = s * 10 + mv;
    }
    mv = s;
    return true;
  }

  bool decimal_digit()
  {
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
    if (!exponent_indicator()) return false;
    if (!signed_integer()) syntax_error();
    return true;
  }

  bool exponent_indicator()
  {
    return match([](auto cp) { return cp == 'e' || cp == 'E'; });
  }

  bool signed_integer()
  {

    if (match("+")) {
      if (!decimal_digits()) syntax_error();
      return true;
    }
    if (match("-")) {
      if (!decimal_digits()) syntax_error();
      mv = -mv;
      return true;
    }
    return decimal_digits();
  }

  bool hex_integer_literal()
  {
    if (!match("0x") && !match("0X")) return false;
    if (!hex_digits()) syntax_error();
    return true;
  }

  bool hex_digits()
  {
    if (!hex_digit()) return false;
    double s = mv;
    while (hex_digit()) {
      s = 16 * s + mv;
    }
    mv = s;
    return true;
  }

  bool hex_digit()
  {
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
    if (match('"')) {
      sv = {};
      double_string_characters();
      if (!match('"')) syntax_error();
      return true;
    }
    else if (match('\'')) {
      sv = {};
      single_string_characters();
      if (!match('\'')) syntax_error();
      return true;
    }
    else
      return false;
  }

  void double_string_characters()
  {
    while (double_string_character()) sv += cv;
  }

  void single_string_characters()
  {
    while (single_string_character()) sv += cv;
  }

  bool double_string_character()
  {
    return match([&](auto cp) {
      switch (cp) {
      case '"': return false;
      case '\\': return escape_sequence() || line_terminator_sequence();
      default:
        if (is_line_terminator(cp)) return false;
        cv = *match;
        return true;
      }
    });
  }

  bool single_string_character()
  {
    return match([&](auto cp) {
      switch (cp) {
      case '\'': return false;
      case '\\': return escape_sequence() || line_terminator_sequence();
      default:
        if (is_line_terminator(cp)) return false;
        cv = *match;
        return true;
      }
    });
  }

  bool escape_sequence()
  {
    if (match.peek('0') && !match.lookahead(is_decimal_digit)) {
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
    return single_escape_character() || non_escape_character();
  }

  static bool is_single_escape_character(int cp)
  {
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
    return match([this] {
      if (!match('x')) return false;
      if (!hex_digit()) return false;
      cv = std::lround(mv);
      if (!hex_digit()) return false;
      cv = cv * 16 + std::lround(mv);
      return true;
    });
  }

  bool unicode_escape_sequence()
  {
    return match([this] {
      if (!match('u')) return false;
      if (!hex_digit()) return false;
      cv = std::lround(mv);
      if (!hex_digit()) return false;
      cv = cv * 16 + std::lround(mv);
      if (!hex_digit()) return false;
      cv = cv * 16 + std::lround(mv);
      if (!hex_digit()) return false;
      cv = cv * 16 + std::lround(mv);
      return true;
    });
  }

  // 7.8.5
  bool regular_expression_literal()
  {
    return match([this] {
      return match('/') && regular_expression_body() && match('/')
             && regular_expression_flags();
    });
  }

  bool regular_expression_body()
  {
    return regular_expression_first_char() && regular_expression_chars();
  }

  bool regular_expression_chars()
  {
    while (regular_expression_char())
      ;
    return true;
  }

  bool regular_expression_first_char()
  {
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
    return match(
        [this] { return match('\\') && regular_expression_non_terminator(); });
  }

  bool regular_expression_non_terminator()
  {
    return match([this] { return !line_terminator() && source_character(); });
  }

  bool regular_expression_class()
  {
    return match([this] {
      return match('[') && regular_expression_class_chars() && match(']');
    });
  }

  bool regular_expression_class_chars()
  {
    while (regular_expression_class_char())
      ;
    return true;
  }

  bool regular_expression_class_char()
  {
    return match.any_of(
        [this] {
          return !match.any_of(']', '\\')
                 && regular_expression_non_terminator();
        },
        [this] { return regular_expression_backslash_sequence(); });
  }

  bool regular_expression_flags()
  {
    while (identifier_part())
      ;
    return true;
  }
};

#endif
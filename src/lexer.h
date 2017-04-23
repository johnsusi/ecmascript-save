#ifndef ECMASCRIPT_LEXER_H
#define ECMASCRIPT_LEXER_H

#include "input_element.h"
#include "matcher.h"
#include "token.h"

#include <cmath>
#include <iostream>
#include <iterator>
#include <string>
#include <vector>

#include <boost/range/any_range.hpp>

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

// 7.8.1
bool is_null_literal(const std::string& str);

// 7.8.2
bool is_boolean_literal(const std::string&);

struct Lexer
{

  using code_point_t = char16_t;


  std::u16string source;
  Matcher<code_point_t, std::u16string::const_iterator> match;

  template <typename It>
  Lexer(It begin, It end) : source(begin, end), match(source.begin(), source.end()) {}

  std::vector<Token> result;

  int decimal_value;
  int decimal_count;
  int exponent_value;

  auto tokens()
  {
    if (!result.empty()) return result;
    bool lt = false;
    int col = 0, row = 0;
    while (auto input_element = input_element_div())
    {

      if (input_element.is_white_space()) col++;
      else if (input_element.has_line_terminator()) {
        lt = true;
        col = 0;
        row++;
      }
      else if (auto token = input_element.to_token()) {
        token->preceded_by_line_terminator = lt;
        result.push_back(std::move(*token));
        lt = false;
      }
    }
    return result;
  }

  Token token_value;

  void syntax_error()
  {
    throw std::runtime_error("Syntax Error");
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
    auto match_start = match.mark();
    if (white_space()) return InputElement::white_space();
    if (line_terminator()) return InputElement::line_terminator();
    if (comment()) return InputElement::comment(std::u16string {match_start, match.mark()});
    if (token() || div_punctuator()) return InputElement::token(std::move(token_value));
    return InputElement::empty();
  }

  InputElement input_element_reg_exp()
  {
    auto match_start = match.mark();
    if (white_space()) return InputElement::white_space();
    if (line_terminator()) return InputElement::line_terminator();
    if (comment()) return InputElement::comment(std::u16string{match_start, match.mark()});
    if (token() || regular_expression_literal()) return InputElement::token(std::move(token_value));
    return InputElement::empty();
  }

  // 7.2 White Space
  bool white_space()
  {
    return match(is_white_space);
  }

  // 7.3 Line Terminators
  bool line_terminator()
  {
    return match(is_line_terminator);
  }

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
        default:
          return false;
      }
    });
  }

  // 7.4 Comments
  bool comment()
  {
    return multi_line_comment() || single_line_comment();
  }

  bool multi_line_comment()
  {
    return match([&]{
      return match('/') && match('*') && multi_line_comment_chars_opt() &&
        match('*') && match('/');
    });
  }

  bool multi_line_comment_chars()
  {
    return match([this]{
      return (multi_line_not_asterisk_char() && multi_line_comment_chars_opt()) ||
             (match('*') && post_asterisk_comment_chars());
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
      [this]{ return match('*') && post_asterisk_comment_chars(); },
      [this]{ return multi_line_not_forward_slash_or_asterisk_char() &&
        multi_line_comment_chars_opt(); }
    );
  }

  bool multi_line_not_asterisk_char()
  {
    return match([this]{ return !match('*') && source_character(); });
  }

  bool multi_line_not_forward_slash_or_asterisk_char()
  {
    return match([](auto cp) {
      switch (cp) {
        case '/':
        case '*':
          return false;
        default:
          return true;
      }
    });
  }

  bool single_line_comment()
  {
    return match([this]{
      return match('/') && match('/') && single_line_comment_chars();
    });
  }

  bool single_line_comment_chars()
  {
    while (single_line_comment_char());
    return true;
  }

  bool single_line_comment_char()
  {
    return match([this]{
      return !line_terminator() && source_character();
    });
  }

  // 7.5 Tokens
  bool token()
  {
    token_value = {};
    auto i = match.mark();
    if (identifier_name()) {
      token_value = {i, match.mark()};
      return true;
    }
    if (punctuator()) {
      token_value = Token::punctuator({i, match.mark()});
      return true;
    }
    if (numeric_literal()) {
      token_value = Token::numeric_literal( std::strtod(std::string{i, match.mark()}.c_str(), nullptr) );
      return true;
    }
    if (string_literal()) {
      token_value = Token::string_literal( {i+1, match.mark()-1} );
      return true;
    }
    return false;
  }

  bool identifier_name()
  {
    if (identifier_start()) {
      while (identifier_part());
      return true;
    }
    return false;
  }

  bool identifier_start() {
    return unicode_letter() || match('$') || match('_') || match([this]{
      return match('\\') && unicode_escape_sequence();
    });
  }

  bool identifier_part()
  {
    return identifier_start() || unicode_combining_mark() || unicode_digit() ||
      unicode_connector_punctuation() || match(0x200C) /* <ZWNJ> */ ||
      match(0x200D) /* <ZWJ> */;
  }

  bool unicode_letter()
  {
    return match(is_unicode_letter);
  }

  bool unicode_combining_mark()
  {
    return match(is_unicode_combining_mark);
  }

  bool unicode_digit()
  {
    return match(is_unicode_digit);
  }

  bool unicode_connector_punctuation()
  {
    return match(is_unicode_connector_punctuation);
  }

  bool reserved_word()
  {
    return keyword() || future_reserved_word() || null_literal() ||
      boolean_literal();
  }

  bool keyword()
  {
    return match.any_of("break", "case", "catch", "continue", "debugger",
      "default", "delete", "do", "else", "finally", "for", "function", "if",
      "in", "instanceof", "new", "return", "switch", "this", "throw", "try",
      "typeof", "var", "void", "while", "with");
  }

  bool future_reserved_word()
  {
    return match.any_of( "class", "enum", "extends", "super", "const", "export",
      "import", "implements", "let", "private", "public", "interface",
      "package", "protected", "static");
  }

  // bool null_literal()
  // {
  //   return match("null");
  // }

  // bool boolean_literal()
  // {
  //   return match.any_of("true", "false");
  // }

  // 7.7
  bool punctuator()
  {
    return match.any_of(
      ">>>=",
      "<<=", ">>=", ">>>", "!==",
      "!=", "%=", "&&", "&=", "*", "*=", "++", "+=", "--", "-=", "<<",  "<=",
      "==", "===", ">=", ">>",
      "!", "%", "&", "?", "+",  ",", "-",  ".", ":", ";", "=", ">", "<", "(",
      ")", "[", "]", "^", "^=", "{", "|", "|=", "||", "}", "~");
  }

  bool div_punctuator()
  {
    if (match('/')) {
      match('=');
      return true;
    }
    return false;
  }

  // 7.8
  bool literal()
  {
    return null_literal() || boolean_literal() || numeric_literal() ||
      string_literal() || regular_expression_literal();
  }

  // 7.8.1
  bool null_literal()
  {
    if (match("null")) {
      token_value = Token::null_literal();
      return true;
    }
    return false;
  }

  // 7.8.2
  bool boolean_literal()
  {
    if (match("true")) {
      token_value = Token::boolean_literal(true);
      return true;
    }
    else if (match("false")) {
      token_value = Token::boolean_literal(false);
      return true;
    }
    else return false;
  }

  // 7.8.3
  bool numeric_literal()
  {
    if (decimal_literal() || hex_integer_literal()) {
      return true;
    }
    return false;
  }

  bool decimal_literal()
  {
    return match([&]{
      if (decimal_integer_literal() && match('.')) {
        decimal_digits();
        exponent_part();
        return true;
      }
      return false;
    }) || match([&]{
      if (match('.') && decimal_digits()) {
        exponent_part();
        return true;
      }
      return false;
    }) || match([&]{
      if (decimal_integer_literal()) {
        exponent_part();
        return true;
      }
      return false;
    });
  }

  bool decimal_integer_literal()
  {
    if (match('0')) return true;
    if (match(is_non_zero_digit)) {
      decimal_digits();
      return true;
    }
    return false;
  }

  bool decimal_digits()
  {
    if (decimal_digit()) {
      decimal_value = *match - u'0';
      decimal_count = 1;
      while (decimal_digit()) {
        decimal_value = decimal_value * 10 + (*match - u'0');
        decimal_count += 1;
      }
      return true;
    }
    return false;
  }

  bool decimal_digit()
  {
    return match(is_decimal_digit);
  }

  bool exponent_part()
  {
    if (exponent_indicator()) {
      if (!signed_integer()) syntax_error();
      exponent_value = decimal_value;
      return true;
    }
    return false;
  }

  bool exponent_indicator() {
    return match([](auto cp) {
      return cp == 'e' || cp == 'E';
    });
  }

  bool signed_integer()
  {

    if (match("+")) {
      if (!decimal_digits()) syntax_error();
      return true;
    }
    if (match("-")) {
      if (!decimal_digits()) syntax_error();
    }
    return decimal_digits();
  }

  bool hex_integer_literal()
  {
    return match([&]{
      return match("0x") && hex_digits();
    }) || match([&]{
      return match("0X") && hex_digits();
    });
  }

  bool hex_digits()
  {
    if (hex_digit()) return false;
    while (hex_digit());
    return true;
  }

  bool hex_digit()
  {
    return match(is_hex_digit);
  }

  // 7.8.4
  bool string_literal()
  {
    if (match([&]{
      return match('"') && double_string_characters(), match('"');
    }) || match([&]{
      return match('\'') && single_string_characters(), match('\'');
    })) {
      return true;
    }
    return false;
  }

  bool double_string_characters()
  {
    if (!double_string_character()) return false;
    while (double_string_character());
    return true;
  }

  bool single_string_characters()
  {
    if (!single_string_character()) return false;
    while (single_string_character());
    return true;
  }

  bool double_string_character()
  {
    return match([&](auto cp) {
      switch (cp) {
        case '"': return false;
        case '\\': return escape_sequence() || line_terminator_sequence();
        default:   return !is_line_terminator(cp);
      }
    });
  }

  bool single_string_character()
  {
    return match([&](auto cp) {
      switch (cp) {
        case '\'': return false;
        case '\\': return escape_sequence() || line_terminator_sequence();
        default:   return !is_line_terminator(cp);
      }
    });
  }

  bool escape_sequence()
  {
    return match([this]{
      return character_escape_sequence() ||
             hex_escape_sequence() ||
             unicode_escape_sequence() ||
             (match('0') && !match(is_decimal_digit));
    });
  }

  bool character_escape_sequence()
  {
    return single_escape_character() || non_escape_character();
  }

  bool single_escape_character()
  {
    return match(is_single_escape_character);
  }

  bool non_escape_character()
  {
    return match([this]{
      return !escape_character() && !line_terminator() && source_character();
    });
  }

  bool escape_character()
  {
    return single_escape_character() || decimal_digit() || match('x') || match('u');
  }

  bool hex_escape_sequence()
  {
    return match([this]{
      return match('x') && hex_digit() && hex_digit();
    });
  }

  bool unicode_escape_sequence()
  {
    return match([this] {
      return match('u') && hex_digit() && hex_digit() && hex_digit() && hex_digit();
    });
  }

  // 7.8.5
  bool regular_expression_literal()
  {
    return match([this]{
      return match('/') && regular_expression_body() && match('/') &&
        regular_expression_flags();
    });
  }

  bool regular_expression_body()
  {
    return regular_expression_first_char() && regular_expression_chars();
  }

  bool regular_expression_chars()
  {
    while (regular_expression_char());
    return true;
  }

  bool regular_expression_first_char()
  {
    return match.any_of(
      [this]{ return !match.any_of('*', '\\', '/', '[') && regular_expression_non_terminator(); },
      [this]{ return regular_expression_backslash_sequence(); },
      [this]{ return regular_expression_class(); }
    );
  }

  bool regular_expression_char()
  {
    return match.any_of(
      [this]{ return !match.any_of('\\', '/', '[') && regular_expression_non_terminator(); },
      [this]{ return regular_expression_backslash_sequence(); },
      [this]{ return regular_expression_class(); }
    );
  }

  bool regular_expression_backslash_sequence()
  {
    return match([this]{
      return match('\\') && regular_expression_non_terminator();
    });
  }

  bool regular_expression_non_terminator()
  {
    return match([this]{
      return !line_terminator() && source_character();
    });
  }

  bool regular_expression_class()
  {
    return match([this]{
      return match('[') && regular_expression_class_chars() && match(']');
    });
  }

  bool regular_expression_class_chars()
  {
    while (regular_expression_class_char());
    return true;
  }

  bool regular_expression_class_char()
  {
    return match.any_of(
      [this]{ return !match.any_of(']', '\\') && regular_expression_non_terminator(); },
      [this]{ return regular_expression_backslash_sequence(); }
    );
  }

  bool regular_expression_flags()
  {
    while (identifier_part());
    return true;
  }
};

template <typename It>
auto make_lexer(It f, It l)
{
  return Lexer {f, l};
}

template <typename Cont>
auto make_lexer(Cont&& cont)
{
  return make_lexer(std::begin(cont), std::end(cont));
}

#endif
#ifndef ECMASCRIPT_TOKEN_H
#define ECMASCRIPT_TOKEN_H

#include "util.h"
#include "types/number.h"

#include <algorithm>
#include <cstddef>
#include <iostream>
#include <memory>
#include <string>
#include <utility>
#include <vector>

// using std::array;
template <typename T, std::size_t N>
struct array {

  T data[N];

  constexpr std::size_t size() const
  {
    return N;
  }

  constexpr auto begin() const
  {
    return data;
  }

  constexpr auto end() const
  {
    return data + N;
  }
};

class Token {

  using type_t = std::size_t;

  // Lower 7 bits reserved for type index [0 94).
  static const type_t TYPE_MASK            = 0b00'111111111'1111111;
  static const type_t CATEGORY_MASK        = 0b00'111111111'0000000;
  static const type_t INDEX_MASK           = 0b00'000000000'1111111;
  static const type_t IDENTIFIER           = 0b00'000000001'0000000;
  static const type_t KEYWORD              = 0b00'000000010'0000000;
  static const type_t PUNCTUATOR           = 0b00'000000100'0000000;
  static const type_t FUTURE_RESERVED_WORD = 0b00'000001000'0000000;
  static const type_t NULL_LITERAL         = 0b00'000010000'0000000;
  static const type_t BOOLEAN_LITERAL      = 0b00'000100000'0000000;
  static const type_t NUMERIC_LITERAL      = 0b00'001000000'0000000;
  static const type_t STRING_LITERAL       = 0b00'010000000'0000000;
  static const type_t REG_EXP_LITERAL      = 0b00'100000000'0000000;
  static const type_t REG_EXP_ALLOWED      = 0b01'000000000'0000000;
  static const type_t LT_PRECEDED          = 0b10'000000000'0000000;

  static const type_t LITERAL = NULL_LITERAL | BOOLEAN_LITERAL | NUMERIC_LITERAL
                                | STRING_LITERAL | REG_EXP_LITERAL;

  static const type_t RESERVED_WORD =
      KEYWORD | FUTURE_RESERVED_WORD | NULL_LITERAL | BOOLEAN_LITERAL;

  type_t m_type;

  union value_t {
    const char*     c_str;
    double          numeric_value;
    std::u16string* string_value;
    constexpr value_t() : c_str(nullptr)
    {
    }
    constexpr value_t(const char* c_str) : c_str(c_str)
    {
    }
    constexpr value_t(double value) : numeric_value(value)
    {
    }
    constexpr value_t(std::u16string* value) : string_value(value)
    {
    }
  } m_value;

  constexpr Token(type_t type, value_t value) : m_type(type), m_value(value)
  {
  }

  static constexpr std::pair<const char*, Token>
  make_token_pair(const char* str, type_t type)
  {
    return {str, Token{type, {str}}};
  }

  static constexpr array<std::pair<const char*, Token>, 95> make_tokens()
  {
    int seq = 0;
    return {{make_token_pair("!", PUNCTUATOR | ++seq | REG_EXP_ALLOWED),
             make_token_pair("!=", PUNCTUATOR | ++seq | REG_EXP_ALLOWED),
             make_token_pair("!==", PUNCTUATOR | ++seq | REG_EXP_ALLOWED),
             make_token_pair("%", PUNCTUATOR | ++seq | REG_EXP_ALLOWED),
             make_token_pair("%=", PUNCTUATOR | ++seq | REG_EXP_ALLOWED),
             make_token_pair("&", PUNCTUATOR | ++seq | REG_EXP_ALLOWED),
             make_token_pair("&&", PUNCTUATOR | ++seq | REG_EXP_ALLOWED),
             make_token_pair("&=", PUNCTUATOR | ++seq | REG_EXP_ALLOWED),
             make_token_pair("(", PUNCTUATOR | ++seq | REG_EXP_ALLOWED),
             make_token_pair(")", PUNCTUATOR | ++seq),
             make_token_pair("*", PUNCTUATOR | ++seq | REG_EXP_ALLOWED),
             make_token_pair("*=", PUNCTUATOR | ++seq | REG_EXP_ALLOWED),
             make_token_pair("+", PUNCTUATOR | ++seq | REG_EXP_ALLOWED),
             make_token_pair("++", PUNCTUATOR | ++seq),
             make_token_pair("+=", PUNCTUATOR | ++seq | REG_EXP_ALLOWED),
             make_token_pair(",", PUNCTUATOR | ++seq | REG_EXP_ALLOWED),
             make_token_pair("-", PUNCTUATOR | ++seq | REG_EXP_ALLOWED),
             make_token_pair("--", PUNCTUATOR | ++seq),
             make_token_pair("-=", PUNCTUATOR | ++seq | REG_EXP_ALLOWED),
             make_token_pair(".", PUNCTUATOR | ++seq),
             make_token_pair("/", PUNCTUATOR | ++seq | REG_EXP_ALLOWED),
             make_token_pair("/=", PUNCTUATOR | ++seq | REG_EXP_ALLOWED),
             make_token_pair(":", PUNCTUATOR | ++seq | REG_EXP_ALLOWED),
             make_token_pair(";", PUNCTUATOR | ++seq | REG_EXP_ALLOWED),
             make_token_pair("<", PUNCTUATOR | ++seq | REG_EXP_ALLOWED),
             make_token_pair("<<", PUNCTUATOR | ++seq | REG_EXP_ALLOWED),
             make_token_pair("<<=", PUNCTUATOR | ++seq | REG_EXP_ALLOWED),
             make_token_pair("<=", PUNCTUATOR | ++seq | REG_EXP_ALLOWED),
             make_token_pair("=", PUNCTUATOR | ++seq | REG_EXP_ALLOWED),
             make_token_pair("==", PUNCTUATOR | ++seq | REG_EXP_ALLOWED),
             make_token_pair("===", PUNCTUATOR | ++seq | REG_EXP_ALLOWED),
             make_token_pair(">", PUNCTUATOR | ++seq | REG_EXP_ALLOWED),
             make_token_pair(">=", PUNCTUATOR | ++seq | REG_EXP_ALLOWED),
             make_token_pair(">>", PUNCTUATOR | ++seq | REG_EXP_ALLOWED),
             make_token_pair(">>=", PUNCTUATOR | ++seq | REG_EXP_ALLOWED),
             make_token_pair(">>>", PUNCTUATOR | ++seq | REG_EXP_ALLOWED),
             make_token_pair(">>>=", PUNCTUATOR | ++seq | REG_EXP_ALLOWED),
             make_token_pair("?", PUNCTUATOR | ++seq | REG_EXP_ALLOWED),
             make_token_pair("[", PUNCTUATOR | ++seq | REG_EXP_ALLOWED),
             make_token_pair("]", PUNCTUATOR | ++seq),
             make_token_pair("^", PUNCTUATOR | ++seq | REG_EXP_ALLOWED),
             make_token_pair("^=", PUNCTUATOR | ++seq | REG_EXP_ALLOWED),
             make_token_pair("break", KEYWORD | ++seq),
             make_token_pair("case", KEYWORD | ++seq | REG_EXP_ALLOWED),
             make_token_pair("catch", KEYWORD | ++seq),
             make_token_pair("class", FUTURE_RESERVED_WORD | ++seq),
             make_token_pair("const", FUTURE_RESERVED_WORD | ++seq),
             make_token_pair("continue", KEYWORD | ++seq),
             make_token_pair("debugger", KEYWORD | ++seq),
             make_token_pair("default", KEYWORD | ++seq),
             make_token_pair("delete", KEYWORD | ++seq | REG_EXP_ALLOWED),
             make_token_pair("do", KEYWORD | ++seq),
             make_token_pair("else", KEYWORD | ++seq | REG_EXP_ALLOWED),
             make_token_pair("enum", FUTURE_RESERVED_WORD | ++seq),
             make_token_pair("export", FUTURE_RESERVED_WORD | ++seq),
             make_token_pair("extends", FUTURE_RESERVED_WORD | ++seq),
             make_token_pair("false", BOOLEAN_LITERAL | ++seq),
             make_token_pair("finally", KEYWORD | ++seq),
             make_token_pair("for", KEYWORD | ++seq),
             make_token_pair("function", KEYWORD | ++seq),
             make_token_pair("get", IDENTIFIER | ++seq),
             make_token_pair("if", KEYWORD | ++seq),
             make_token_pair("implements", FUTURE_RESERVED_WORD | ++seq),
             make_token_pair("import", FUTURE_RESERVED_WORD | ++seq),
             make_token_pair("in", KEYWORD | ++seq | REG_EXP_ALLOWED),
             make_token_pair("instanceof", KEYWORD | ++seq | REG_EXP_ALLOWED),
             make_token_pair("interface", FUTURE_RESERVED_WORD | ++seq),
             make_token_pair("let", FUTURE_RESERVED_WORD | ++seq),
             make_token_pair("new", KEYWORD | ++seq | REG_EXP_ALLOWED),
             make_token_pair("null", NULL_LITERAL | ++seq),
             make_token_pair("package", FUTURE_RESERVED_WORD | ++seq),
             make_token_pair("private", FUTURE_RESERVED_WORD | ++seq),
             make_token_pair("protected", FUTURE_RESERVED_WORD | ++seq),
             make_token_pair("public", FUTURE_RESERVED_WORD | ++seq),
             make_token_pair("return", KEYWORD | ++seq | REG_EXP_ALLOWED),
             make_token_pair("set", IDENTIFIER | ++seq),
             make_token_pair("static", FUTURE_RESERVED_WORD | ++seq),
             make_token_pair("super", FUTURE_RESERVED_WORD | ++seq),
             make_token_pair("switch", KEYWORD | ++seq),
             make_token_pair("this", KEYWORD | ++seq),
             make_token_pair("throw", KEYWORD | ++seq | REG_EXP_ALLOWED),
             make_token_pair("true", BOOLEAN_LITERAL | ++seq),
             make_token_pair("try", KEYWORD | ++seq),
             make_token_pair("typeof", KEYWORD | ++seq | REG_EXP_ALLOWED),
             make_token_pair("var", KEYWORD | ++seq),
             make_token_pair("void", KEYWORD | ++seq | REG_EXP_ALLOWED),
             make_token_pair("while", KEYWORD | ++seq),
             make_token_pair("with", KEYWORD | ++seq),
             make_token_pair("yield", FUTURE_RESERVED_WORD | ++seq),
             make_token_pair("{", PUNCTUATOR | ++seq | REG_EXP_ALLOWED),
             make_token_pair("|", PUNCTUATOR | ++seq | REG_EXP_ALLOWED),
             make_token_pair("|=", PUNCTUATOR | ++seq | REG_EXP_ALLOWED),
             make_token_pair("||", PUNCTUATOR | ++seq | REG_EXP_ALLOWED),
             make_token_pair("}", PUNCTUATOR | ++seq),
             make_token_pair("~", PUNCTUATOR | ++seq | REG_EXP_ALLOWED)}};
  }

  template <typename T1, typename T2>
  static constexpr bool less(const T1* lhs, const T2* rhs)
  {
    for (; *lhs || *rhs; ++lhs, ++rhs)
      if (*lhs != *rhs)
        return *lhs < *rhs;
    return false;
  }

  template <typename T1, typename T2>
  static constexpr bool equal(const T1* lhs, const T2* rhs)
  {
    for (; *lhs || *rhs; ++lhs, ++rhs)
      if (*lhs != *rhs) return false;
    return true;
  }
  template <typename T>
  static constexpr Token find(const T* str)
  {
    constexpr auto tokens = make_tokens();
    // Find lower bound. The range is sorted on the first value in each pair.
    auto count = tokens.size();
    auto first = tokens.begin();
    auto last  = tokens.end();
    while (count > 0) {
      auto it   = first;
      auto step = count / 2;
      it += step;
      if (less(it->first, str)) {
        first = ++it;
        count -= step + 1;
      }
      else
        count = step;
    }

    if (first == last || less(str, first->first))
      return {0, {""}};
    return first->second;
  }

  template <typename Value>
  constexpr explicit Token(type_t type, Value value)
      : m_type(type), m_value(value)
  {
  }

public:
  constexpr Token() : Token(0, {""})
  {
  }

  template <typename T>
  constexpr Token(const T* str) : Token(find(str))
  {
    if (m_type == 0)
      throw std::logic_error("Cannot create compile time token");
  }

  static Token identifier(std::u16string* value)
  {
    return Token(IDENTIFIER, {value});
  }

  static Token identifier_name(std::u16string* value)
  {
    if (auto token = find(value->data())) {
      return token;
    }
    else
      return identifier(value);
  }

  static constexpr Token null_literal()
  {
    return Token("null");
  }

  static constexpr Token boolean_literal(bool value)
  {
    return value ? Token("true") : Token("false");
  }

  static Token numeric_literal(double value)
  {
    return {NUMERIC_LITERAL, {value}};
  }

  static Token string_literal(std::u16string* value)
  {
    return {STRING_LITERAL, {value}};
  }

  static Token regular_expression_literal(std::u16string* value)
  {
    return {REG_EXP_LITERAL, {value}};
  }

  constexpr type_t type() const
  {
    return m_type & TYPE_MASK;
  }

  constexpr operator type_t() const
  {
    return m_type & TYPE_MASK;
  }

  constexpr type_t category() const
  {
    return m_type & CATEGORY_MASK;
  }

  constexpr type_t index() const
  {
    return m_type & INDEX_MASK;
  }

  explicit operator bool() const
  {
    return m_type != 0;
  }

  constexpr bool is_identifier() const
  {
    return (m_type & IDENTIFIER) != 0;
  }

  constexpr bool is_identifier_name() const
  {
    return is_identifier() || is_reserved_word();
  }

  constexpr bool is_reserved_word() const
  {
    return (m_type & RESERVED_WORD) != 0;
  }

  constexpr bool is_keyword() const
  {
    return (m_type & KEYWORD) != 0;
  }

  constexpr bool is_future_reserved_word() const
  {
    return (m_type & FUTURE_RESERVED_WORD) != 0;
  }

  constexpr bool is_punctuator() const
  {
    return (m_type & PUNCTUATOR) != 0;
  }

  constexpr bool is_null_literal() const
  {
    return (m_type & NULL_LITERAL) != 0;
  }

  constexpr bool is_boolean_literal() const
  {
    return (m_type & BOOLEAN_LITERAL) != 0;
  }

  constexpr bool is_numeric_literal() const
  {
    return (m_type & NUMERIC_LITERAL) != 0;
  }

  constexpr bool is_string_literal() const
  {
    return (m_type & STRING_LITERAL) != 0;
  }

  constexpr bool is_regular_expression_literal() const
  {
    return (m_type & REG_EXP_LITERAL) != 0;
  }

  constexpr bool is_literal() const
  {
    return (m_type & LITERAL) != 0;
  }

  constexpr bool is_preceded_by_line_terminator() const
  {
    return (m_type & LT_PRECEDED) == LT_PRECEDED;
  }

  constexpr void set_preceded_by_line_terminator()
  {
    m_type |= LT_PRECEDED;
  }

  constexpr bool regular_expression_allowed() const
  {
    return (m_type & REG_EXP_ALLOWED) == REG_EXP_ALLOWED;
  }

  constexpr bool boolean_value() const
  {
    // return m_value.boolean_value;
    switch (type()) {
    case find("true"): return true;
    case find("false"): return false;
    default: return fail_with_logic_error("Invalid type");
    // default: throw std::runtime_error("Invalid type");
    }
  }

  constexpr double numeric_value() const
  {
    return m_value.numeric_value;
  }

  constexpr std::u16string* string_value() const
  {
    return m_value.string_value;
    // if (index() > 0) {
    //   std::string s = {m_value.c_str};
    //   return {s.begin(), s.end()};
    // }
    // if (is_numeric_literal()) {
    //   auto s = std::to_string(numeric_value());
    //   return {s.begin(), s.end()};
    // }
    // if (is_string_literal() || is_regular_expression_literal()
    //     || is_identifier())
    //   return {m_value.string_value};
    // return u"something else";
  }

  constexpr bool operator==(const Token& other) const
  {
    if (type() != other.type())
      return false;
    switch (category()) {
    case NULL_LITERAL: return true;
    case BOOLEAN_LITERAL: return boolean_value() == other.boolean_value();
    case NUMERIC_LITERAL: return numeric_value() == other.numeric_value();
    case STRING_LITERAL: return string_value() == other.string_value();
    case REG_EXP_LITERAL: return string_value() == other.string_value();
    case IDENTIFIER: return string_value() == other.string_value();
    default: return true;
    }
  }

  constexpr bool operator!=(const Token& other) const
  {
    return !operator==(other);
  }

  bool any_of(std::initializer_list<Token> tokens) const
  {
    for (auto it = tokens.begin(); it != tokens.end(); ++it)
      if (*this == *it)
        return true;
    return false;
  }

  template <typename... Args>
  constexpr bool any_of(Args&&... args) const
  {
    return any_of(std::initializer_list<Token>{std::forward<Args>(args)...});
  }

  operator std::string() const
  {
    return to_string();
  }

  std::string to_string() const
  {
    switch (m_type & CATEGORY_MASK) {
    case NUMERIC_LITERAL: return std::to_string(numeric_value());
    case STRING_LITERAL: return convert_utf16_to_utf8(*string_value());
    case REG_EXP_LITERAL: return convert_utf16_to_utf8(*string_value());
    case IDENTIFIER: return convert_utf16_to_utf8(*string_value());
    default: return m_value.c_str;
    }
  }

  // std::string to_u16string() const
  // {
  //   switch (m_type & CATEGORY_MASK) {
  //   case NUMERIC_LITERAL: return std::to_string(numeric_value());
  //   case STRING_LITERAL: return convert_utf16_to_utf8(*string_value());
  //   case REG_EXP_LITERAL: return convert_utf16_to_utf8(*string_value());
  //   case IDENTIFIER: return convert_utf16_to_utf8(*string_value());
  //   default: return m_value.c_str;
  //   }
  // }

  Number to_number() const
  {
    return { numeric_value() };
  }

  // constexpr String to_string() const
  // {
  //   return { string_value() };
  // }
};

std::ostream& operator<<(std::ostream& out, const Token& token);

#endif
#ifndef ECMASCRIPT_TOKEN_H
#define ECMASCRIPT_TOKEN_H

#include "ast.h"

#include <algorithm>
#include <memory>
#include <string>

#include <boost/optional.hpp>
// #include <boost/variant.hpp>

#include <mpark/variant.hpp>

class Token {

  template <typename... Ts>
  using variant = mpark::variant<Ts...>;

  struct Empty {
  };
  struct Keyword {
    std::string value;
  };
  struct Punctuator {
    std::string value;
  };

  struct Identifier {
    std::u16string value;
  };
  struct NullLiteral {
  };
  struct BooleanLiteral {
    bool value;
  };
  struct NumericLiteral {
    double value;
  };
  struct StringLiteral {
    std::u16string value;
  };
  struct RegularExpressionLiteral {
    std::u16string value;
  };

  using Value = variant<Empty, Identifier, Keyword, Punctuator, NullLiteral,
                        BooleanLiteral, NumericLiteral, StringLiteral,
                        RegularExpressionLiteral>;

  Value value;

  static Value value_from_string(const std::string&);

  Token(Value value) : value(value) {}

  struct equal_visitor;
  struct print_visitor;

public:
  bool preceded_by_line_terminator = false;

  struct DebugInfo {
    virtual std::string syntax_error_at() const = 0;
    virtual std::string loc() const             = 0;
  };

  std::shared_ptr<DebugInfo> debug_info;

  Token()              = default;
  Token(const Token&)  = default;
  Token(Token&& other) = default;
  Token& operator=(const Token&) = default;
  Token& operator=(Token&&) = default;
  Token(const char* str);
  Token(const std::string& str);

  template <typename It>
  Token(It f, It l)
  // Determines the type of the range [f, l). Passing in anything but a valid
  // Identifier, Keyword, FutureReservedKeyword, Punctuator, NullLiteral or
  // BooleanLiteral is undefined behaviour.
  {
    if (std::any_of(f, l, [](auto cp) { return cp >= 128; })) {
      value = Identifier{std::u16string{f, l}};
    }
    else {
      value = value_from_string({f, l});
    }
  }

  static Token identifier(std::string value);
  static Token identifier(std::u16string value);
  static Token keyword(const std::string& value);
  static Token punctuator(const std::string& value);
  static Token null_literal();
  static Token boolean_literal(bool value);
  static Token numeric_literal(double value);
  static Token string_literal(std::u16string value);
  static Token regular_expression_literal();

  bool is_empty() const;
  bool is_identifier() const;
  bool is_keyword() const;
  bool is_future_reserved_word() const;
  bool is_punctuator() const;
  bool is_null_literal() const;
  bool is_boolean_literal() const;
  bool is_numeric_literal() const;
  bool is_string_literal() const;
  bool is_regular_expression_literal() const;

  bool is_identifier_name() const;

  bool any_of_impl(const std::initializer_list<Token>& tokens) const
  {
    return std::find(tokens.begin(), tokens.end(), *this) != tokens.end();
  }

  template <typename... Args>
  bool any_of(Args&&... args) const
  {
    return any_of_impl(
        std::initializer_list<Token>{std::forward<Args>(args)...});
  }

  boost::optional<const std::u16string&> to_identifier() const;
  boost::optional<const std::string&>    to_keyword() const;
  boost::optional<const std::string&>    to_punctuator() const;
  boost::optional<const bool&>           to_boolean_literal() const;
  boost::optional<const double&>         to_numeric_literal() const;
  boost::optional<const std::u16string&> to_string_literal() const;

  bool operator==(const Token& other) const;
  // bool operator!=(const Token& other) const { return value != other.value; }
  void print(std::ostream&) const;

  operator bool() const;
  operator double() const;
  operator std::string() const;
  operator std::u16string() const;
};

std::ostream& operator<<(std::ostream& out, const Token& token);

#endif
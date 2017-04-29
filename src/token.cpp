#include "token.h"
#include "lexer.h"

#include <algorithm>
#include <cmath>
#include <string>
#include <vector>

using namespace std::string_literals;

Token::Token(const char* str)
{
  if (::is_keyword(str)) value = Keyword { str };
  else if (::is_punctuator(str)) value = Punctuator { str };
  else if ("null"s == str) value = NullLiteral {};
  else if ("true"s == str) value = BooleanLiteral { true };
  else if ("false"s == str) value = BooleanLiteral { false };
}

Token::Value Token::value_from_string(const std::string& str)
{
  if (::is_keyword(str))         return Keyword { str };
  else if (::is_punctuator(str)) return Punctuator { str };
  else if (str == "null")        return NullLiteral {};
  else if (str == "true")        return BooleanLiteral { true };
  else if (str == "false")       return BooleanLiteral { false };
  else return Identifier { std::u16string{str.begin(), str.end()} };
}

Token Token::identifier(std::string value)
{
  return { Value { Identifier { std::u16string(value.begin(), value.end() ) } } };
}


Token Token::identifier(std::u16string value)
{
  return { Value { Identifier { std::move(value) } } };
}

Token Token::keyword(const std::string& value)
{
  return { Keyword { value } };
}

Token Token::punctuator(const std::string& value)
{
  return { Punctuator{ value } };
}

Token Token::null_literal()
{
  return { NullLiteral{} };
}

Token Token::boolean_literal(bool value)
{
  return { BooleanLiteral{value} };
}

Token Token::numeric_literal(double value)
{
  return { NumericLiteral{value} };
}

Token Token::string_literal(std::u16string value)
{
  return { StringLiteral{value} };
}

Token Token::regular_expression_literal()
{
  return { RegularExpressionLiteral{} };
}

bool Token::is_empty() const
{
  return boost::get<Empty>(&value) != nullptr;
}

bool Token::is_identifier() const
{
  return boost::get<Identifier>(&value) != nullptr;
}

bool Token::is_keyword() const
{
  return boost::get<Keyword>(&value) != nullptr;
}

bool Token::is_future_reserved_word() const
{
  return false;
}

bool Token::is_punctuator() const
{
  return boost::get<Punctuator>(&value) != nullptr;
}

bool Token::is_null_literal() const
{
  return boost::get<NullLiteral>(&value) != nullptr;
}

bool Token::is_boolean_literal() const
{
  return boost::get<BooleanLiteral>(&value) != nullptr;
}

bool Token::is_numeric_literal() const
{
  return boost::get<NumericLiteral>(&value) != nullptr;
}

bool Token::is_string_literal() const
{
  return boost::get<StringLiteral>(&value) != nullptr;
}

bool Token::is_regular_expression_literal() const
{
  return boost::get<RegularExpressionLiteral>(&value) != nullptr;
}

bool Token::is_identifier_name() const
{
  return is_identifier() || is_keyword() || is_future_reserved_word() ||
    is_null_literal() || is_boolean_literal();
}

boost::optional<const std::u16string&> Token::to_identifier() const
{
  if (auto o = boost::get<Identifier>(&value)) return o->value;
  else return {};
}

boost::optional<const std::string&> Token::to_keyword() const
{
  if (auto o = boost::get<Keyword>(&value)) return o->value;
  else return {};
}

boost::optional<const std::string&> Token::to_punctuator() const
{
  if (auto o = boost::get<Punctuator>(&value)) return o->value;
  else return {};
}

boost::optional<const bool&> Token::to_boolean_literal() const
{
  if (auto o = boost::get<BooleanLiteral>(&value)) return o->value;
  else return {};
}

boost::optional<const double&> Token::to_numeric_literal() const
{
  if (auto o = boost::get<NumericLiteral>(&value)) return o->value;
  else return {};
}

boost::optional<const std::u16string&> Token::to_string_literal() const
{
  if (auto o = boost::get<StringLiteral>(&value)) return o->value;
  else return {};
}

struct Token::print_visitor : boost::static_visitor<void>
{

  std::ostream& out;

  print_visitor(std::ostream& out) : out(out) {}

  void operator()(const Empty&) const { out << "EmptyToken"; }
  void operator()(const Identifier& identifier) const
  {
    out << std::string { identifier.value.begin(), identifier.value.end() };
  }
  void operator()(const Keyword& keyword) const { out << keyword.value; }
  void operator()(const Punctuator& punctuator) const { out << punctuator.value; }
  void operator()(const NullLiteral&) const { out << "null"; }
  void operator()(const BooleanLiteral& literal) const { out << (literal.value ? "true" : "false"); }
  void operator()(const NumericLiteral& literal) const { out << literal.value; }
  void operator()(const StringLiteral& literal) const
  {
    out << std::string { literal.value.begin(), literal.value.end() };
  }
  void operator()(const RegularExpressionLiteral& literal) const {  }


};

void Token::print(std::ostream& out) const
{
  boost::apply_visitor(print_visitor(out), value);
}

struct Token::equal_visitor : boost::static_visitor<bool>
{

  template <typename T>
  auto operator()(const T& lhs, const T& rhs) const
    -> decltype(lhs.value == rhs.value, bool())
  {
    return lhs.value == rhs.value;
  }

  bool operator()(const Empty&, const Empty&) const
  {
    return true;
  }

  bool operator()(const NullLiteral&, const NullLiteral&) const
  {
    return true;
  }

  template <typename T, typename U>
  bool operator()(const T&, const U&) const { return false; }
};

bool Token::operator==(const Token& other) const
{
  return boost::apply_visitor(equal_visitor(), value, other.value);
}

std::ostream& operator<<(std::ostream& out, const Token& token)
{
  token.print(out);
  return out;
}

Token::operator std::string() const
{
  if (auto o = to_keyword())    return *o;
  if (auto o = to_punctuator()) return *o;
  return {};
}

Token::operator std::u16string() const
{
  if (auto o = to_identifier())    return *o;
  if (auto o = to_string_literal()) return *o;
  return {};
}

Token::operator bool() const
{
  if (auto o = to_boolean_literal()) return *o;
  return false;
}

Token::operator double() const
{
  if (auto o = to_numeric_literal()) return *o;
  return NAN;
}
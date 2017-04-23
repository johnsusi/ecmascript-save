#include "types.h"
#include "lexer.h"

#include <limits>

// 9.1
Type ToPrimitive(Type input, boost::optional<Type> preferred_type)
{
  struct detail_t : boost::static_visitor<Type> {
    Type operator()(const Undefined& value) const { return value; }
    Type operator()(const Null& value) const { return value; }
    Type operator()(const Boolean& value) const { return value; }
    Type operator()(const String& value) const { return value; }
    Type operator()(const Number& value) const { return value; }
    Type operator()(const Object& value) const
    {
      // TODO
      return Undefined {};
    }
  } detail;
  return boost::apply_visitor(detail, input);
}

// 9.2
Boolean ToBoolean(Type input)
{
 struct detail_t : boost::static_visitor<Boolean> {
    Boolean operator()(const Undefined& value) const { return { false }; }
    Boolean operator()(const Null& value) const { return { false }; }
    Boolean operator()(const Boolean& value) const { return value; }
    Boolean operator()(const String& value) const { return { value.value.size() > 0 }; }
    Boolean operator()(const Number& value) const
    {
      switch (std::fpclassify(value.value))
      {
        case FP_NAN: return { false };
        case FP_ZERO: return { false };
        default: return { true };
      }
    }
    Boolean operator()(const Object& value) const { return { true }; }
  } detail;
  return boost::apply_visitor(detail, input);
}

// 9.3
Number ToNumber(Type input)
{
 struct detail_t : boost::static_visitor<Number> {
    Number operator()(const Undefined& value) const { return { std::nan("") }; }
    Number operator()(const Null& value) const { return { 0 }; }
    Number operator()(const Boolean& value) const { return value.value ? Number { 1.0 } : Number { 0 }; }
    Number operator()(const String& value) const
    {
      // TODO
      return {};
    }
    Number operator()(const Number& value) const { return value; }
    Number operator()(const Object& value) const { return { true }; }
  } detail;
  return boost::apply_visitor(detail, input);
}

Number ToInteger(Type input)
{
  // 9.4.1
  Number number = ToNumber(input);
  if (number.isNaN()) return { 0 };
  if (number.isZero() || number.isInfinity()) return number;
}

struct StringConverter : Lexer
{

  int sign = +1;

  double value;

  int decimal_value = 0;
  int decimal_count = 0;

  bool string_white_space()
  {
    while (str_white_space());
    str_numeric_literal();
    while (str_white_space());

  }

  bool str_white_space()
  {
    return white_space() || line_terminator();
  }

  bool str_numeric_literal()
  {
    return str_decimal_literal() || hex_integer_literal();
  }

  bool str_decimal_literal()
  {
    if (match("+")) {
      sign = +1;
    }
    else if (match("-")) {
      sign = -1;
    }
    return str_unsigned_decimal_literal();
  }

  bool str_unsigned_decimal_literal()
  {
    int a = 0, b = 0, c = 1;
    if (match("Infinity")) value = std::numeric_limits<double>::infinity();
    if (decimal_digits()) a = decimal_value;
    if (match(".")) {
      if (decimal_digits()) {
        b = decimal_value;
        c = -decimal_count;
      }
    }
    if ()


  }

  bool decimal_digits()
  {
    decimal_value = 0;
    decimal_count = 0;
    if (decimal_digit()) {
      do {
        int d = static_cast<int>(match) - '0';
        decimal_value = decimal_value * 10 + d;
        ++decimal_count;
      }
      while (decimal_digit());
      return true;
    }
    return false;
  };

};
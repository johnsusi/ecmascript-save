#include "type.h"

#include <cmath>
#include <string>

#include "dtoa.h"

Type::Value::Value() : tag(Tag::UNDEFINED_VALUE)
{
}

Type::Value::Value(Undefined value)
    : tag(Tag::UNDEFINED_VALUE), undefined_value(value)
{
}

Type::Value::Value(Null value) : tag(Tag::NULL_VALUE), null_value(value)
{
}

Type::Value::Value(Boolean value)
    : tag(Tag::BOOLEAN_VALUE), boolean_value(value)
{
}

Type::Value::Value(Number value) : tag(Tag::NUMBER_VALUE), number_value(value)
{
}

Type::Value::Value(String value) : tag(Tag::STRING_VALUE), string_value(value)
{
}

Type::Value::Value(Object value) : tag(Tag::OBJECT_VALUE), object_value(value)
{
}

Type::Value::Value(const Type::Value& other)
{
  switch ((tag = other.tag)) {
  case Tag::UNDEFINED_VALUE: undefined_value = other.undefined_value; break;
  case Tag::NULL_VALUE: null_value           = other.null_value; break;
  case Tag::BOOLEAN_VALUE: boolean_value     = other.boolean_value; break;
  case Tag::NUMBER_VALUE: number_value       = other.number_value; break;
  case Tag::STRING_VALUE: string_value       = other.string_value; break;
  case Tag::OBJECT_VALUE: object_value       = other.object_value; break;
  }
}

Type::Value& Type::Value::operator=(const Type::Value& other)
{
  switch ((tag = other.tag)) {
  case Tag::UNDEFINED_VALUE: undefined_value = other.undefined_value; break;
  case Tag::NULL_VALUE: null_value           = other.null_value; break;
  case Tag::BOOLEAN_VALUE: boolean_value     = other.boolean_value; break;
  case Tag::NUMBER_VALUE: number_value       = other.number_value; break;
  case Tag::STRING_VALUE: string_value       = other.string_value; break;
  case Tag::OBJECT_VALUE: object_value       = other.object_value; break;
  }
  return *this;
}

Type::Type()
{
}

// bool Type::IsUndefined() const
// {
//   return tag == Tag::UNDEFINED;
// }

// bool Type::IsNull() const
// {
//   return tag == Tag::NULL_VALUE;
// }

// bool Type::IsBoolean() const
// {
//   return tag == Tag::BOOLEAN_VALUE;
// }

// bool Type::IsNumber() const
// {
//   return tag == Tag::NUMBER_VALUE;
// }

// bool Type::IsString() const
// {
//   return tag == Tag::STRING_VALUE;
// }

// bool Type::IsObject() const
// {
//   return tag == Tag::OBJECT_VALUE;
// }

// bool Type::IsReference() const
// {
//   return false;
//   // return tag == Tag::REFERENCE_VALUE;
// }

Object TypeError()
{
  return {};
}

Object BooleanObject(const Boolean& value)
{
  return {};
}

Object NumberObject(const Number& value)
{
  return {};
}

Object StringObject(const String& value)
{
  return {};
}

// 9.1 ToPrimitive
Type ToPrimitive(const Type& input, const char* PreferredType)
{
  switch (input.value.tag) {
  case Type::Tag::UNDEFINED_VALUE: return input;
  case Type::Tag::NULL_VALUE: return input;
  case Type::Tag::BOOLEAN_VALUE: return input;
  case Type::Tag::NUMBER_VALUE: return input;
  case Type::Tag::STRING_VALUE: return input;
  case Type::Tag::OBJECT_VALUE:
    // return input.value.object_value.DefaultValue(PreferredType);
    return {};
  }
}

// 9.2 ToBoolean
Boolean ToBoolean(const Type& input)
{
  switch (input.value.tag) {
  case Type::Tag::UNDEFINED_VALUE: return Boolean{false};
  case Type::Tag::NULL_VALUE: return Boolean{false};
  case Type::Tag::BOOLEAN_VALUE: return input.value.boolean_value;
  case Type::Tag::NUMBER_VALUE:
    return Boolean{
        !(input.value.number_value.isNaN() || input.value.number_value == 0)};
  case Type::Tag::STRING_VALUE:
    return Boolean{input.value.string_value.length() > 0};
  case Type::Tag::OBJECT_VALUE: return Boolean{true};
  }
}

// 9.3 ToNumber
Number ToNumber(const Type& input)
{
  switch (input.value.tag) {
  case Type::Tag::UNDEFINED_VALUE: return Number::NaN();
  case Type::Tag::NULL_VALUE: return 0;
  case Type::Tag::BOOLEAN_VALUE: return input.value.boolean_value ? 1 : 0;
  case Type::Tag::NUMBER_VALUE: return input.value.number_value;
  case Type::Tag::STRING_VALUE: // TODO
    return Number::NaN();
  case Type::Tag::OBJECT_VALUE:
    auto primValue = ToPrimitive(input, "Number");
    return ToNumber(primValue);
  }
}

// 9.4 ToInteger
Number ToInteger(const Type& input)
{
  auto number = ToNumber(input);
  if (number.isNaN()) return 0;
  if (number.isZero() || number.isInfinity()) return number;
  return number.sign() * Number(number.abs()).floor();
}

// 9.8 ToString
String ToString(const Type& input)
{
  switch (input.value.tag) {
  case Type::Tag::UNDEFINED_VALUE: return u"undefined";
  case Type::Tag::NULL_VALUE: return u"null";
  case Type::Tag::BOOLEAN_VALUE:
    return input.value.number_value ? u"true" : u"false";
  case Type::Tag::NUMBER_VALUE: return input.value.number_value.ToString();
  case Type::Tag::STRING_VALUE: return input.value.string_value;
  case Type::Tag::OBJECT_VALUE:
    auto primValue = ToPrimitive(input, "String");
    return ToString(primValue);
  }
}

// 9.9 ToObject
Object ToObject(const Type& input)
{
  switch (input.value.tag) {
  case Type::Tag::UNDEFINED_VALUE: return TypeError();
  case Type::Tag::NULL_VALUE: return TypeError();
  case Type::Tag::BOOLEAN_VALUE:
    return BooleanObject(input.value.boolean_value);
  case Type::Tag::NUMBER_VALUE: return NumberObject(input.value.number_value);
  case Type::Tag::STRING_VALUE: return StringObject(input.value.string_value);
  case Type::Tag::OBJECT_VALUE: return input.value.object_value;
  }
}

// 9.10 CheckObjectCoercible
Type CheckObjectCoercible(const Type& input)
{
  switch (input.value.tag) {
  case Type::Tag::UNDEFINED_VALUE: return TypeError();
  case Type::Tag::NULL_VALUE: return TypeError();
  case Type::Tag::BOOLEAN_VALUE: return {};
  case Type::Tag::NUMBER_VALUE: return {};
  case Type::Tag::STRING_VALUE: return {};
  case Type::Tag::OBJECT_VALUE: return {};
  }
}

// 9.11 IsCallable
bool IsCallable(const Type& x)
{
  switch (x.value.tag) {
  case Type::Tag::UNDEFINED_VALUE: return false;
  case Type::Tag::NULL_VALUE: return false;
  case Type::Tag::BOOLEAN_VALUE: return false;
  case Type::Tag::NUMBER_VALUE: return false;
  case Type::Tag::STRING_VALUE: return false;
  case Type::Tag::OBJECT_VALUE: return x.value.object_value["[[Call]]"];
  }
}

// 9.12 The SameValue Algorithm
bool SameValue(const Type& x, const Type& y)
// The internal comparison abstract operation SameValue(x, y), where x and y are
// ECMAScript language values, produces true or false. Such a comparison is
// performed as follows:
{

  // 1. If Type(x) is different from Type(y), return false.
  if (x.value.tag != y.value.tag) return false;

  switch (x.value.tag) {

  // 2. If Type(x) is Undefined, return true.
  case Type::Tag::UNDEFINED_VALUE:
    return true;

  // 3. If Type(x) is Null, return true.
  case Type::Tag::NULL_VALUE:
    return true;

  // 4. If Type(x) is Number, then.
  case Type::Tag::NUMBER_VALUE:

    // a. If x is NaN and y is NaN, return true.
    if (x.value.number_value.isNaN() && y.value.number_value.isNaN())
      return true;

    // b. If x is +0 and y is -0, return false.
    // c. If x is -0 and y is +0, return false.
    // d. If x is the same Number value as y, return true.
    if (x.value.number_value == y.value.number_value)
      return x.value.number_value.sign() == y.value.number_value.sign();

    // e. Return false.
    return false;

  // 5. If Type(x) is String, then return true if x and y are exactly the same
  // sequence of characters (same length and same characters in corresponding
  // positions); otherwise, return false.
  case Type::Tag::STRING_VALUE:
    return x.value.string_value == y.value.string_value;

  // 6. If Type(x) is Boolean, return true if x and y are both true or both
  // false; otherwise, return false.
  case Type::Tag::BOOLEAN_VALUE:
    return x.value.boolean_value == y.value.boolean_value;

  // 7. Return true if x and y refer to the same object. Otherwise, return
  // false.
  case Type::Tag::OBJECT_VALUE:
    return false; // TODO
  }
}

// 11.9.3 The Abstract Equality Comparison Algorithm
bool AbstractEqualityComparison(const Type& x, const Type& y)
// The comparison x == y, where x and y are values, produces true or false. Such
// a comparison is performed as follows:
{
  // 1. If Type(x) is the same as Type(y), then
  if (x.value.tag == y.value.tag) {
    switch (x.value.tag) {
    // a. If Type(x) is Undefined, return true.
    case Type::Tag::UNDEFINED_VALUE:
      return true;
    // b. If Type(x) is Null, return true.
    case Type::Tag::NULL_VALUE:
      return true;
    // c. If Type(x) is Number, then
    case Type::Tag::NUMBER_VALUE:
      // i. If x is NaN, return false.
      if (x.value.number_value.isNaN()) return false;
      //  ii. If y is NaN, return false.
      if (y.value.number_value.isNaN()) return false;
      // iii. If x is the same Number value as y, return true.
      // iv. If x is +0 and y is  0, return true.
      // v. If x is  0 and y is +0, return true.
      if (x.value.number_value == y.value.number_value) return true;
      // vi. Return false.
      return false;

    // d. If Type(x) is String, then return true if x and y are exactly the same
    // sequence of characters (same length and same characters in corresponding
    // positions). Otherwise, return false.
    case Type::Tag::STRING_VALUE:
      return x.value.string_value == y.value.string_value;
    // e. If Type(x) is Boolean, return true if x and y are both true or both
    // false. Otherwise, return false.
    case Type::Tag::BOOLEAN_VALUE:
      return x.value.boolean_value == y.value.boolean_value;

    // f. Return true if x and y refer to the same object. Otherwise, return
    // false.
    case Type::Tag::OBJECT_VALUE:
      return false; // TODO
    }
  }
  // 2. Ifx is null and y is undefined, return true.
  if (x.value.tag == Type::Tag::NULL_VALUE
      && y.value.tag == Type::Tag::UNDEFINED_VALUE)
    return true;
  // 3. Ifx is undefined and y is null, return true.
  if (x.value.tag == Type::Tag::UNDEFINED_VALUE
      && y.value.tag == Type::Tag::NULL_VALUE)
    return true;

  // 4. If Type(x) is Number and Type(y) is String, return the result of the
  // comparison x == ToNumber(y).
  if (x.value.tag == Type::Tag::NUMBER_VALUE
      && y.value.tag == Type::Tag::STRING_VALUE)
    return AbstractEqualityComparison(x, ToNumber(y));

  // 5. If Type(x) is String and Type(y) is Number, return the result of the
  // comparison ToNumber(x) == y.
  if (x.value.tag == Type::Tag::STRING_VALUE
      && y.value.tag == Type::Tag::NUMBER_VALUE)
    return AbstractEqualityComparison(ToNumber(x), y);

  // 6. If Type(x) is Boolean, return the result of the comparison ToNumber(x)
  // == y.
  if (x.value.tag == Type::Tag::BOOLEAN_VALUE)
    return AbstractEqualityComparison(ToNumber(x), y);

  // 7. If Type(y) is Boolean, return the result of the comparison x ==
  // ToNumber(y).
  if (y.value.tag == Type::Tag::BOOLEAN_VALUE)
    return AbstractEqualityComparison(x, ToNumber(y));

  // 8. If Type(x) is either String or Number and Type(y) is Object, return the
  // result of the comparison x == ToPrimitive(y).
  if ((x.value.tag == Type::Tag::STRING_VALUE
       || x.value.tag == Type::Tag::NUMBER_VALUE)
      && y.value.tag == Type::Tag::OBJECT_VALUE)
    return AbstractEqualityComparison(x, ToPrimitive(y));

  // 9. If Type(x) is Object and Type(y) is either String or Number, return the
  // result of the comparison ToPrimitive(x) == y.
  if (x.value.tag == Type::Tag::OBJECT_VALUE
      && (y.value.tag == Type::Tag::STRING_VALUE
          || y.value.tag == Type::Tag::NUMBER_VALUE))
    return AbstractEqualityComparison(ToPrimitive(x), y);
  // 10. Return false.
  return false;

  // NOTE 1: Given the above definition of equality:
  //   - String comparison can be forced by: "" + a == "" + b.
  //   - Numeric comparison can be forced by: +a == +b.
  //   - Boolean comparison can be forced by: !a == !b.

  // NOTE 2: The equality operators maintain the following invariants:
  //   - A != B is equivalent to !(A==B).
  //   - A == B is equivalent to B==A, except in the order of evaluation of A
  //   and B.

  // NOTE 3: The equality operator is not always transitive. For example, there
  // might be two distinct String objects, each representing the same String
  // value; each String object would be considered equal to the String value by
  // the == operator, but the two String objects would not be equal to each
  // other. For Example:
  //   - new String("a") == "a" and "a" == new String("a") are both true.
  //   - new String("a") == new String("a") is false.

  // NOTE 4 Comparison of Strings uses a simple equality test on sequences of
  // code unit values. There is no attempt to use the more complex, semantically
  // oriented definitions of character or string equality and collating order
  // defined in the Unicode specification. Therefore Strings values that are
  // canonically equal according to the Unicode standard could test as unequal.
  // In effect this algorithm assumes that both Strings are already in
  // normalised form.
}

bool StrictEqualityComparison(const Type& x, const Type& y)
// The comparison x === y, where x and y are values, produces true or false.
// Such a comparison is performed as follows:
{
  // 1. If Type(x) is different from Type(y), return false.
  if (x.value.tag != y.value.tag) return false;
  switch (x.value.tag) {
  // 2. If Type(x) is Undefined, return true.
  case Type::Tag::UNDEFINED_VALUE:
    return true;
  // 3. If Type(x) is Null, return true.
  case Type::Tag::NULL_VALUE:
    return true;
  // 4. If Type(x) is Number, then
  case Type::Tag::NUMBER_VALUE:
    // a. If x is NaN, return false.
    if (x.value.number_value.isNaN()) return false;
    // b. If y is NaN, return false.
    if (y.value.number_value.isNaN()) return false;
    // c. If x is the same Number value as y, return true.
    // d. If x is +0 and y is -0, return true.
    // e. If x is - 0 and y is +0, return true.
    if (x.value.number_value == y.value.number_value) return true;
    // f. Return false.
    return false;
  // 5. If Type(x) is String, then return true if x and y are exactly the same
  // sequence of characters(same length and same characters in corresponding
  // positions); otherwise, return false.
  case Type::Tag::STRING_VALUE:
    return x.value.string_value == y.value.string_value;
  // 6. If Type(x) is Boolean, return true if x and y are both true or both
  // false; otherwise, return false.
  case Type::Tag::BOOLEAN_VALUE:
    return x.value.boolean_value == y.value.boolean_value;
  // 7. Return true if x and y refer to the same object.Otherwise, return false.
  case Type::Tag::OBJECT_VALUE: return false;
  }

  // NOTE This algorithm differs from the SameValue Algorithm(9.12) in its
  // treatment of signed zeroes and NaNs.
}

bool operator==(const Type& x, const Type& y)
{
  return StrictEqualityComparison(x, y);
}

bool operator!=(const Type& x, const Type& y)
{
  return !(x == y);
}

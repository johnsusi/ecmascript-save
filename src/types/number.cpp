#include "number.h"

#include <cmath>

#include "../dtoa.h"

Number Number::NaN()
{
  return std::nan("");
}

Number::Number() : value(0.0)
{
}

Number::Number(double value) : value(value)
{
}

double Number::abs() const
{
  return value < 0 ? -value : value;
}

double Number::floor() const
{
  return std::floor(value);
}

double Number::sign() const
{
  return value < 0 ? -1 : 1;
}

bool Number::isNaN() const
{
  return std::isnan(value);
}

bool Number::isInfinity() const
{
  return std::isinf(value);
}

bool Number::isZero() const
{
  return std::fpclassify(value) == FP_ZERO;
}

Number::operator double() const
{
  return value;
}

bool Number::operator==(const Number& other) const
{
  return value == other.value;
}

bool Number::operator!=(const Number& other) const
{
  return value != other.value;
}

bool Number::operator<=(const Number& other) const
{
  return value <= other.value;
}

bool Number::operator>=(const Number& other) const
{
  return value >= other.value;
}

bool Number::operator<(const Number& other) const
{
  return value < other.value;
}

bool Number::operator>(const Number& other) const
{
  return value > other.value;
}

// static int abs(int x)
// {
//   return x < 0 ? -x : x;
// }

// 9.8.1 ToString Applied to the Number Type
std::string Number::ToString() const
// The abstract operation ToString converts a Number m to String format as
// follows:
{

  using namespace std;

  auto abs = [](int x) { return x < 0 ? -x : x; };

  double m = value;

  // 1. If m is NaN, return the String "NaN".
  if (std::isnan(m)) return "NaN";

  // 2. If m is +0 or -0, return the String "0".
  if (m == 0) return "0";

  // 3. If m is less than zero, return the String concatenation of the String
  // "-" and ToString(-m).
  if (m < 0) return "-" + Number(-m).ToString();

  // 4. If m is infinity, return the String "Infinity".
  if (std::isinf(m)) return "Infinity";

  // 5. Otherwise, let n, k, and s be integers such that k >= 1, 10^(k-1) <= s <
  // 10^k, the Number value for s x 10^(n-k) is m, and
  // k is as small as possible. Note that k is the number of digits in the
  // decimal representation of s, that s is not divisible by 10, and that the
  // least significant digit of s is not necessarily uniquely determined by
  // these criteria.

  int   n, sign;
  char* s1;
  char* s0 = dtoa(m, 0, 0, &n, &sign, &s1);
  int   k  = s1 - s0;

  string ss(s0);
  freedtoa(s0);
  auto s = ss.data();

  // 6. If k <= n <= 21, return the String consisting of the k digits of the
  // decimal
  // representation of s (in order, with no leading zeroes), followed by n k
  // occurrences of the character  '0'.
  if (k <= n && n <= 21) return s + string(n - k, '0');

  // 7. If 0 < n <= 21, return the String consisting of the most significant n
  // digits
  // of the decimal representation of s, followed by a decimal point  '.',
  // followed by
  // the remaining k-n digits of the decimal representation of s.
  if (0 < n && n <= 21) return string{s, s + n} + "." + string{s + n, s + k};

  // 8. If -6 < n <= 0, return the String consisting of the character '0',
  // followed by
  // a decimal point '.', followed by -n occurrences of the character '0',
  // followed
  // by the k digits of the decimal representation of s.
  if (-6 < n && n <= 0) return "0." + string(-n, '0') + string{s, s + k};

  // 9. Otherwise, if k = 1, return the String consisting of the single digit of
  // s,
  // followed by lowercase character 'e', followed by a plus sign '+' or minus
  // sign '-'
  // according to whether n 1 is positive or negative, followed by the decimal
  // representation of the integer abs(n-1) (with no leading zeroes).
  if (k == 1)
    return string{s, s + 1} + "e" + (n > 0 ? "+" : "-") + to_string(abs(n - 1));

  //  10. Return the String consisting of the most significant digit of the
  //  decimal
  // representation of s, followed by a decimal point '.', followed by the
  // remaining
  // k-1 digits of the decimal representation of s, followed by the lowercase
  // character 'e', followed by a plus sign '+' or minus sign '-' according to
  // whether n-1 is positive or negative, followed by the decimal representation
  // of
  // the integer abs(n-1) (with no leading zeroes).

  return string{s, s + 1} + "." + string{s + 1, s + k} + "e"
         + (n - 1 > 0 ? "+" : "-") + to_string(abs(n - 1));
}

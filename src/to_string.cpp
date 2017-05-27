#include "to_string.h"

#include <cmath>
#include <cstring>
#include <iostream>

#include "dtoa.h"

std::string ToString(double m) {
  //   9.8.1 ToString Applied to the Number Type
  // The abstract operation ToString converts a Number m to String format as
  // follows:

  // 1. If m is NaN, return the String "NaN".
  if (std::isnan(m))
    return "NaN";

  // 2. If m is +0 or  0, return the String "0".
  if (m == 0)
    return std::signbit(m) ? "-0" : "0";

  // 3. If m is less than zero, return the String concatenation of the String
  // "-" and ToString(-m).
  if (std::isless(m, 0.0))
    return "-" + ToString(-m);

  // 4. If m is infinity, return the String "Infinity".
  if (std::isinf(m))
    return "Infinity";

  // 5. Otherwise, let n, k, and s be integers such that k >= 1, 10^(k-1) <= s <
  // 10^k, the Number value for s x 10^(n-k) is m, and
  // k is as small as possible. Note that k is the number of digits in the
  // decimal representation of s, that s is not divisible by 10, and that the
  // least significant digit of s is not necessarily uniquely determined by
  // these criteria.

  int n, sign;
  char *s1;
  char *s0 = dtoa(m, 0, 0, &n, &sign, &s1);
  int k = s1 - s0;

  std::string ss(s0);
  freedtoa(s0);
  auto s = ss.data();

  // 6. If k <= n <= 21, return the String consisting of the k digits of the
  // decimal
  // representation of s (in order, with no leading zeroes), followed by n k
  // occurrences of the character  '0'.
  if (k <= n && n <= 21)
    return s + std::string(n - k, '0');

  // 7. If 0 < n <= 21, return the String consisting of the most significant n
  // digits
  // of the decimal representation of s, followed by a decimal point  '.',
  // followed by
  // the remaining k-n digits of the decimal representation of s.
  if (0 < n && n <= 21)
    return std::string{s, s + n} + "." + std::string{s + n, s + k};

  // 8. If -6 < n <= 0, return the String consisting of the character '0',
  // followed by
  // a decimal point '.', followed by -n occurrences of the character '0',
  // followed
  // by the k digits of the decimal representation of s.
  if (-6 < n && n <= 0)
    return "0." + std::string(-n, '0') + std::string{s, s + k};

  // 9. Otherwise, if k = 1, return the String consisting of the single digit of
  // s,
  // followed by lowercase character 'e', followed by a plus sign '+' or minus
  // sign '-'
  // according to whether n 1 is positive or negative, followed by the decimal
  // representation of the integer abs(n-1) (with no leading zeroes).
  if (k == 1)
    return std::string{s, s + 1} + "e" + (n > 0 ? "+" : "-") +
           std::to_string(std::abs(n - 1));

  //  10. Return the String consisting of the most significant digit of the
  //  decimal
  // representation of s, followed by a decimal point '.', followed by the
  // remaining
  // k-1 digits of the decimal representation of s, followed by the lowercase
  // character 'e', followed by a plus sign '+' or minus sign '-' according to
  // whether n-1 is positive or negative, followed by the decimal representation
  // of
  // the integer abs(n-1) (with no leading zeroes).

  return std::string{s, s + 1} + "." + std::string{s + 1, s + k} + "e" +
         (n - 1 > 0 ? "+" : "-") + std::to_string(std::abs(n - 1));
}
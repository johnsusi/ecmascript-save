#ifndef ECMASCRIPT_TYPES_NUMBER_H
#define ECMASCRIPT_TYPES_NUMBER_H

#include <string>

class Number {

  double value;

public:
  static Number NaN();

  Number();
  Number(double);

  double abs() const;
  double floor() const;
  double sign() const;

  bool isNaN() const;
  bool isInfinity() const;
  bool isZero() const;

  operator double() const;

  bool operator==(const Number&) const;
  bool operator!=(const Number&) const;
  bool operator<=(const Number&) const;
  bool operator>=(const Number&) const;
  bool operator<(const Number&) const;
  bool operator>(const Number&) const;

  template <typename T>
  bool operator==(const T& v) const
  {
    return value == v;
  }

  std::string ToString() const;
};

#endif
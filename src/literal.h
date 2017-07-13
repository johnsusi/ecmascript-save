#ifndef ECMASCRIPT_LITERAL_H
#define ECMASCRIPT_LITERAL_H

struct Literal {

  virtual void
};

struct NullLiteral {};
struct BooleanLiteral {
  boolean value;
};

struct NumericLiteral {
  double value;
};

struct StringLiteral {
  std::u16string* value;
};


#endif

#ifndef ECMASCRIPT_INPUT_ELEMENT_H
#define ECMASCRIPT_INPUT_ELEMENT_H

#include "optional.h"
#include "token.h"
#include "variant.hpp"

#include <algorithm>
#include <iterator>
#include <ostream>
#include <string>

bool is_line_terminator(int);

class InputElement {
  struct Empty {
  };
  struct Token {
    ::Token value;
  };
  struct LineTerminator {
  };
  struct Comment {
    std::u16string value;
  };
  struct WhiteSpace {
  };

  using Value = mpark::variant<Empty, Token, LineTerminator, Comment, WhiteSpace>;

  Value value;

  InputElement() = delete;
  InputElement(Value&& value) : value(value)
  {
  }

public:
  static InputElement empty();
  static InputElement white_space();
  static InputElement line_terminator();
  static InputElement comment(std::u16string value);
  static InputElement token(::Token value);

  bool is_empty() const;
  // bool is_token() const;
  bool is_line_terminator() const;
  // bool is_comment() const;
  bool is_white_space() const;
  bool has_line_terminator() const;

  explicit operator bool() const;

  optional<::Token> to_token() const;
};

#endif
#ifndef ECMASCRIPT_TOKEN_H
#define ECMASCRIPT_TOKEN_H

#include "util.h"

#include <algorithm>
#include <cstddef>
#include <iostream>
#include <memory>
#include <string>
#include <utility>
#include <vector>

std::u16string &create_static_string(std::u16string value);

class Token {

  using type_t = std::size_t;

  // Lower 7 bits reserved for type index [0 94).
  static const type_t IDENTIFIER = 0b0000000010000000;
  static const type_t KEYWORD = 0b0000000100000000;
  static const type_t PUNCTUATOR = 0b0000001000000000;
  static const type_t FUTURE_RESERVED_WORD = 0b0000010000000000;
  static const type_t NULL_LITERAL = 0b0000100000000000;
  static const type_t BOOLEAN_LITERAL = 0b0001000000000000;
  static const type_t NUMERIC_LITERAL = 0b0010000000000000;
  static const type_t STRING_LITERAL = 0b0100000000000000;
  static const type_t REGULAR_EXPRESSION_LITERAL = 0b1000000000000000;

  static const type_t RESERVED_WORD =
      KEYWORD | FUTURE_RESERVED_WORD | NULL_LITERAL | BOOLEAN_LITERAL;

  template <typename T1, typename T2>
  static constexpr bool less(const T1 *lhs, const T2 *rhs) {
    for (; *lhs || *rhs; ++lhs, ++rhs)
      if (*lhs != *rhs)
        return *lhs < *rhs;
    return false;
  }

  template <typename T> static constexpr type_t type(const T *str) {
    std::initializer_list<std::pair<const char *, type_t>> tokens{
        {"!", PUNCTUATOR | 1},
        {"!=", PUNCTUATOR | 2},
        {"!==", PUNCTUATOR | 3},
        {"%", PUNCTUATOR | 4},
        {"%=", PUNCTUATOR | 5},
        {"&", PUNCTUATOR | 6},
        {"&&", PUNCTUATOR | 7},
        {"&=", PUNCTUATOR | 8},
        {"(", PUNCTUATOR | 9},
        {")", PUNCTUATOR | 10},
        {"*", PUNCTUATOR | 11},
        {"*=", PUNCTUATOR | 12},
        {"+", PUNCTUATOR | 13},
        {"++", PUNCTUATOR | 14},
        {"+=", PUNCTUATOR | 15},
        {",", PUNCTUATOR | 16},
        {"-", PUNCTUATOR | 17},
        {"--", PUNCTUATOR | 18},
        {"-=", PUNCTUATOR | 19},
        {".", PUNCTUATOR | 20},
        {"/", PUNCTUATOR | 21},
        {"/=", PUNCTUATOR | 22},
        {":", PUNCTUATOR | 23},
        {";", PUNCTUATOR | 24},
        {"<", PUNCTUATOR | 25},
        {"<<", PUNCTUATOR | 26},
        {"<<=", PUNCTUATOR | 27},
        {"<=", PUNCTUATOR | 28},
        {"=", PUNCTUATOR | 29},
        {"==", PUNCTUATOR | 30},
        {"===", PUNCTUATOR | 31},
        {">", PUNCTUATOR | 32},
        {">=", PUNCTUATOR | 33},
        {">>", PUNCTUATOR | 34},
        {">>=", PUNCTUATOR | 35},
        {">>>", PUNCTUATOR | 36},
        {">>>=", PUNCTUATOR | 37},
        {"?", PUNCTUATOR | 38},
        {"[", PUNCTUATOR | 39},
        {"]", PUNCTUATOR | 40},
        {"^", PUNCTUATOR | 41},
        {"^=", PUNCTUATOR | 42},
        {"break", KEYWORD | 43},
        {"case", KEYWORD | 44},
        {"catch", KEYWORD | 45},
        {"class", FUTURE_RESERVED_WORD | 46},
        {"const", FUTURE_RESERVED_WORD | 47},
        {"continue", KEYWORD | 48},
        {"debugger", KEYWORD | 49},
        {"default", KEYWORD | 50},
        {"delete", KEYWORD | 51},
        {"do", KEYWORD | 52},
        {"else", KEYWORD | 53},
        {"enum", FUTURE_RESERVED_WORD | 54},
        {"export", FUTURE_RESERVED_WORD | 55},
        {"extends", FUTURE_RESERVED_WORD | 56},
        {"false", BOOLEAN_LITERAL | 57},
        {"finally", KEYWORD | 58},
        {"for", KEYWORD | 59},
        {"function", KEYWORD | 60},
        {"get", IDENTIFIER | 101},
        {"if", KEYWORD | 61},
        {"implements", FUTURE_RESERVED_WORD | 62},
        {"import", FUTURE_RESERVED_WORD | 63},
        {"in", KEYWORD | 64},
        {"instanceof", KEYWORD | 65},
        {"interface", FUTURE_RESERVED_WORD | 66},
        {"let", FUTURE_RESERVED_WORD | 67},
        {"new", KEYWORD | 68},
        {"null", NULL_LITERAL | 69},
        {"package", FUTURE_RESERVED_WORD | 70},
        {"private", FUTURE_RESERVED_WORD | 71},
        {"protected", FUTURE_RESERVED_WORD | 72},
        {"public", FUTURE_RESERVED_WORD | 73},
        {"return", KEYWORD | 74},
        {"set", IDENTIFIER | 102},
        {"static", FUTURE_RESERVED_WORD | 75},
        {"super", FUTURE_RESERVED_WORD | 76},
        {"switch", KEYWORD | 77},
        {"this", KEYWORD | 78},
        {"throw", KEYWORD | 79},
        {"true", BOOLEAN_LITERAL | 80},
        {"try", KEYWORD | 81},
        {"typeof", KEYWORD | 82},
        {"var", KEYWORD | 83},
        {"void", KEYWORD | 84},
        {"while", KEYWORD | 85},
        {"with", KEYWORD | 86},
        {"yield", FUTURE_RESERVED_WORD | 87},
        {"{", PUNCTUATOR | 88},
        {"|", PUNCTUATOR | 89},
        {"|=", PUNCTUATOR | 90},
        {"||", PUNCTUATOR | 91},
        {"}", PUNCTUATOR | 92},
        {"~", PUNCTUATOR | 93}};

    // Find lower bound. The range is sorted on the first value in each pair.
    auto count = tokens.size();
    auto first = tokens.begin();
    while (count > 0) {
      auto it = first;
      auto step = count / 2;
      it += step;
      if (less(it->first, str)) {
        first = ++it;
        count -= step + 1;
      } else
        count = step;
    }

    if (first == tokens.end() || less(str, first->first))
      return 0;
    return first->second;
  }

  type_t m_type;

  union value_t {
    std::nullptr_t empty;
    double numeric_value;
    std::u16string *string_value;
    constexpr value_t() : empty(nullptr) {}
    constexpr value_t(double value) : numeric_value(value) {}
    constexpr value_t(std::u16string *value) : string_value(value) {}
  } m_value;

  bool m_lt = false;

  template <typename Value>
  constexpr explicit Token(type_t type, Value value)
      : m_type(type), m_value(value) {
    // if (type == 0) throw std::logic_error("");
  }

  constexpr explicit Token(type_t type) : Token(type, nullptr) {}

public:
  static Token identifier(std::u16string value) {
    return Token(IDENTIFIER, &create_static_string(std::move(value)));
  }

  static Token identifier_name(std::u16string value) {
    if (auto t = type(value.data())) {
      return Token(t);
    } else {
      return Token(IDENTIFIER, &create_static_string(std::move(value)));
    }
  }
  static constexpr Token null_literal() { return Token(nullptr); }
  constexpr static Token punctuator(const char *str) { return Token(str); }
  static Token punctuator(const std::u16string &str) { return Token(str); }
  static Token numeric_literal(double value) { return Token(value); }
  static Token string_literal(std::u16string value) {
    return Token(STRING_LITERAL, &create_static_string(std::move(value)));
  }
  static Token regular_expression_literal(std::u16string value) {
    return Token(REGULAR_EXPRESSION_LITERAL,
                 &create_static_string(std::move(value)));
  }
  struct DebugInfo {
    virtual std::string syntax_error_at() const = 0;
    virtual std::string loc() const = 0;
  };

  DebugInfo *debug_info = nullptr;

  Token(const std::u16string &str) : Token(type(str.data())) {}

  constexpr Token() : Token(0, nullptr) {}

  constexpr Token(const char *str) : Token(type(str)) {
    if (m_type == 0)
      throw std::logic_error("Cannot create compile time token");
  }

  constexpr Token(const char16_t *str) : Token(type(str)) {}

  constexpr Token(std::nullptr_t) : Token(type("null")) {}

  constexpr explicit Token(bool value)
      : Token(value ? type("true") : type("false")) {}

  constexpr Token(double value) : Token(NUMERIC_LITERAL, value) {}

  constexpr Token(std::u16string *value) : Token(STRING_LITERAL, value) {}

  constexpr Token(const Token &) = default;

  constexpr Token(Token &&) = default;

  ~Token() = default;

  constexpr Token &operator=(const Token &) = default;

  constexpr Token &operator=(Token &&) = default;

  constexpr type_t type() const { return m_type; }

  constexpr operator type_t() const { return m_type; }

  constexpr bool is_identifier() const {
    return (m_type & IDENTIFIER) == IDENTIFIER;
  }

  constexpr bool is_identifier_name() const {
    return is_identifier() || is_reserved_word();
  }

  constexpr bool is_reserved_word() const {
    return (m_type & RESERVED_WORD) > 0;
  }

  constexpr bool is_keyword() const { return (m_type & KEYWORD) == KEYWORD; }

  constexpr bool is_future_reserved_word() const {
    return (m_type & FUTURE_RESERVED_WORD) == FUTURE_RESERVED_WORD;
  }

  constexpr bool is_punctuator() const {
    return (m_type & PUNCTUATOR) == PUNCTUATOR;
  }

  constexpr bool is_null_literal() const {
    return (m_type & NULL_LITERAL) == NULL_LITERAL;
  }

  constexpr bool is_boolean_literal() const {
    return (m_type & BOOLEAN_LITERAL) == BOOLEAN_LITERAL;
  }

  constexpr bool is_numeric_literal() const {
    return (m_type & NUMERIC_LITERAL) == NUMERIC_LITERAL;
  }

  constexpr bool is_string_literal() const {
    return (m_type & STRING_LITERAL) == STRING_LITERAL;
  }

  constexpr bool is_regular_expression_literal() const {
    return (m_type & REGULAR_EXPRESSION_LITERAL) == REGULAR_EXPRESSION_LITERAL;
  }

  constexpr bool boolean_value() const {
    switch (m_type) {
    case type("true"):
      return true;
    case type("false"):
      return false;
    default:
      throw std::runtime_error("Invalid type");
    }
  }

  constexpr double numeric_value() const { return m_value.numeric_value; }

  std::u16string string_value() const {
    if (is_reserved_word())
      return convert_utf8_to_utf16(to_string());
    else if (!m_value.string_value) {
      return convert_utf8_to_utf16(to_string());
    }
    return *m_value.string_value;
  }

  constexpr bool operator==(const Token &other) const {
    if (type() != other.type())
      return false;
    switch (type()) {
    case NULL_LITERAL:
      return true;
    case BOOLEAN_LITERAL:
      return boolean_value() == other.boolean_value();
    case NUMERIC_LITERAL:
      return numeric_value() == other.numeric_value();
    case STRING_LITERAL:
      return string_value() == other.string_value();
    case REGULAR_EXPRESSION_LITERAL:
      return string_value() == other.string_value();
    default:
      return true;
    }
  }

  constexpr bool operator!=(const Token &other) const {
    return !operator==(other);
  }

  constexpr bool any_of(std::initializer_list<Token> tokens) const {
    for (auto it = tokens.begin(); it != tokens.end(); ++it)
      if (*this == *it)
        return true;
    return false;
  }

  template <typename... Args> constexpr bool any_of(Args &&... args) const {
    return any_of(std::initializer_list<Token>{std::forward<Args>(args)...});
  }

  operator std::string() const { return to_string(); }

  std::string to_string() const {
    switch (m_type) {
    case type("!"):
      return "!";
    case type("!="):
      return "!=";
    case type("!=="):
      return "!==";
    case type("%"):
      return "%";
    case type("%="):
      return "%=";
    case type("&"):
      return "&";
    case type("&&"):
      return "&&";
    case type("&="):
      return "&=";
    case type("("):
      return "(";
    case type(")"):
      return ")";
    case type("*"):
      return "*";
    case type("*="):
      return "*=";
    case type("+"):
      return "+";
    case type("++"):
      return "++";
    case type("+="):
      return "+=";
    case type(","):
      return ",";
    case type("-"):
      return "-";
    case type("--"):
      return "--";
    case type("-="):
      return "-=";
    case type("."):
      return ".";
    case type("/"):
      return "/";
    case type("/="):
      return "/=";
    case type(":"):
      return ":";
    case type(";"):
      return ";";
    case type("<"):
      return "<";
    case type("<<"):
      return "<<";
    case type("<<="):
      return "<<=";
    case type("<="):
      return "<=";
    case type("="):
      return "=";
    case type("=="):
      return "==";
    case type("==="):
      return "===";
    case type(">"):
      return ">";
    case type(">="):
      return ">=";
    case type(">>"):
      return ">>";
    case type(">>="):
      return ">>=";
    case type(">>>"):
      return ">>>";
    case type(">>>="):
      return ">>>=";
    case type("?"):
      return "?";
    case type("["):
      return "[";
    case type("]"):
      return "]";
    case type("^"):
      return "^";
    case type("^="):
      return "^=";
    case type("break"):
      return "break";
    case type("case"):
      return "case";
    case type("catch"):
      return "catch";
    case type("class"):
      return "class";
    case type("const"):
      return "const";
    case type("continue"):
      return "continue";
    case type("debugger"):
      return "debugger";
    case type("default"):
      return "default";
    case type("delete"):
      return "delete";
    case type("do"):
      return "do";
    case type("else"):
      return "else";
    case type("enum"):
      return "enum";
    case type("export"):
      return "export";
    case type("extends"):
      return "extends";
    case type("false"):
      return "false";
    case type("finally"):
      return "finally";
    case type("for"):
      return "for";
    case type("function"):
      return "function";
    case type("get"):
      return "get";
    case type("if"):
      return "if";
    case type("implements"):
      return "implements";
    case type("import"):
      return "import";
    case type("in"):
      return "in";
    case type("instanceof"):
      return "instanceof";
    case type("interface"):
      return "interface";
    case type("let"):
      return "let";
    case type("new"):
      return "new";
    case type("null"):
      return "null";
    case type("package"):
      return "package";
    case type("private"):
      return "private";
    case type("protected"):
      return "protected";
    case type("public"):
      return "public";
    case type("return"):
      return "return";
    case type("set"):
      return "set";
    case type("static"):
      return "static";
    case type("super"):
      return "super";
    case type("switch"):
      return "switch";
    case type("this"):
      return "this";
    case type("throw"):
      return "throw";
    case type("true"):
      return "true";
    case type("try"):
      return "try";
    case type("typeof"):
      return "typeof";
    case type("var"):
      return "var";
    case type("void"):
      return "void";
    case type("while"):
      return "while";
    case type("with"):
      return "with";
    case type("yield"):
      return "yield";
    case type("{"):
      return "{";
    case type("|"):
      return "|";
    case type("|="):
      return "|=";
    case type("||"):
      return "||";
    case type("}"):
      return "}";
    case type("~"):
      return "~";
    case NUMERIC_LITERAL:
      return std::to_string(numeric_value());
    case STRING_LITERAL:
      return convert_utf16_to_utf8(string_value());
    case REGULAR_EXPRESSION_LITERAL:
      return convert_utf16_to_utf8(string_value());
    case IDENTIFIER:
      return convert_utf16_to_utf8(string_value());
    default:
      throw std::runtime_error("Do not know what to do with Token of type " +
                               std::to_string(m_type));
    }
  }

  void print(std::ostream &out) const { out << to_string(); }

  constexpr bool preceded_by_line_terminator() const { return m_lt; }
  constexpr void set_preceded_by_line_terminator() { m_lt = true; }

  constexpr bool empty() const { return m_type == 0; }
  constexpr bool is_empty() const { return m_type == 0; }
};

std::ostream &operator<<(std::ostream &out, const Token &token);

#endif
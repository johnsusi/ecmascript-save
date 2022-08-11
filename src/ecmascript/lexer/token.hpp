#ifndef __ECMASCRIPT_LEXER_TOKEN_HPP
#define __ECMASCRIPT_LEXER_TOKEN_HPP

#include <string>
#include <variant>

namespace ecmascript
{

class Token
{
  public:
    struct IdentifierName
    {
    };
    struct Punctuator
    {
    };
    struct NumericLiteral
    {
    };
    struct StringLiteral
    {
    };

    Token()
    {
    }
    Token(const char *value, auto type) : value(value), type(type)
    {
    }

    Token(const char *value) : value(value)
    {
        if (isPunctuator() || isDivPunctuator())
            type = Punctuator{};
    }

    Token(std::string_view value) : value(value)
    {
    }

    Token(double value) : value(std::to_string(value)), type(NumericLiteral{})
    {
    }

    bool operator==(std::string_view other) const;
    operator std::string() const;

    bool isIdentifierName() const;
    bool isNullLiteral() const;
    bool isBooleanLiteral() const;
    bool isNumericLiteral() const;
    bool isStringLiteral() const;
    bool isRegularExpressionLiteral() const;
    bool isReservedWord() const;
    bool isKeyword() const;
    bool isFutureReservedWord() const;
    bool isPunctuator() const;
    bool isDivPunctuator() const;
    bool isIdentifier() const;
    bool separatedWithLineTerminator() const;

  private:
    std::string value;

    using Type = std::variant<IdentifierName, Punctuator, NumericLiteral, StringLiteral>;
    Type type;
};

} // namespace ecmascript

#endif
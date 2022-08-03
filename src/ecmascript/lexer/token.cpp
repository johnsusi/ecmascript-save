#include "token.hpp"

#include <algorithm>
#include <initializer_list>
#include <variant>

using namespace ecmascript;

bool Token::operator==(std::string_view other) const
{
    return value == other;
}

Token::operator std::string() const
{
    return value;
}

bool Token::isIdentifierName() const
{
    return std::holds_alternative<IdentifierName>(type);
}

bool Token::isNullLiteral() const
{
    return value == "null";
}

bool Token::isBooleanLiteral() const
{
    return value == "true" || value == "false";
}

bool Token::isNumericLiteral() const
{
    return false;
}

bool Token::isStringLiteral() const
{
    return false;
}

bool Token::isRegularExpressionLiteral() const
{
    return false;
}

bool Token::isReservedWord() const
{
    return isKeyword() || isFutureReservedWord() || isNullLiteral() || isBooleanLiteral();
}

bool Token::isKeyword() const
{
    constexpr static auto keywords = {
        "break",   "case",  "catch",    "continue", "debugger", "default",    "delete", "do",     "else",
        "finally", "for",   "function", "if",       "in",       "instanceof", "new",    "return", "switch",
        "this",    "throw", "try",      "typeof",   "var",      "void",       "while",  "with",
    };
    return std::find(keywords.begin(), keywords.end(), value) != keywords.end();
}

bool Token::isFutureReservedWord() const
{
    constexpr static auto keywords = {
        "class", "const",   "enum",    "export",    "extends", "implements", "import", "interface",
        "let",   "package", "private", "protected", "public",  "static",     "super",  "yield",
    };
    return std::find(keywords.begin(), keywords.end(), value) != keywords.end();
}

bool Token::isPunctuator() const
{
    static constexpr auto punctuators = {
        "{",   "}", "(", ")", "[",  "]",  ".",  ";",  ",",   "<",   ">",    "<=", ">=", "==", "!=", "===",
        "!==", "+", "-", "*", "%",  "++", "--", "<<", ">>",  ">>>", "&",    "|",  "^",  "!",  "~",  "&&",
        "||",  "?", ":", "=", "+=", "-=", "*=", "%=", "<<=", ">>=", ">>>=", "&=", "|=", "^=",
    };
    return std::find(punctuators.begin(), punctuators.end(), value) != punctuators.end();
}

bool Token::isDivPunctuator() const
{
    constexpr static auto punctuators = {"/", "/="};
    return std::find(punctuators.begin(), punctuators.end(), value) != punctuators.end();
}

bool Token::isIdentifier() const
{
    return false;
}

bool Token::separatedWithLineTerminator() const
{
    return false;
}

#ifndef __ECMASCRIPT_LEXER_LEXER_HPP
#define __ECMASCRIPT_LEXER_LEXER_HPP

#include <ecmascript/lexer/token.hpp>

#include <initializer_list>
#include <memory>

namespace ecmascript
{

class Lexer
{
  public:
    class Sentinel
    {
    };
    class Iterator
    {
      public:
        Iterator &operator++()
        {
            next();
            return *this;
        }

        Iterator operator++(int)
        {
            auto copy = *this;
            next();
            return copy;
        }

        const Token &operator*() const
        {
            return *get();
        }

        const Token *operator->() const
        {
            return get();
        }

        bool done() const;
        Token *get() const;
        bool next();

        bool operator==(const Iterator &other) const = default;
        bool operator==(const Sentinel &) const
        {
            return done();
        }

      private:
        std::string::iterator it, end;
    };

    Lexer(std::initializer_list<Token> tokens);
    Lexer(std::string source) : _source(source)
    {
    }

    Iterator begin();
    Sentinel end();

  private:
    std::string _source;
};

} // namespace ecmascript

#endif
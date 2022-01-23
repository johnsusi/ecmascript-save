#pragma once
#ifndef LIBECMASCRIPT_LEXER_HPP
#define LIBECMASCRIPT_LEXER_HPP

#include <memory>
#include <string>

class Token
{
  public:
    bool is_identifier_name() const;
    bool is_punctuator() const;
    bool is_numeric_literal() const;
    bool is_string_literal() const;
};

class Lexer
{
  public:
    Lexer(std::string_view source);
    bool is_input_element_div(std::string_view::iterator& it, std::string_view::iterator end, Token& value);

    class iterator
    {
      public:
        class sentinel
        {
        };
        iterator(Lexer* lexer, std::string_view::iterator begin, std::string_view::iterator end)
            : _prev(begin), _next(begin), _end(end)
        {
            advance();
        }
        iterator& operator++()
        {
            advance();
            return *this;
        }
        iterator operator++(int)
        {
            auto result = *this;
            result.advance();
            return result;
        }

        bool operator==(const sentinel& other) const
        {
            return _next == _end;
        }
        Token& operator*()
        {
            return _value;
        }

      private:
        Lexer* _lexer;
        std::string_view::iterator _prev, _next, _end;
        Token _value;
        void advance()
        {
            auto cur = _next;
            _lexer->is_input_element_div(_next, _end, _value);
        }
    };

    auto begin()
    {
        return iterator{this, _source.begin(), _source.end()};
    }
    auto end()
    {
        return iterator::sentinel{};
    }

  private:
    std::string_view _source;
};

#endif
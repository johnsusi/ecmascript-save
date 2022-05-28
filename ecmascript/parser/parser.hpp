#pragma once
#ifndef LIBECMASCRIPT_PARSER_PARSER_HPP
#define LIBECMASCRIPT_PARSER_PARSER_HPP

#include <stdexcept>
#include <string_view>
#include <string>

class SyntaxError : std::runtime_error
{
  public:
    SyntaxError(const char *message) : std::runtime_error(message)
    {
    }
};

class Parser
{

};

#endif
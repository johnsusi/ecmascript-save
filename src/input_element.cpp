#include "input_element.h"

bool is_line_terminator(int cp)
{
  switch (cp) {
  case 0x000A: // Line Feed <LF>
  case 0x000D: // Carriage Return <CR>
  case 0x2028: // Line separator <LS>
  case 0x2029: // Paragraph separator <PS>
    return true;
  default: return false;
  }
}

InputElement InputElement::empty()
{
  return {Empty{}};
}

InputElement InputElement::white_space()
{
  return {WhiteSpace{}};
}
InputElement InputElement::line_terminator()
{
  return {LineTerminator{}};
}

InputElement InputElement::comment(std::u16string value)
{
  return {Comment{value}};
}
InputElement InputElement::token(::Token value)
{
  return {InputElement::Token{std::move(value)}};
}

bool InputElement::is_empty() const
{
  return boost::get<Empty>(&value) != nullptr;
}

bool InputElement::is_token() const
{
  return boost::get<Token>(&value) != nullptr;
}

bool InputElement::is_line_terminator() const
{
  return boost::get<LineTerminator>(&value) != nullptr;
}

bool InputElement::is_comment() const
{
  return boost::get<Comment>(&value) != nullptr;
}

bool InputElement::is_white_space() const
{
  return boost::get<WhiteSpace>(&value) != nullptr;
}

bool InputElement::has_line_terminator() const
{
  if (is_line_terminator()) return true;
  if (auto comment = boost::get<Comment>(&value)) {
    return std::find_if(comment->value.begin(), comment->value.end(),
                        ::is_line_terminator) != comment->value.end();
  }
  return false;
}

InputElement::operator bool() const
{
  return !is_empty();
}

boost::optional<::Token> InputElement::to_token() const
{
  if (auto o = boost::get<Token>(&value)) return o->value;
  return {};
}
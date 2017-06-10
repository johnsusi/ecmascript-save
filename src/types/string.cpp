#include "string.h"

// struct String::Private {
//   std::u16string data;

//   Private(std::u16string data = {}) : data(data) {}
// };

String::String()
{
}

String::String(const std::u16string& value)
    : data(std::make_unique<std::u16string>(value))
{
}

String::String(const std::string& value)
    : String(std::u16string{value.begin(), value.end()})
{
}

String::String(const char* value) : String(std::string{value})
{
}

String::String(const char16_t* value) : String(std::u16string(value))
{
}

String::String(const String& other) : String(*other.data)
{
}

String::String(String&& other) : data(std::move(other.data))
{
}

String& String::operator=(const String& other)
{
  new (this) String(other);
  return *this;
}

String& String::operator=(String&& other)
{
  std::swap(data, other.data);
  return *this;
}

String::~String() = default;

std::size_t String::length() const
{
  return data->size();
}

bool String::operator==(const String& other) const
{
  return *data == *other.data;
}

bool String::operator!=(const String& other) const
{
  return !(operator==(other));
}

String::operator std::string() const
{
  return std::string{data->begin(), data->end()};
}

String::operator std::u16string() const
{
  return *data;
}
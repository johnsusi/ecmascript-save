#ifndef ECMASCRIPT_SOURCE_H
#define ECMASCRIPT_SOURCE_H

#include <string>

class Source {

  std::u16string data;

  Source(std::u16string data);

public:
  Source()
  {
  }
  static Source from_utf8(const std::string& s);
  static Source from_utf16(std::u16string);

  explicit operator const std::u16string&() const
  {
    return data;
  }

  auto begin() const
  {
    return data.begin();
  }
  auto end() const
  {
    return data.end();
  }
  auto size() const
  {
    return data.size();
  }
};

#endif
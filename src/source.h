#ifndef ECMASCRIPT_SOURCE_H
#define ECMASCRIPT_SOURCE_H

#include <string>

class Source {

  std::u16string data;

  Source(std::u16string data);

public:
  using iterator = std::u16string::const_iterator;

  Source();
  static Source from_utf8(const std::string& s);
  static Source from_utf16(std::u16string);

  explicit operator const std::u16string&() const;
  iterator    begin() const;
  iterator    end() const;
  std::size_t size() const;
};

#endif
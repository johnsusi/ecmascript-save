#ifndef ECMASCRIPT_EVAL_H
#define ECMASCRIPT_EVAL_H

// #include "ecmascript_export.h"
#include "options.h"
#include "util.h"
#include "variant.h"
#include "visitor.h"

#include <string>
#include <vector>

class Source {

  std::u16string data;

  Source(std::u16string&& data) : data(std::forward<std::u16string>(data))
  {
  }

public:
  static Source from_utf8(const std::string& s)
  {
    return {convert_utf8_to_utf16(s)};
  }
  static Source from_utf16(const std::u16string&);

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

// void eval(const std::string&, Visitor&, const Options&);
// void eval(const std::u16string&, Visitor&, const Options&);

void eval(const Source&, Visitor&, const Options&);

#endif
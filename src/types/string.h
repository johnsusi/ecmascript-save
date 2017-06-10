#ifndef ECMASCRIPT_STRING_H
#define ECMASCRIPT_STRING_H

#include <memory>
#include <string>

class String {

  std::unique_ptr<std::u16string> data;

public:
  String();
  String(const std::string&);
  String(const std::u16string&);
  String(const char16_t*);
  String(const char*);
  String(const String&);
  String(String&&);
  String& operator=(const String&);
  String& operator=(String&&);
  ~String();

  std::size_t length() const;

  operator std::u16string() const;
  operator std::string() const;
  bool operator==(const String& other) const;
  bool operator!=(const String& other) const;

  auto begin() const { return data->begin(); }
  auto end() const { return data->end(); }
};

#endif
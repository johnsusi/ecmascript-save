#include "source.h"

#include "util.h"

using namespace std;

Source::Source()
{
}

Source::Source(u16string data) : data(move(data))
{
}

Source Source::from_utf8(const string& s)
{
  return {convert_utf8_to_utf16(s)};
}

Source Source::from_utf16(u16string s)
{
  return {move(s)};
}

Source::operator const std::u16string&() const
{
  return data;
}

Source::iterator Source::begin() const
{
  return data.begin();
}

Source::iterator Source::end() const
{
  return data.end();
}

std::size_t Source::size() const
{
  return data.size();
}
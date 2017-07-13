#include "source.h"

#include "util.h"

using namespace std;

Source::Source(u16string data) : data(move(data))
{
}

Source Source::from_utf8(const string& s)
{
  return {move(convert_utf8_to_utf16(s))};
}

Source Source::from_utf16(u16string s)
{
  return {move(s)};
}

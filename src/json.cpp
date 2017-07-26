#include "json.h"
#include "util.h"

#include <stdexcept>

static constexpr char16_t ToHexDigit(int x)
{
  switch (x) {
  case 0x0: return u'0';
  case 0x1: return u'1';
  case 0x2: return u'2';
  case 0x3: return u'3';
  case 0x4: return u'4';
  case 0x5: return u'5';
  case 0x6: return u'6';
  case 0x7: return u'7';
  case 0x8: return u'8';
  case 0x9: return u'9';
  case 0xA: return u'a';
  case 0xB: return u'b';
  case 0xC: return u'c';
  case 0xD: return u'd';
  case 0xE: return u'e';
  case 0xF: return u'f';
  default: return 0;
  // default: return fail_with_logic_error("ToHexDigit called with invalid input ");
  // default: throw std::logic_error("ToHexDigit called with invalid input ");
  }
}

String Quote(const String& value)
// The abstract operation Quote(value) wraps a String value in double quotes and
// escapes characters within it.
{

  // 1. Let product be the double quote character.
  std::u16string product = u"\"";

  // 2. For each character C in value

  for (auto C : value) {
    switch (C) {
    // a. If C is the double quote character or the backslash character
    case u'"':
    case u'\\':
      //  i. Let product be the concatenation of product and the backslash
      //  character.
      product += u'\\';
      // ii. Let product be the concatenation of product and C.
      product += C;
      break;

    // b. Else if C is backspace, formfeed, newline, carriage return, or tab
    // i. Let product be the concatenation of product and the backslash
    // character.
    // ii. Let abbrev be the character corresponding to the value of C as
    // follows:
    // iii. Let product be the concatenation of product and abbrev.
    case u'\b': product += u"\\b"; break;
    case u'\f': product += u"\\f"; break;
    case u'\n': product += u"\\n"; break;
    case u'\r': product += u"\\r"; break;
    case u'\t': product += u"\\t"; break;
    default:

      // c. Else if C is a control character having a code unit value less than
      // the space character
      //   i. Let product be the concatenation of product and the backslash
      //   character.
      //  ii. Let product be the concatenation of product and "u".
      // iii. Let hex be the result of converting the numeric code unit value of
      // C to a String of four hexadecimal digits.
      //  iv. Let product be the concatenation of product and hex.

      if (C < u' ') {
        product += u"\\u";
        product += ToHexDigit((C / 0x1000) & 0xF);
        product += ToHexDigit((C / 0x0100) & 0xF);
        product += ToHexDigit((C / 0x0010) & 0xF);
        product += ToHexDigit((C / 0x0001) & 0xF);
      }
      // d. Else
      else {
        // i. Let product be the concatenation of product and C.
        product += C;
      }
      break;
    }
  }
  product += u'\"';
  return {std::move(product)};
}
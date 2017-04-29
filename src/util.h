#include <unicode/uchar.h>

const char* u_charCategory(int c)
{
  switch(u_charType(c))
  {
    case U_GENERAL_OTHER_TYPES: return "Cn";
    case U_UPPERCASE_LETTER: return "Lu";
    case U_LOWERCASE_LETTER: return "Ll";
    case U_TITLECASE_LETTER: return "Lt";
    case U_MODIFIER_LETTER: return "Lm";
    case U_OTHER_LETTER: return "Lo";
    case U_NON_SPACING_MARK: return "Mn";
    case U_ENCLOSING_MARK: return "Me";
    case U_COMBINING_SPACING_MARK: return "Mc";
    case U_DECIMAL_DIGIT_NUMBER: return "Nd";
    case U_LETTER_NUMBER: return "Nl";
    case U_OTHER_NUMBER: return "No";
    case U_SPACE_SEPARATOR: return "Zs";
    case U_LINE_SEPARATOR: return "Zl";
    case U_PARAGRAPH_SEPARATOR: return "Zp";
    case U_CONTROL_CHAR: return "Cc";
    case U_FORMAT_CHAR: return "Cf";
    case U_PRIVATE_USE_CHAR: return "Co";
    case U_SURROGATE: return "Cs";
    case U_DASH_PUNCTUATION: return "Pd";
    case U_START_PUNCTUATION: return "Ps";
    case U_END_PUNCTUATION: return "Pe";
    case U_CONNECTOR_PUNCTUATION: return "Pc";
    case U_OTHER_PUNCTUATION: return "Po";
    case U_MATH_SYMBOL: return "Sm";
    case U_CURRENCY_SYMBOL: return "Sc";
    case U_MODIFIER_SYMBOL: return "Sk";
    case U_OTHER_SYMBOL: return "So";
    case U_INITIAL_PUNCTUATION: return "Pi";
    default: return "??";
  }
}

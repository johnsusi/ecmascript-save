include(CheckCXXSourceCompiles)

check_cxx_source_compiles(
  "
    #include <unicode/unistr.h>

    int main()
    {
      icu::UnicodeString::fromUTF8(\"test\");
      return 0;
    }
  "
  HAVE_ICU_FROM_UTF8
)

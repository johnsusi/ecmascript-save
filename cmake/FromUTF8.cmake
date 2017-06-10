include(CheckCXXSourceCompiles)

# set(CMAKE_REQUIRED_INCLUDES  "${ICU_INCLUDE_DIRS}")
# set(CMAKE_REQUIRED_LIBRARIES "${ICU_LIBRARIES}")

check_cxx_source_compiles(
  "
    #include <unicode/unistr.h>

    int main()
    {
      icu::UnicodeString::fromUTF8(\"test\");
      return 0;
    }
  "
  HAVE_FROM_UTF8
)

if (HAVE_FROM_UTF8)
  add_definitions(-DHAVE_FROM_UTF8)
endif()
include(CheckCXXSourceCompiles)

message("CXX flags ${CMAKE_CXX_FLAGS}")

set(CMAKE_REQUIRED_FLAGS "-std=c++14")

check_cxx_source_compiles(
  "
    #include <variant>
    int main()
    {
      std::variant<double, int> foo { 1 };
      return 0;
    }
  "
  HAVE_STD_VARIANT
)

set(CMAKE_REQUIRED_FLAGS "-std=c++14")

check_cxx_source_compiles(
  "
    #include <experimental/variant>
    int main()
    {
      std::experimental::variant<double, int> foo(1);
      return 0;
    }
  "
  HAVE_STD_EXPERIMENTAL_VARIANT
)

set(CMAKE_REQUIRED_FLAGS "-std=c++14")
set(CMAKE_REQUIRED_INCLUDES  "${Boost_INCLUDE_DIRS}")
set(CMAKE_REQUIRED_LIBRARIES "${Boost_LIBRARIES}")

check_cxx_source_compiles(
  "
    #include <boost/variant.hpp>
    int main()
    {
      boost::variant<double, int> foo(1);
      return 0;
    }
  "
  HAVE_BOOST_VARIANT
)

unset(CMAKE_REQUIRED_FLAGS)
unset(CMAKE_REQUIRED_INCLUDES)
unset(CMAKE_REQUIRED_LIBRARIES)
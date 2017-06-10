include(CheckCXXSourceCompiles)

set(CMAKE_REQUIRED_FLAGS "-std=c++14")

check_cxx_source_compiles(
  "
    #include <optional>
    int main()
    {
      std::optional<int> foo { 1 };
      return foo ? 0 : -1;
    }
  "
  HAVE_STD_OPTIONAL
)

set(CMAKE_REQUIRED_FLAGS "-std=c++14")

check_cxx_source_compiles(
  "
    #include <experimental/optional>
    int main()
    {
      std::experimental::optional<int> foo(1);
      return foo ? 0 : -1;
    }
  "
  HAVE_STD_EXPERIMENTAL_OPTIONAL
)


set(CMAKE_REQUIRED_FLAGS "-std=c++14")
set(CMAKE_REQUIRED_INCLUDES  "${Boost_INCLUDE_DIRS}")
set(CMAKE_REQUIRED_LIBRARIES "${Boost_LIBRARIES}")

check_cxx_source_compiles(
  "
    #include <boost/optional.hpp>
    int main()
    {
      boost::optional<int> foo(1);
      return foo ? 0 : -1;
    }
  "
  HAVE_BOOST_OPTIONAL
)

unset(CMAKE_REQUIRED_FLAGS)
unset(CMAKE_REQUIRED_INCLUDES)
unset(CMAKE_REQUIRED_LIBRARIES)
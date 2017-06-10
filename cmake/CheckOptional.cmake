include(CheckCXXSourceCompiles)

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
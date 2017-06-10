include(CheckCXXSourceCompiles)

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

check_cxx_source_compiles(
  "
    #include <mpark/variant.hpp>
    int main()
    {
      mpark::variant<double, int> foo(1);
      return 0;
    }
  "
  HAVE_MPARK_VARIANT
)

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

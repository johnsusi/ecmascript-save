include(CheckCXXSourceCompiles)

check_cxx_source_compiles(
  "
    #include <filesystem>
    namespace fs = std::filesystem;

    int main()
    {
      auto name = fs::current_path().root_name();
      return 0;
    }
  "
  HAVE_STD_FILESYSTEM
)

check_cxx_source_compiles(
  "
    #include <experimental/filesystem>
    namespace fs = std::experimental::filesystem;

    int main()
    {
      auto name = fs::current_path().root_name();
      return 0;
    }
  "
  HAVE_STD_EXPERIMENTAL_FILESYSTEM
)

check_cxx_source_compiles(
  "
    #include <boost/filesystem.hpp>
    namespace fs = boost::filesystem;

    int main()
    {
      auto name = fs::current_path().root_name();
      return 0;
    }
  "
  HAVE_BOOST_FILESYSTEM
)
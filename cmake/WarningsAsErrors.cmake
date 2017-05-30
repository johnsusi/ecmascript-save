
include(CheckCXXCompilerFlag)

CHECK_CXX_COMPILER_FLAG(-Wall      WALL_SUPPORTED)
CHECK_CXX_COMPILER_FLAG(-Werror    WERROR_SUPPORTED)
CHECK_CXX_COMPILER_FLAG(-Wpedantic WPEDANTIC_SUPPORTED)

if (WALL_SUPPORTED)
  add_compile_options(-Wall)
endif()

if (WERROR_SUPPORTED)
  add_compile_options(-Werror)
endif()

if (WPEDANTIC_SUPPORTED)
  add_compile_options(-Wpedantic)
endif()
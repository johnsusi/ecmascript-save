
include(CheckCXXCompilerFlag)

CHECK_CXX_COMPILER_FLAG(-fcolor-diagnostics COMPILER_FLAG_SUPPORTED)

if (COMPILER_FLAG_SUPPORTED)
  add_compile_options(-fcolor-diagnostics)
endif()

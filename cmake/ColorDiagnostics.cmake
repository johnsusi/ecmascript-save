
include(CheckCXXCompilerFlag)

check_cxx_compiler_flag(-fcolor-diagnostics HAVE_COLOR_DIAGNOSTICS)

if (HAVE_COLOR_DIAGNOSTICS)
  add_compile_options(-fcolor-diagnostics)
endif()



include(CheckCXXCompilerFlag)

check_cxx_compiler_flag(-flto           LTO_SUPPORTED)
check_cxx_compiler_flag(-fwhole-program WP_SUPPORTED)

if (LTO_SUPPORTED)
  add_compile_options(-flto)
  SET( CMAKE_EXE_LINKER_FLAGS  "${CMAKE_EXE_LINKER_FLAGS} -flto" )
endif()

if (WP_SUPPORTED)
  add_compile_options(-fwhole-program)
  SET( CMAKE_EXE_LINKER_FLAGS  "${CMAKE_EXE_LINKER_FLAGS} -fwhole-program" )
endif()

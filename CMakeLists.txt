cmake_minimum_required(VERSION 3.8)

project(ECMAScript C CXX)

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_LIST_DIR}/cmake")

if (CMAKE_VERSION VERSION_LESS 3.7)
  list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_LIST_DIR}/cmake/compat")
endif ()

option(TESTING "Enable testing"                      OFF)
option(UNITY   "Enable unity build"                  OFF)
option(BUILD_SHARED_LIBS "Build shared libraries"    OFF)

include(Cxx14Support)
include(ColorDiagnostics)

if (UNITY)
 add_library(Ecmascript
      src/single.cpp
      src/dtoa.c
  )
else ()

  add_library(Ecmascript
      src/ast.cpp
      src/eval.cpp
      src/input_element.cpp
      src/json.cpp
      src/lexer.cpp
      src/logger.cpp
      src/matcher.cpp
      src/parser.cpp
      src/runtime.cpp
      src/token.cpp
      src/trace.cpp
      src/util.cpp
      src/type.cpp
      src/visitor.cpp
      src/dtoa.c

      src/types/undefined.cpp
      src/types/null.cpp
      src/types/boolean.cpp
      src/types/number.cpp
      src/types/string.cpp
      src/types/object.cpp
  )

endif()

target_compile_definitions(Ecmascript PRIVATE -DIEEE_8087)

target_include_directories(Ecmascript
  PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}>
  $<INSTALL_INTERFACE:include>
)

add_executable(ECMAScript src/main.cpp src/single.cpp src/dtoa.c)
target_compile_definitions(ECMAScript PRIVATE -DIEEE_8087)

target_include_directories(ECMAScript
  PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}>
)

if (NOT Boost_FOUND)
  find_package(Boost COMPONENTS filesystem)
  if (Boost_FOUND)
    target_link_libraries(ECMAScript PUBLIC Boost::filesystem)
  endif()
endif()

if (NOT ICU_FOUND)
  find_package(ICU REQUIRED COMPONENTS uc)
  target_link_libraries(ECMAScript PUBLIC ICU::uc)
endif()


if (TESTING)

  include(WarningsAsErrors)

  set_source_files_properties(src/dtoa.c PROPERTIES COMPILE_FLAGS -Wno-error)

  enable_testing()

  file(GLOB UNITTEST_SOURCES "src/*_unittest.cpp")

  add_executable(UnitTest src/test_runner.cpp ${UNITTEST_SOURCES})

  target_link_libraries(UnitTest Ecmascript)

  add_test(NAME test COMMAND UnitTest)

endif()

# include(GenerateExportHeader)
# generate_export_header(ecmascript)

get_target_property(CMAKE_REQUIRED_INCLUDES ECMAScript INCLUDE_DIRECTORIES)
get_target_property(CMAKE_REQUIRED_LIBRARIES ECMAScript LINK_LIBRARIES)

include(CheckFilesystem)
include(CheckICU)
include(CheckOptional)
include(CheckVariant)

configure_file(src/config.h.in config.h @ONLY)

install(TARGETS Ecmascript ECMAScript EXPORT EcmascriptTargets
  LIBRARY DESTINATION lib
  ARCHIVE DESTINATION lib
  RUNTIME DESTINATION bin
  INCLUDES DESTINATION include
)

# install(EXPORTS EcmascriptTargets
#   FILE EcmascriptTargets.cmake
#   NAMESPACE Ecmascript::
#   DESTINATION lib/cmake/Ecmascript
# )
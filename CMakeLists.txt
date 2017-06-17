cmake_minimum_required(VERSION 3.5)

cmake_policy(SET CMP0056 NEW)

project(ECMAScript C CXX)

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_LIST_DIR}/cmake")

if (CMAKE_VERSION VERSION_LESS 3.7)
  list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_LIST_DIR}/cmake/compat")
endif ()

option(BUILD_SHARED_LIBS "Build shared libraries"           OFF)
option(DEFINES           "Add defines to build"             OFF)
option(INCLUDES          "Add include directories to build" OFF)
option(LIBRARIES         "Add libraries to build"           OFF)
option(UNITY             "Enable unity build"               OFF)
option(TESTING           "Enable testing"                   OFF)
option(STATIC            "Enable static building"           OFF)

include(Cxx14Support)

if (STATIC)
  set(CMAKE_FIND_LIBRARY_SUFFIXES ".lib;.a")
endif()

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

set_target_properties(Ecmascript
  PROPERTIES
    CXX_VISIBILITY_PRESET hidden
    VISIBILITY_INLINES_HIDDEN 1
)

target_compile_definitions(Ecmascript PRIVATE -DIEEE_8087)

target_include_directories(Ecmascript
  PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}>
    $<INSTALL_INTERFACE:include>
)

if (DEFINES)
  target_compile_definitions(Ecmascript PRIVATE ${DEFINES})
endif()

if (INCLUDES)
  target_include_directories(Ecmascript PUBLIC $<BUILD_INTERFACE:${INCLUDES}>)
endif()

if (LIBRARIES)
  target_link_libraries(Ecmascript PUBLIC ${LIBRARIES})
endif()

if (NOT Boost_FOUND)
  set(Boost_USE_MULTITHREADED OFF)
  find_package(Boost COMPONENTS filesystem)
  if (Boost_FOUND)
    target_link_libraries(Ecmascript PUBLIC Boost::filesystem)
  endif()
endif()

if (NOT ICU_FOUND)
  find_package(ICU REQUIRED COMPONENTS uc)
  target_link_libraries(Ecmascript PUBLIC ICU::uc)
endif()

add_executable(ECMAScriptBin src/main.cpp)
target_link_libraries(ECMAScriptBin PUBLIC Ecmascript)
set_target_properties(ECMAScriptBin
  PROPERTIES
    OUTPUT_NAME "ECMAScript"
    CXX_VISIBILITY_PRESET hidden
    VISIBILITY_INLINES_HIDDEN 1
)
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

get_target_property(CMAKE_REQUIRED_INCLUDES Ecmascript INCLUDE_DIRECTORIES)
get_target_property(CMAKE_REQUIRED_LIBRARIES_ Ecmascript LINK_LIBRARIES)
list(APPEND CMAKE_REQUIRED_LIBRARIES ${CMAKE_REQUIRED_LIBRARIES_})

include(CheckFilesystem)
include(CheckICU)
include(CheckOptional)
include(CheckVariant)

configure_file(src/config.h.in config.h @ONLY)

# install(TARGETS Ecmascript ECMAScript EXPORT EcmascriptTargets
#   LIBRARY DESTINATION lib
#   ARCHIVE DESTINATION lib
#   RUNTIME DESTINATION bin
#   INCLUDES DESTINATION include
# )

# install(EXPORTS EcmascriptTargets
#   FILE EcmascriptTargets.cmake
#   NAMESPACE Ecmascript::
#   DESTINATION lib/cmake/Ecmascript
# )
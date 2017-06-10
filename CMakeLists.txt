cmake_minimum_required(VERSION 3.5)
# 3.7 with compability support for 3.5

project(ECMAScript C CXX)

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_LIST_DIR}/cmake")

if (CMAKE_VERSION VERSION_LESS 3.7)
  list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_LIST_DIR}/cmake/compat")
endif ()

option(LTO     "Enable Link-Time-Optimization"       OFF)
option(PGO     "Enable Profile-Guided-Optimization"  OFF)
option(RELEASE "Enable release build"                OFF)
option(STATIC  "Avoid dynamic libraries if possible" OFF)
option(TESTING "Enable testing"                      OFF)
option(BUILD_SHARED_LIBS "Generate shared library instead of static"   OFF)

include(Cxx14Support)
include(ColorDiagnostics)
include(EnableVisibilityHidden)

if (LTO)
  include(EnableLTO)
endif ()

if (STATIC)
  set(Boost_USE_STATIC_LIBS ON)
  if(${CMAKE_SYSTEM_NAME} STREQUAL "Linux")
    set(CMAKE_FIND_LIBRARY_SUFFIXES ".a")
    set(BUILD_SHARED_LIBRARIES OFF)
    set(CMAKE_EXE_LINKER_FLAGS "-static")
    list(APPEND EXTRA_LIBRARIES "-lpthread")
  endif()
endif ()

if (RELEASE)
  add_definitions(-DECMASCRIPT_RELEASE)
endif ()

if (APPLE)
  set (icu_include_suffixes "include")
  if (NOT ICU_ROOT)
    set(ICU_ROOT "/usr/local/opt/icu4c")
  endif ()
endif ()

find_package(Boost REQUIRED COMPONENTS filesystem program_options)
find_package(ICU   REQUIRED COMPONENTS uc)

if (STATIC AND APPLE)
  set(ICU_LIBRARIES icucore)
  add_definitions(-DU_DISABLE_RENAMING)
endif()

include(CheckOptional)
include(checkVariant)

configure_file(src/config.h.in config.h @ONLY)

include_directories(
  ${CMAKE_CURRENT_BINARY_DIR}
  vendor/Catch/include
  vendor/GSL/include
  vendor/variant/include
  ${Boost_INCLUDE_DIRS}
  ${ICU_INCLUDE_DIRS}
)

include(FromUTF8)

add_definitions(-DIEEE_8087)

if (TESTING)

  set(SOURCES
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

  include(WarningsAsErrors)

  enable_testing()

  file(GLOB UNITTEST_SOURCES "src/*_unittest.cpp")

  add_executable(UnitTest src/test_runner.cpp ${UNITTEST_SOURCES} ${SOURCES})

  target_link_libraries(UnitTest ${Boost_LIBRARIES} ${ICU_LIBRARIES})

  add_test(NAME test COMMAND UnitTest)

else ()

  set(SOURCES
    src/single.cpp
    src/dtoa.c
  )

endif ()

add_library(ecmascript ${SOURCES})

include(GenerateExportHeader)
generate_export_header(ecmascript)

add_executable(ECMAScript src/main.cpp)

target_link_libraries(ECMAScript
  ecmascript
  ${Boost_LIBRARIES}
  ${ICU_LIBRARIES}
  ${EXTRA_LIBRARIES}
)

cmake_minimum_required(VERSION 3.5)
# 3.7 with compability support for 3.5

project(ECMAScript C CXX)

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_LIST_DIR}/cmake")

if (CMAKE_VERSION VERSION_LESS 3.7)
  list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_LIST_DIR}/cmake/compat")
endif ()

option(LTO     "Enable Link-Time-Optimization"       OFF)
option(RELEASE "Enable release build"                OFF)
option(STATIC  "Avoid dynamic libraries if possible" OFF)
option(TESTING "Enable testing"                      OFF)

include(Cxx14Support)
include(ColorDiagnostics)

if (LTO)
  include(EnableLTO)
endif ()

if (STATIC)
  set(Boost_USE_STATIC_LIBS ON)
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
  message("Linking to icucore")
  set(ICU_LIBRARIES icucore)
  add_definitions(-DU_DISABLE_RENAMING)
endif()

include_directories(
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
    src/lexer.cpp
    src/logger.cpp
    src/matcher.cpp
    src/parser.cpp
    src/to_string.cpp
    src/token.cpp
    src/tracer.cpp
    src/util.cpp
    src/visitor.cpp
    src/dtoa.c
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

add_executable(${PROJECT_NAME} src/main.cpp ${SOURCES})

target_link_libraries(${PROJECT_NAME} ${Boost_LIBRARIES} ${ICU_LIBRARIES})

set_property(TARGET ${PROJECT_NAME} PROPERTY INTERPROCEDURAL_OPTIMIZATION 1)
cmake_minimum_required(VERSION 3.21)

project(
    ECMAScript
    DESCRIPTION  "Implementation of the 5.1 edition of the ECMAScript language specification"
    VERSION      1.0.0
    LANGUAGES    CXX
    HOMEPAGE_URL https://github.com/johnsusi/ecmascript
)

option(BUILD_TESTS "Build tests"         YES)
option(BUILD_DOCS  "Build documentation" YES)

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/build/cmake")

include(test)


add_subdirectory(src)

if (BUILD_TESTS)
    add_subdirectory(tests)
endif()

if (BUILD_DOCS)
    find_package(Doxygen REQUIRED)
    add_subdirectory(docs)
endif()
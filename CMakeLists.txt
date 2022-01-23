cmake_minimum_required(VERSION 3.16.3...3.21.1)

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")
set(CMAKE_CXX_EXTENSIONS NO)

project(ecmascript
    DESCRIPTION "Implementation of ECMA-262 5.1 edition."
    LANGUAGES   CXX
    VERSION     1.0.0
)

option(BUILD_TESTS "Build tests"         YES)
option(BUILD_DOCS  "Build documentation"  NO)

if (BUILD_TESTS)
    find_package(Catch2 CONFIG REQUIRED)
    include(CTest)
    include(Catch)
endif()

add_subdirectory(ecmascript)

if (BUILD_DOCS)
    find_package(Doxygen REQUIRED)
    add_subdirectory(docs)
endif()

if (BUILD_TESTS)
    add_subdirectory(tests)
endif()

# include(GNUInstallDirs)
# install(
#     TARGETS  libecmascript es
#     EXPORT   ${CMAKE_PROJECT_NAME}Targets
#     RUNTIME  DESTINATION ${CMAKE_INSTALL_BINDIR}
#              COMPONENT          ${CMAKE_PROJECT_NAME}_Runtime
#     LIBRARY  DESTINATION ${CMAKE_INSTALL_LIBDIR}
#              COMPONENT          ${CMAKE_PROJECT_NAME}_Runtime
#              NAMELINK_COMPONENT ${CMAKE_PROJECT_NAME}_Development
#     ARCHIVE  DESTINATION ${CMAKE_INSTALL_LIBDIR}
#              COMPONENT          ${CMAKE_PROJECT_NAME}_Development
#     INCLUDES DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
#              COMPONENT          ${CMAKE_PROJECT_NAME}_Development
# )

# if (NOT APPLE)
#     set(CMAKE_INSTALL_RPATH $ORIGIN)
# endif()

# include(Pack)
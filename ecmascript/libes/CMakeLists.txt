project(libes LANGUAGES CXX)

add_library("${PROJECT_NAME}"
    compiler.cpp
    compiler.hpp
    json.cpp
    json.hpp
    lexer.cpp
    lexer.hpp
    parser.cpp
    parser.hpp
)

add_library("${CMAKE_PROJECT_NAME}::${PROJECT_NAME}" ALIAS "${PROJECT_NAME}")

target_compile_features("${PROJECT_NAME}" PRIVATE cxx_std_20)

target_include_directories("${PROJECT_NAME}"
    PUBLIC 
        $<BUILD_INTERFACE:${CMAKE_BINARY_DIR}/include>
        $<INSTALL_INTERFACE:include>
)

set_target_properties("${PROJECT_NAME}"
    PROPERTIES
        PREFIX     ""
        SOVERSION  ${CMAKE_PROJECT_VERSION_MAJOR}
        VERSION    ${CMAKE_PROJECT_VERSION}
)

file(GLOB 
    PUBLIC_HEADERS 
    LIST_DIRECTORIES NO
    "${CMAKE_CURRENT_SOURCE_DIR}"
    *.hpp
)
file(COPY ${PUBLIC_HEADERS} DESTINATION ${CMAKE_BINARY_DIR}/include/${CMAKE_PROJECT_NAME}/${PROJECT_NAME}/)

install(
    DIRECTORY ${CMAKE_BINARY_DIR}/include/
    DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
)

# target_sources(libecmascript
#     PUBLIC
#         $<INSTALL_INTERFACE:ecmascript.hpp>
# )


# add_executable(libecmascript-tests
#     ../tests/driver.cpp
#     json.test.cpp
# )

# target_compile_features(libecmascript-tests PRIVATE cxx_std_20)

# target_link_libraries(libecmascript-tests PRIVATE
#     Catch2::Catch2
#     Catch2::Catch2WithMain
# )

# catch_discover_tests(libecmascript-tests)

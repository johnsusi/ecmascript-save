add_subdirectory(lexer)
add_subdirectory(json)

add_library(libecmascript
    ecmascript.h
    ecmascript.cpp
    parser/parser.cpp
    parser/parser.hpp
)

add_library(ecmascript::libecmascript ALIAS libecmascript)

target_compile_features(libecmascript PRIVATE cxx_std_20)
target_include_directories(libecmascript
    PUBLIC 
        $<BUILD_INTERFACE:${CMAKE_SOURCE_DIR}>
        $<INSTALL_INTERFACE:include>
)

target_link_libraries(libecmascript PRIVATE json lexer)

add_executable(es main.cpp)

target_compile_features(es PRIVATE cxx_std_20)
target_link_libraries(es PRIVATE libecmascript)

add_executable(unittests
    parser/parser.test.cpp
)

target_compile_features(unittests PRIVATE cxx_std_20)
target_link_libraries(unittests PRIVATE
    Catch2::Catch2WithMain
)

catch_discover_tests(unittests)
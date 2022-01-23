add_subdirectory(lexer)
add_subdirectory(json)

add_library(libecmascript
    ecmascript.h
    ecmascript.cpp
)

add_library(ecmascript::libecmascript ALIAS libecmascript)

target_include_directories(libecmascript
    PUBLIC 
        $<BUILD_INTERFACE:${CMAKE_SOURCE_DIR}>
        $<INSTALL_INTERFACE:include>
)

target_link_libraries(libecmascript PRIVATE json lexer)

add_executable(es main.cpp)

target_compile_features(es PRIVATE cxx_std_20)
target_link_libraries(es PRIVATE libecmascript)
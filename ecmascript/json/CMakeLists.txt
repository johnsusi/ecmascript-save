project(json LANGUAGES CXX)

add_library("${PROJECT_NAME}"
    json.cpp
    json.hpp
    parser.cpp
)

add_library("${CMAKE_PROJECT_NAME}::${PROJECT_NAME}" ALIAS "${PROJECT_NAME}")

target_compile_features("${PROJECT_NAME}" PRIVATE cxx_std_20)

target_include_directories("${PROJECT_NAME}"
    PUBLIC 
        $<BUILD_INTERFACE:${CMAKE_SOURCE_DIR}>
        $<INSTALL_INTERFACE:include>
)


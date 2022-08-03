add_library(json
    ecmascript/json/json.hpp
    ecmascript/json/json.cpp
)

set_target_properties(json
    PROPERTIES PUBLIC_HEADER "ecmascript/json/json.hpp"
)

target_compile_features(json
    PUBLIC cxx_std_20
)

target_include_directories(json
    PUBLIC $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
)

add_library(ecmascript::json ALIAS json)

add_library(ecmascript
    ecmascript/ecmascript.cpp
    ecmascript/ecmascript.hpp
    ecmascript/lexer/token.cpp
    ecmascript/lexer/token.hpp
    ecmascript/lexer/lexer.cpp
    ecmascript/lexer/lexer.hpp
    ecmascript/parser/expression.cpp
    ecmascript/parser/expression.hpp
    ecmascript/parser/grammar.hpp
    ecmascript/parser/grammar.cpp
    ecmascript/parser/parser.cpp
    ecmascript/parser/parser.hpp
    ecmascript/parser/program.hpp
    ecmascript/parser/statement.cpp
    ecmascript/parser/statement.hpp
)

set_target_properties(ecmascript
    PROPERTIES PUBLIC_HEADER "ecmascript/lexer/lexer.hpp;ecmascript/lexer/token.hpp;ecmascript/parser/parser.hpp"
)

target_compile_features(ecmascript
    PUBLIC cxx_std_20
)

target_include_directories(ecmascript
    PUBLIC $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
)

install(TARGETS ecmascript EXPORT ECMAScriptTargets
    LIBRARY DESTINATION lib
    ARCHIVE DESTINATION lib
    RUNTIME DESTINATION bin
    INCLUDES DESTINATION include
    PUBLIC_HEADER DESTINATION include
)

install(EXPORT ECMAScriptTargets
    FILE ecmascripTargets.cmake
    NAMESPACE ECMAScript::
    DESTINATION lib/cmake/ECMAScript
)

add_library(ecmascript::ecmascript ALIAS ecmascript)


add_executable(es
    ecmascript/es/main.cpp
)

target_compile_features(es
    PUBLIC cxx_std_20
)

target_link_libraries(es
    PRIVATE json
)



add_unit_test(
    ecmascript/lexer/lexer.test.cpp
    ecmascript
)

add_unit_test(
    ecmascript/parser/expression.test.cpp
    ecmascript
)


add_unit_test(
    ecmascript/parser/grammar.test.cpp
    ecmascript
)




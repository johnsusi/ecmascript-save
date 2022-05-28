add_executable(tests
    json/null.cpp
    json/false.cpp
    json/true.cpp
    json/number.cpp
    json/string.cpp
    json/array.cpp
    json/object.cpp
    json/whitespace.cpp
    lexer/expression.cpp
    parser/statement.cpp
)

target_compile_features(tests PRIVATE cxx_std_20)
target_link_libraries(tests PRIVATE
    Catch2::Catch2WithMain
    ecmascript::json
    ecmascript::lexer
)

catch_discover_tests(tests)
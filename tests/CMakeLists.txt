add_executable(tests
    json/number.cpp
    json/true.cpp
    json/false.cpp
    json/null.cpp    
)

target_compile_features(tests PRIVATE cxx_std_20)

target_link_libraries(tests
    PRIVATE Catch2::Catch2WithMain
            ecmascript::json        
)

catch_discover_tests(tests)

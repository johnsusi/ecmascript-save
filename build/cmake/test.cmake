if (BUILD_TESTS)
    find_package(Catch2 3 REQUIRED)
    include(CTest)
    include(Catch)

    function(add_unit_test test)
        string(REGEX REPLACE "[\\/.]" "_" testname ${test})
        add_executable(${testname} ${test})
        target_compile_features(${testname} PRIVATE cxx_std_20)
        target_link_libraries(${testname} PRIVATE Catch2::Catch2WithMain ${ARGN})
        catch_discover_tests(${testname})
    endfunction()

    function(add_unit_tests)
        foreach(test IN ITEMS ${ARGN})
            add_unit_test(${test})
        endforeach()
    endfunction()

else()

    function(add_unit_test test)
    endfunction()

    function(add_unit_tests)
    endfunction()

endif()



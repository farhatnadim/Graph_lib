if(BUILD_TESTING)
    add_executable(graph_test graph_test_program.cpp)
    target_link_libraries(graph_test GTest::gtest_main)

    include(GoogleTest)
    gtest_discover_tests(graph_test_program)
endif()
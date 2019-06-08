set(TEST_SRCS
    test_event.cpp
    test_dispatcher.cpp
    test_event_queue.cpp
    )

foreach(TEST_FILE ${TEST_SRCS})
    get_filename_component(TEST_MODULE ${TEST_FILE} NAME_WE)
    message("Test ${TEST_MODULE}")
    add_executable(${TEST_MODULE} ${TEST_FILE})
    target_link_libraries(${TEST_MODULE} gtest_main pthread)
    target_link_libraries(${TEST_MODULE} aopp)
    add_test(${TEST_MODULE} ${TEST_MODULE})
endforeach()

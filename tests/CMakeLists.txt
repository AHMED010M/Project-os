# Tests executable
add_executable(basic_test
    basic_test.cpp
    ../shared/common.cpp
)

target_include_directories(basic_test PRIVATE
    ${CMAKE_SOURCE_DIR}
)

target_link_libraries(basic_test
    Threads::Threads
)

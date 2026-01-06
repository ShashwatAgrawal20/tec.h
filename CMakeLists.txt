cmake_minimum_required(VERSION 3.10)
project(tec.h C)
set(CMAKE_C_STANDARD 23)

if(MSVC)
    add_compile_options(/TP /EHsc)
    add_compile_definitions("_CRT_SECURE_NO_WARNINGS")
else()
    add_compile_options(-Wall -Wextra -pedantic)
endif()

if(CMAKE_PROJECT_NAME STREQUAL PROJECT_NAME)
    file(GLOB_RECURSE SRC_FILES "src/*.c")
    file(GLOB_RECURSE TEST_SOURCES "tests/*.c")
    list(FILTER SRC_FILES EXCLUDE REGEX ".*/main\\.c$")

    add_executable(main src/main.c ${SRC_FILES})
    add_executable(test_runner ${TEST_SOURCES} ${SRC_FILES})
    target_include_directories(main PRIVATE include)
    target_include_directories(test_runner PRIVATE include)
    add_custom_target(test
        COMMAND $<TARGET_FILE:test_runner>
        DEPENDS test_runner
        WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
        COMMENT "Running tests..."
    )
    add_custom_target(workflow_test -f !this_xfail_should_unexpectedly_pass_and_fail_the_suite
        COMMAND $<TARGET_FILE:test_runner>
        DEPENDS test_runner
        WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
        COMMENT "Running tests..."
    )
else()
    message(STATUS "tec.h: Header-only library (skipping executables)")
endif()

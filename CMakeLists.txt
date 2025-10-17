cmake_minimum_required(VERSION 3.10)
project(tec.h C)
set(CMAKE_C_STANDARD 23)
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall -Wextra -pedantic")
include_directories(include)

if(CMAKE_PROJECT_NAME STREQUAL PROJECT_NAME)
    file(GLOB_RECURSE SRC_FILES "src/*.c")
    file(GLOB_RECURSE TEST_SOURCES "tests/*.c")
    list(FILTER SRC_FILES EXCLUDE REGEX ".*/main\\.c$")

    add_executable(main src/main.c ${SRC_FILES})
    add_executable(test_runner ${TEST_SOURCES} ${SRC_FILES})
    add_custom_target(build_tests
        COMMAND test_runner
        DEPENDS test_runner
        COMMENT "Running tests..."
    )
else()
    message(STATUS "tec.h: Header-only library (skipping executables)")
endif()

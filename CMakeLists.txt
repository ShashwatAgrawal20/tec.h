cmake_minimum_required(VERSION 3.10)
project(tec.h C)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall -Wextra -pedantic")

include_directories(include)

file(GLOB_RECURSE SRC_FILES "src/*.c")

file(GLOB_RECURSE TEST_SOURCES "tests/*.c")

list(FILTER SRC_FILES EXCLUDE REGEX ".*/main\\.c$")

add_executable(main src/main.c ${SRC_FILES})
add_executable(test_runner ${TEST_SOURCES} ${SRC_FILES})

add_custom_target(test
    COMMAND test_runner
    DEPENDS test_runner
    COMMENT "Running tests..."
)

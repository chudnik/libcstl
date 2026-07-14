cmake_minimum_required(VERSION 4.2)
project(libcstl C)

set(CMAKE_C_STANDARD 23)
set(CMAKE_C_STANDARD_REQUIRED ON)

add_library(libcstl STATIC src/vector.c)

target_include_directories(libcstl PUBLIC include)

target_compile_options(libcstl PRIVATE -Wall -Wextra -Wpedantic)

enable_testing()

add_executable(vector_test tests/vector_test.c)
target_link_libraries(vector_test PRIVATE libcstl)

add_test(NAME vector_test COMMAND vector_test)
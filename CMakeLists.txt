cmake_minimum_required(VERSION 3.16...3.23)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_VERBOSE_MAKEFILE ON)
set(CMAKE_CXX_FLAGS_INIT "-fsanitize=address -fsanitize=undefined -fsanitize-address-use-after-scope")

project(BasicString)

add_executable(
    test
    test.cpp
)

add_compile_options(
    test
    -fsanitize=address
    -flto
)

add_link_options(
    test
    -fsanitize=address
    -flto
)

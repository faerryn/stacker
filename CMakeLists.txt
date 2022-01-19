cmake_minimum_required(VERSION 3.22)

project(stacker)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)
add_compile_options(-g -Wall -Wextra -Wpedantic -Werror)
set(CMAKE_EXPORT_COMPILE_COMMANDS True)

add_executable(stacker main.cc lexer.cc parser.cc engine.cc)

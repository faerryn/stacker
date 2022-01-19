cmake_minimum_required(VERSION 3.22)

project(stacker)

add_executable(stacker main.cc lexer.cc parser.cc engine.cc)

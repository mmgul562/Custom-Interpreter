cmake_minimum_required(VERSION 3.23)
project(cpp_interpreter)

set(CMAKE_CXX_STANDARD 20)

add_executable(cpp_interpreter main.cpp lexer.cpp lexer.h parser.cpp parser.h ast.cpp ast.h)

cmake_minimum_required(VERSION 3.23)
project(cpp_interpreter)

set(CMAKE_CXX_STANDARD 20)

add_executable(cpp_interpreter main.cpp core/lexer.cpp core/lexer.h core/parser.cpp core/parser.h core/ast.cpp core/ast.h core/scope.h)

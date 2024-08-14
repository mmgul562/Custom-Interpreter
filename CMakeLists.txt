cmake_minimum_required(VERSION 3.23)
project(cpp_interpreter)

set(CMAKE_CXX_STANDARD 20)

add_executable(cpp_interpreter main.cpp core/main/lexer.cpp core/main/lexer.h core/main/parser.cpp core/main/parser.h core/main/ast.cpp core/main/ast.h core/scope.h core/value.h core/scope.cpp core/value.cpp util/errors.h
        util/functions.cpp
        util/functions.h
        util/utf8string.cpp
        util/utf8string.h)

cmake_minimum_required(VERSION 3.27)
project(prog)

set(CMAKE_CXX_STANDARD 17)
add_executable(prog main.cpp
        Tokenizer.cpp
        Tokenizer.h
        Lexer.cpp
        Lexer.h
        Visitor.cpp
        Visitor.h)

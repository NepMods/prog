cmake_minimum_required(VERSION 3.27)
project(prog)

set(CMAKE_CXX_STANDARD 17)
add_executable(prog src/main.cpp
        src/Tokenizer.cpp
        src/Tokenizer.h
        src/Lexer.cpp
        src/Lexer.h
        src/Visitor.cpp
        src/Visitor.h)

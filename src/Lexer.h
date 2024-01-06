//
// Created by arjun on 12/31/23.
//
#pragma once
#include <iostream>
#include <fstream>
#include "json.hpp"
#include "Tokenizer.h"

class Lexer {
public:
    Lexer(Tokenizer& tokens);
    void eat(const std::string& tokenType);
    nlohmann::json lexParam();
    nlohmann::json lexStatement();
    nlohmann::json lexID();
    nlohmann::json lex();
    
    nlohmann::json lex_if_condition();

    nlohmann::json lex_function_defination();
    nlohmann::json lex_variable_defination();

    nlohmann::json lex_function_call();


    void MainLoop();
    nlohmann::json getCompund();

private:
    Tokenizer& tokenizer;
    std::vector<nlohmann::json> tokens;
    size_t at;
    nlohmann::json currentToken;
    nlohmann::json ast;

    nlohmann::json addAST(const std::string& type, const nlohmann::json& data);
};

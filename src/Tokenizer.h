#pragma once
#include <iostream>
#include <vector>
#include "json.hpp"

class Tokenizer {
public:
    Tokenizer(const std::string& source);
    void MainLoop();
    void addToken(const std::string& tokenName, const std::string& value);
    nlohmann::json getTokens();

private:
    std::string realSource;
    std::string source;
    size_t at;
    size_t line;
    size_t atLine;
    char currentChar;
    std::vector<nlohmann::json> tokens;
    const std::unordered_map<char, std::string> predefinedTokens = {
        {'=', "TOKEN_EQUALS"},
        {';', "TOKEN_SEMI"},
        {'(', "TOKEN_LPAREN"},
        {')', "TOKEN_RPAREN"},
        {'{', "TOKEN_LBRACE"},
        {'}', "TOKEN_RBRACE"},
        {'[', "TOKEN_LBRACKET"},
        {']', "TOKEN_RBRACKET"},
        {',', "TOKEN_COMMA"},
        {'.', "TOKEN_PERIOD"},
        {'+', "TOKEN_PLUS"},
        {'-', "TOKEN_MINUS"},
        {'*', "TOKEN_STAR"},
        {'/', "TOKEN_SLASH"},
        {'%', "TOKEN_PERCENT"},
        {':', "TOKEN_COLON"},
        {'\'', "TOKEN_QUOTE"},
        {'"', "TOKEN_QUOTE"},
        {'#', "TOKEN_HASH"},
        {'<', "TOKEN_LT"},
        {'>', "TOKEN_GT"},
        {'_', "TOKEN_UNDERSCORE"},
        {'\\', "TOKEN_BACKSLASH"},
        {'!', "TOKEN_BANG"}
    };

    char peek(size_t i = 1);
    char cur();
    char eat();
    std::string parseId();
    std::string parseNumber();
    std::string parseString();
    void parse();
};

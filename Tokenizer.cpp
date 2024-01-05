#include "Tokenizer.h"
#include <fstream>

Tokenizer::Tokenizer(const std::string& source) :
        realSource(source), source(source), at(0), line(1), atLine(0), currentChar(' ') {
    MainLoop();
    addToken("TOKEN_EOF", "\0");

    std::ofstream outputFile("tokenizer.json");
    outputFile << getTokens().dump(2);
    outputFile.close();
}

void Tokenizer::MainLoop() {
    while (true) {
        if (at > source.length() - 1) {
            break;
        }
        parse();
    }
}

void Tokenizer::addToken(const std::string& tokenName, const std::string& value) {
    tokens.push_back({
                             {"name", tokenName},
                             {"value", value},
                             {"at", {line, atLine}}
                     });
}

nlohmann::json Tokenizer::getTokens() {
    nlohmann::json jsonTokens(tokens);
    return jsonTokens;
}

char Tokenizer::peek(size_t i) {
    if (at + i >= source.length()) {
        return '\0';
    }
    if (source[at + i] == '\0') {
        return '\0';
    }
    return source[at + i];
}

char Tokenizer::cur() {
    try {
        return source.at(at);
    } catch (...) {
        return '\0';
    }
}

char Tokenizer::eat() {
    if (cur() == '\0') {
        return currentChar;
    }

    if (peek() == '\0') {
        at += 1;
        return currentChar;
    }

    currentChar = peek();
    at += 1;
    atLine += 1;
    return currentChar;
}

std::string Tokenizer::parseId() {
    std::string result = "";
    char current = cur();

    if (std::isalpha(current)) {
        result += current;
    }
    eat();

    while (std::isalnum(cur())) {
        result += cur();
        eat();
    }

    return result;
}

std::string Tokenizer::parseNumber() {
    std::string result = "";
    char current = cur();

    if (std::isdigit(current)) {
        result += current;
    }
    eat();

    while (std::isdigit(cur())) {
        result += cur();
        eat();
    }

    return result;
}

std::string Tokenizer::parseString() {
    std::string result = "";
    char start = cur();
    if (cur() == '"' || cur() == '\'') {
        eat();
    } else {
        return "";
    }

    while (cur() != start) {
        result += cur();
        eat();
    }

    eat();
    return result;
}

void Tokenizer::parse() {
    char current = cur();

    if (current == ' ') {
        eat();
        return;
    }

    if (current == '\n') {
        line += 1;
        atLine = 0;
        eat();
        return;
    }

    if (cur() == '"' || cur() == '\'') {
        addToken("TOKEN_STRING", parseString());
        return;
    }

    if (std::isalpha(current)) {
        addToken("TOKEN_ID", parseId());
        return;
    }

    if (std::isdigit(current)) {
        addToken("TOKEN_NUMBER", parseNumber());
        return;
    }

    if (predefinedTokens.find(current) != predefinedTokens.end()) {
        addToken(predefinedTokens.at(current), std::string(1, current));
        eat();
        return;
    }

    throw std::runtime_error("UNKNOWN TOKEN: " + std::string(1, current));
}

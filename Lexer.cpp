//
// Created by arjun on 12/31/23.
//

#include "Lexer.h"

Lexer::Lexer(Tokenizer& tokens) : tokenizer(tokens), tokens(tokens.getTokens()), at(0), currentToken(tokens.getTokens()[0]) {
    MainLoop();

    std::ofstream outputFile("ast.json");
    outputFile << ast.dump(2);
    outputFile.close();
}

void Lexer::eat(const std::string& tokenType) {
    if (tokens[at]["name"] == tokenType) {
        at += 1;
        currentToken = tokens[at];
    } else {
        throw std::runtime_error("Expected " + tokenType + " But found " + tokens[at]["name"].get<std::string>());
    }
}

nlohmann::json Lexer::lexParam() {
    nlohmann::json params;

    while (currentToken["name"] != "TOKEN_RPAREN") {
        params.push_back(lexStatement());
        if (currentToken["name"] != "TOKEN_RPAREN") {
            eat("TOKEN_COMMA");
        }
    }

    return params;
}

nlohmann::json Lexer::lexStatement() {
    std::string value = currentToken["value"];
    std::string c_type = currentToken["name"];
    if (c_type == "TOKEN_ID") {
        auto ret = addAST("VAR_GET", {{"value", value}, {"type", c_type}});
        eat(currentToken["name"]);
        if(currentToken["name"] == "TOKEN_LBRACKET") {
            eat("TOKEN_LBRACKET");
            eat("TOKEN_RBRACKET");
            ret["name"] = "PARAM_ARGV";
        }
        return ret;
    }

    if(c_type == "TOKEN_LBRACKET") {
        value = currentToken["value"];
        c_type = currentToken["name"];
        eat("TOKEN_LBRACKET");
        eat("TOKEN_RBRACKET");
        auto ret = addAST("PARAM_ARGV", {{"value", value}, {"type", c_type}});
        eat("TOKEN_ID");
        return ret;
    }
    std::string type = (c_type == "TOKEN_STRING") == 1 ? "String" : "any";
    type = (c_type == "TOKEN_NUMBER") == 1 ? "Number" : type;
    type = (value == "true") ? "Boolean" : type;
    type = (value == "false") ? "Boolean" : type;

    auto ret = addAST("CONST", {{"value", value}, {"type", type}});
    eat(currentToken["name"]);
    return ret;
}

nlohmann::json Lexer::lexID() {
    nlohmann::json ret;
    if (currentToken["value"] == "let") {
        return lex_variable_defination();
    }
    if (currentToken["value"] == "if") {
        return lex_if_condition();
    }
    if (currentToken["value"] == "function") {
        return lex_function_defination();
    }
    if (tokens[at + 1]["name"] == "TOKEN_LPAREN") {
        return lex_function_call();
    }
    return lexStatement();

}

nlohmann::json Lexer::lex() {

    if (currentToken["name"] == "TOKEN_ID") {
        return lexID();
    } else {
        return lexStatement();
    }
}

nlohmann::json Lexer::lex_if_condition() {
    nlohmann::json ret;
    eat("TOKEN_ID");

    eat("TOKEN_LPAREN");
    auto value = lexStatement();
    eat("TOKEN_RPAREN");
    eat("TOKEN_LBRACE");
    
    std::vector<nlohmann::json> nodes;
    while(currentToken["name"] != "TOKEN_RBRACE") {
        auto currentNode = lex();
        nodes.push_back(currentNode);
    }
    eat("TOKEN_RBRACE");
    ret = addAST("AST_CONDITION", nodes);

    ret["value"]= value;
    return ret;
}

nlohmann::json Lexer::lex_function_defination() {
    nlohmann::json ret;
    eat("TOKEN_ID");
    std::string name = currentToken["value"];
    eat("TOKEN_ID");

    eat("TOKEN_LPAREN");
    auto params = lexParam();

    eat("TOKEN_RPAREN");
    eat("TOKEN_LBRACE");
    std::vector<nlohmann::json> nodes;
    while(currentToken["name"] != "TOKEN_RBRACE") {
        auto currentNode = lex();
        nodes.push_back(currentNode);
    }
    eat("TOKEN_RBRACE");
    ret = addAST("AST_FUNCDEF", nodes);

    ret["value"]= name;

    return ret;
}

nlohmann::json Lexer::lex_variable_defination() {
    nlohmann::json ret;
    eat("TOKEN_ID");
    std::string name = currentToken["value"];
    eat("TOKEN_ID");

    if (currentToken["name"] == "TOKEN_SEMI") {
        ret = addAST("VAR_ADD", {{"value", "None"}, {"type", "any"}, {"name", name}});
        eat("TOKEN_SEMI");
        return ret;
    }

    eat("TOKEN_EQUALS");
    auto value = lexStatement();
    ret = addAST("VAR_ADD", {{"value", value}, {"type", value["data"]["type"]}, {"name", name}});
    eat("TOKEN_SEMI");
    return ret;
}

nlohmann::json Lexer::lex_function_call() {
    nlohmann::json ret;
    std::string name = currentToken["value"];
    eat("TOKEN_ID");
    eat("TOKEN_LPAREN");

    auto params = lexParam();
    eat("TOKEN_RPAREN");
    ret = addAST("FUN_CALL", {{"value", name}, {"params", params}});
    eat("TOKEN_SEMI");

    return ret;
}

void Lexer::MainLoop() {
    std::vector<nlohmann::json> nodes;

    while (true) {
        if (currentToken["name"] == "TOKEN_EOF") {
            break;
        }

        auto currentNode = lex();
        nodes.push_back(currentNode);
    }

    ast = addAST("AST_COMPOUND", nodes);
}

nlohmann::json Lexer::getCompund() {
    return ast;
}

nlohmann::json Lexer::addAST(const std::string& type, const nlohmann::json& data) {
    return {{"name", type}, {"data", data}};
}

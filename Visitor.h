#ifndef VISITOR_H
#define VISITOR_H

#include <iostream>
#include <fstream>
#include "json.hpp"
#include <unordered_map>

class Visitor {
public:
    explicit Visitor(const nlohmann::json& mainCompound);

    long generateAssembly(const nlohmann::json& node, std::ostream& output);

private:
    std::ofstream outputFile;
    int currentStackOffset;
    std::unordered_map<std::string, long> variableOffsets;
    std::unordered_map<std::string, long> var_size;
    std::string data;
    std::string text;
    std::string code;
    int strCount = 0;
    long stackSize = 0;
    void emit(std::ostream& output, const std::string& instruction, const std::string& operand = "") const;
    void generateVariableDeclaration(const nlohmann::json& node);
    long generateVariableAccess(const nlohmann::json& node);
    void generateFunctionCall(const nlohmann::json& node);
    void generatePrintCall(const nlohmann::json& node);
};

#endif // VISITOR_H

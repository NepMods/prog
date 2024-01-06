#ifndef VISITOR_H
#define VISITOR_H

#include <iostream>
#include <fstream>
#include "json.hpp"
#include <unordered_map>

class Visitor {
public:
    explicit Visitor(const nlohmann::json& mainCompound);


private:
    std::ofstream outputFile;
    int currentStackOffset;
    std::unordered_map<std::string, std::string> variableOffsets;
    std::unordered_map<std::string, std::string> var_type;
    std::string data;
    std::string text;
    std::string code;
    int strCount = 0;
    long stackSize = 0;
    long lastType = 0;
    void emit(std::ostream& output, const std::string& instruction, const std::string& operand = "") const;
    void visit(const nlohmann::json& node);
    void visit_compound(const nlohmann::json& node);
    void visit_condition(const nlohmann::json& node);
    void visit_function_defincation(const nlohmann::json& node);
    void visit_variable_defination(const nlohmann::json& node);
    void visit_variable_visit(const nlohmann::json& node);
    void visit_function_call(const nlohmann::json& node);

    void generatePrintCall(const nlohmann::json& node);
};

#endif // VISITOR_H

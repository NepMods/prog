#include "Visitor.h"

Visitor::Visitor(const nlohmann::json& mainCompound) : currentStackOffset(0) {
    outputFile.open("output.asm");
    data = "section .data\n";
    text = "section .text\n";
    code = "";

    text += "global _start";
    code += "_start:\n"\
            "   push    rbp\n"\
            "   mov     rbp, rsp\n"\
            "   sub     rsp, 48\n"\
            "   mov     dword [rbp - 4], 0\n";
    generateAssembly(mainCompound, outputFile);
    code += "   mov eax, 60\n" \
            "   xor edi, edi\n"\
            "   syscall\n";
    emit(outputFile, data);
    emit(outputFile, text);
    emit(outputFile, code);
    outputFile.close();
}

long Visitor::generateAssembly(const nlohmann::json& node, std::ostream& output) {
    const std::string type = node["name"].get<std::string>();

    if (type == "AST_COMPOUND") {
        for (const auto& child : node["data"]) {
            generateAssembly(child, output);
        }
    } else if (type == "VAR_ADD") {
        generateVariableDeclaration(node);
    } else if (type == "VAR_GET") {
        return generateVariableAccess(node);
    } else if (type == "FUN_CALL") {
        generateFunctionCall(node);
    } else {
        // Handle other node types if necessary
    }
    return 0;
}

void Visitor::emit(std::ostream& output, const std::string& instruction, const std::string& operand) const {
    output << "" << instruction;
    if (!operand.empty()) {
        output << " " << operand;
    }
    output << "\n";
}

void Visitor::generateVariableDeclaration(const nlohmann::json& node) {
    const std::string varName = node["data"]["name"].get<std::string>();

    std::string type =  node["data"]["type"].get<std::string>();

    if(type == "String") {
        std::string name = "str" + std::to_string(strCount);
        std::string value =  node["data"]["value"]["data"]["value"].get<std::string>();
        data += name + " db \"" + value + "\", 0\n";
        code += "   lea     rax, [rel "+ name+"]\n";
        code += "   mov     qword [rbp - "+std::to_string(stackSize+8)+"], rax\n";
        stackSize += 8;
        strCount++;
    }
    if(type == "Number") {
        std::string value =  node["data"]["value"]["data"]["value"].get<std::string>();
        code += "   mov     dword [rbp - "+std::to_string(stackSize+4)+"], "+value+"\n";
        stackSize += 4;
    }
    variableOffsets[varName] = stackSize;
    var_size[varName] = static_cast<long>(node["data"]["value"]["data"]["value"].get<std::string>().size());
}

long Visitor::generateVariableAccess(const nlohmann::json& node) {
    const std::string varName = node["data"]["value"].get<std::string>();
    const long offset = variableOffsets[varName];
    code += "   mov     rdi, qword [rbp - "+std::to_string(offset)+"]\n";
    return var_size[varName];
}

void Visitor::generateFunctionCall(const nlohmann::json& node) {
    const std::string funcName = node["data"]["value"].get<std::string>();

    if (funcName == "print") {
        generatePrintCall(node);
    } else if(funcName == "inject") {
        auto assembly = "   " + node["data"]["params"][0]["data"]["value"].get<std::string>();
        code += assembly + "\n";
    }else {

    }
}

void Visitor::generatePrintCall(const nlohmann::json& node) {
    const auto& params = node["data"]["params"];

    for (const auto& param : params) {
        long length = generateAssembly(param, outputFile);
        code += "   ;Print \n"\
        "   mov     rsi, rdi\n"\
        "   mov     rdx, "+std::to_string(length)+"\n"\
        "   mov     rax, 1\n"\
        "    mov     rdi, 1\n"\
        "   syscall\n";
    }

}

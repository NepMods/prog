#include "Visitor.h"

Visitor::Visitor(const nlohmann::json& mainCompound) : currentStackOffset(0) {
    outputFile.open("output.asm");
    data = "section .data\n";
    text = "section .text\n";
    code = "";

    text += "global _start\n";
    code += "_start:\n"\
            "   push    rbp\n"\
            "   mov     rbp, rsp\n"\
            "   sub     rsp, 48\n"\
            "   mov     dword [rbp - 4], 0\n"\
            "   call _main;\n"\
            "   mov eax, 60\n" \
            "   xor edi, edi\n"\
            "   syscall\n";

    text += "\nglobal print_uint32 \n";
    text += "\nglobal print \n";
    code += "print_int:\n"\
            "    mov    eax, edi\n"\
            "    mov    ecx, 0xa\n"\
            "    push   rcx\n"\
            "    mov    rsi, rsp\n"\
            "    sub    rsp, 16\n"\
            "\n"\
            "    .toascii_digit:\n"\
            "    xor    edx, edx\n"\
            "    div    ecx\n"\
            "    add    edx, '0'\n"\
            "    dec    rsi\n"\
            "    mov    [rsi], dl\n"\
            "    test   eax,eax\n"\
            "    jnz  .toascii_digit\n"\
            "    mov    eax, 1\n"\
            "    mov    edi, 1\n"\
            "    lea    edx, [rsp+16 + 1]\n"\
            "    sub    edx, esi\n"\
            "    syscall\n"\
            "    add  rsp, 24\n"\
            "    ret";
    code +=  "\nprint: \n"\
        "   mov     rsi, rdi\n"\
        "   mov     rdx, r8\n"\
        "   mov     rax, 1\n"\
        "   mov     rdi, 1\n"\
        "   syscall\n"\
        "   ret\n";


    visit(mainCompound);
    emit(outputFile, data);
    emit(outputFile, text);
    emit(outputFile, code);
    outputFile.close();
}



void Visitor::emit(std::ostream& output, const std::string& instruction, const std::string& operand) const {
    output << "" << instruction;
    if (!operand.empty()) {
        output << " " << operand;
    }
    output << "\n";
}

void Visitor::visit(const nlohmann::json& node) {
    const std::string type = node["name"].get<std::string>();

    if (type == "AST_COMPOUND") {
        visit_compound(node);
    }
    if(type == "AST_FUNCDEF") {
        visit_function_defincation(node);
    }
    if (type == "VAR_ADD") {
        visit_variable_defination(node);
    }
    if (type == "VAR_GET") {
        visit_variable_visit(node);
    }
    if(type == "FUN_CALL") {
        visit_function_call(node);
    }
}

void Visitor::visit_compound(const nlohmann::json& node) {
    for (const auto& child : node["data"]) {
        visit(child);
    }
}

void Visitor::visit_function_defincation(const nlohmann::json& node) {
    auto name = node["value"].get<std::string>();

    text += "global _"+name+"\n";
    code += "\n_"+name+":\n";
    for (const auto& child : node["data"]) {
        visit(child);
    }
    code += "   ret\n";
}

void Visitor::visit_variable_defination(const nlohmann::json& node) {
    const std::string varName = node["data"]["name"].get<std::string>();

    std::string type =  node["data"]["type"].get<std::string>();
    std::string name = type + std::to_string(strCount);
    if(type == "String") {
        std::string value =  node["data"]["value"]["data"]["value"].get<std::string>();
        data += name + " db \"" + value + "\", 0xA\n";
        data += name+"_len equ $ - "+name+"\n";
    }
    if(type == "Number") {
        std::string value =  node["data"]["value"]["data"]["value"].get<std::string>();
        data += name+" dq "+value+"\n";
    }
    variableOffsets[varName] = name;
    var_type[varName] = type;
    strCount ++;
}

void Visitor::visit_variable_visit(const nlohmann::json& node) {
    const std::string varName = node["data"]["value"].get<std::string>();
    const std::string name = variableOffsets[varName];
    const std::string type = var_type[varName];

    if(type == "String") {
        code += "   mov     rdi,  "+name+"\n";
        code += "   mov     r8, "+name+"_len\n";
        lastType = 0;
    }
    if(type == "Number") {
        code += "   mov     edi,  ["+name+"]\n";
        lastType = 1;
    }

}

void Visitor::visit_function_call(const nlohmann::json& node) {
    const std::string funcName = node["data"]["value"].get<std::string>();

    if (funcName == "print") {
        generatePrintCall(node);
    } else if(funcName == "inject") {
        auto assembly = "   " + node["data"]["params"][0]["data"]["value"].get<std::string>();
        code += assembly + "\n";
    }else {
        code += "   call _" + funcName + "\n";
    }
}



void Visitor::generatePrintCall(const nlohmann::json& node) {
    const auto& params = node["data"]["params"];

    for (const auto& param : params) {
        visit(param);
        switch (lastType) {
            case 0:
                code += "   call print\n";
            break;
            case 1:
                code += "   call print_int\n";
        }
    }

}

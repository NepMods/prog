#include "Tokenizer.h"
#include <fstream>

#include "Lexer.h"
#include "Visitor.h"
#include <cstdlib>  // for the system function
int main(int argc, char* argv[]) {
    if (argc < 2) {
        std::cerr << "Usage: " << argv[0] << " <input_file>\n";
        return 1;
    }

    std::ifstream inputFile(argv[1]);
    if (!inputFile.is_open()) {
        std::cerr << "Error opening file: " << argv[1] << "\n";
        return 1;
    }
    std::string source((std::istreambuf_iterator<char>(inputFile)),
                       std::istreambuf_iterator<char>());

    Tokenizer tokenizer(source);
    Lexer lexter(tokenizer);
    Visitor visitor(lexter.getCompund());

    std::system("nasm -f elf64 -o output.o output.asm; ld -o output output.o; ./output");

    return 0;
}

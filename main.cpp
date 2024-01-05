#include "Tokenizer.h"
#include <fstream>

#include "Lexer.h"
#include "Visitor.h"

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

    return 0;
}

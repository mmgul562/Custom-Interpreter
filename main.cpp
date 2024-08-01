#include "core/parser.h"


int main() {
    std::cout << std::boolalpha;
    Lexer lexer("");
    Parser parser(lexer);
    auto globalScope = std::make_shared<Scope>();
    std::string input;

    std::cout << "Type 'exit' to quit" << std::endl;
    while (true) {
        std::cout << "> ";
        std::string line;
        input.clear();
        do {
            std::getline(std::cin, line);
            if (line == "exit" && input.empty()) {
                return 0;
            }
            input += line + "\n";
            lexer.reset(input);
            parser.advanceToken();
        } while (!parser.isStatementComplete());

        try {
            lexer.reset(input);
            parser.advanceToken();
            auto statements = parser.parse();
            Value lastResult;
            for (const auto& statement : statements) {
                lastResult = statement->evaluate(globalScope);
            }
            printValue(lastResult);
        } catch (const std::exception &e) {
            std::cout << "Error: " << e.what() << std::endl;
        }
    }
}

#include "parser.h"

int main() {
    Lexer lexer = Lexer("");
    Parser parser = Parser(lexer);
    std::string input;
    double result;

    std::cout << "Enter a basic mathematical expression (or type 'exit' to quit)" << std::endl;

    while (true) {
        std::cout << "> ";
        std::getline(std::cin, input);

        if (input == "exit") {
            break;
        }

        try {
            lexer.reset(input);
            parser.advanceToken();
            auto ast = parser.parseStatement();
            result = ast->evaluate();

            std::cout << result << std::endl;
        } catch (const std::exception& e) {
            std::cout << e.what() << std::endl;
        }
    }

    return 0;
}

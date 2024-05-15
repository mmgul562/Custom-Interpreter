#include "parser.h"

int main() {
    Lexer lexer = Lexer("");
    Parser parser = Parser(lexer);

    std::cout << "Enter a mathematical expression (type 'exit' to quit)" << std::endl;
    while (true) {
        std::string input;
        std::cout << "> ";
        std::getline(std::cin, input);

        if (input == "exit") {
            break;
        }

        try {
            lexer.reset(input);
            parser.reset();
            auto ast = parser.parseExpression();
            double result = ast->evaluate();

            std::cout << result << std::endl;
        } catch (const std::exception& e) {
            std::cout << e.what() << std::endl;
        }
    }

    return 0;
}

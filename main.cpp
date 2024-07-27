#include "parser.h"

int main() {
    Lexer lexer("");
    Parser parser(lexer);
    std::string input;
    std::cout << "Type 'exit' to quit" << std::endl;
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
            Value result = ast->evaluate();
            std::visit([](const auto& v) {
                using T = std::decay_t<decltype(v)>;
                if constexpr (std::is_same_v<T, bool>) {
                    std::cout << (v ? "true" : "false") << std::endl;
                } else {
                    std::cout << v << std::endl;
                }
            }, result);
        } catch (const std::exception &e) {
            std::cout << "Error: " << e.what() << std::endl;
        }
    }
    return 0;
}
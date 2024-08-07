#include "core/main/parser.h"

#define RST  "\x1B[0m"
#define RED  "\x1B[31m"


int main() {
    std::cout << std::boolalpha;
    Lexer lexer("");
    Parser parser(lexer);
    auto globalScope = std::make_shared<Scope>();
    std::string input;
    bool continuation;

    std::cout << "Type 'exit' to quit" << std::endl;
    while (true) {
        std::cout << "> ";
        std::string line;
        input.clear();
        do {
            std::getline(std::cin, line);
            line.erase(line.find_last_not_of(" \t")+1);
            if (line == "exit" && input.empty()) {
                return 0;
            }
            if (!line.empty() && line.back() == '\\') {
                line.pop_back();
                continuation = true;
            } else {
                continuation = false;
            }
            input += line;
            if (!continuation) {
                input += "\n";
            }
            lexer.reset(input);
            parser.advanceToken();
        } while (continuation || !parser.isStatementComplete());

        try {
            auto statements = parser.parse();
            Value result;
            for (const auto &statement: statements) {
                result = statement->evaluate(globalScope);
                printValue(result);
                std::cout << std::endl;
            }
        } catch (const ControlFlowException &e) {
            std::cout << RED << "Syntax error: Use of " + std::string(e.what()) + " outside of loops" << std::endl;
        } catch (const BaseError &e) {
            std::cout << RED << e.what() << RST << std::endl;
        } catch (const std::exception &e) {
            std::cout << RED << "Unexpected error: " << e.what() << RST << std::endl;
        }
    }
}

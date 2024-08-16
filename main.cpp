#include "util/errors.h"
#include "core/main/parser.h"
#include <iostream>

#define RST  "\x1B[0m"
#define RED  "\x1B[31m"


int main() {
    std::cout << std::fixed;
    Lexer lexer("");
    Parser parser(lexer);
    auto globalScope = std::make_shared<Scope>();
    std::string input;
    bool continuation;

    std::cout << "Wpisz 'wyjdz' aby wyjść" << std::endl;
    while (true) {
        std::cout << "> ";
        std::string line;
        input.clear();
        try {
            do {
                std::getline(std::cin, line);
                line.erase(line.find_last_not_of(" \t") + 1);
                if (line == "wyjdz" && input.empty()) {
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

            auto statements = parser.parse();
            Value result;
            for (const auto &statement: statements) {
                result = statement->evaluate(globalScope);
                printValue(result, true);
                std::cout << std::endl;
            }
        } catch (const ControlFlowException &e) {
            std::cout << RED << "Błąd przepływu sterowania: Użycie " + std::string(e.what()) + " poza pętlą" << RST << std::endl;
        } catch (const ReturnException &e) {
            std::cout << RED << "Błąd przepływu sterowania: Użycie ZWROC poza funkcją" << RST << std::endl;
        } catch (const BaseError &e) {
            std::cout << RED << e.what() << RST << std::endl;
        } catch (const std::exception &e) {
            std::cout << RED << "Niespodziewany błąd: " << e.what() << RST << std::endl;
        }
    }
}

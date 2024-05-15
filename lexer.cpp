#include "lexer.h"


Token Lexer::getNextToken() {
    while (pos < input.length()) {
        if (isspace(input[pos])) {
            ++pos;
            continue;
        }
        if (isdigit(input[pos]) || input[pos] == '.') {
            return extractNumber();
        }
        switch (input[pos]) {
            case '+': ++pos; return Token(TokenType::PLUS);
            case '-': ++pos; return Token(TokenType::MINUS);
            case '*': ++pos; return Token(TokenType::ASTER);
            case '/': ++pos; return Token(TokenType::SLASH);
            case '(': ++pos; return Token(TokenType::LPAREN);
            case ')': ++pos; return Token(TokenType::RPAREN);
            default:
                char err[23];
                std::sprintf(err, "Unexpected character: %c", input[pos]);
                throw std::runtime_error(err);
        }
    }
    return Token(TokenType::END);
}

Token Lexer::extractNumber() {
    size_t start = pos;
    while (pos < input.length() && (isdigit(input[pos]) || input[pos] == '.')) {
        ++pos;
    }
    return Token(TokenType::NUMBER, std::stod(input.substr(start, pos - start)));
}

void Lexer::reset(const std::string& newInput) {
    input = newInput;
    pos = 0;
}
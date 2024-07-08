#include "lexer.h"


Token Lexer::getNextToken() {
    size_t length = input.length();
    while (pos < length) {
        if (isspace(input[pos])) {
            ++pos;
            continue;
        }
        if (isdigit(input[pos])) {
            return extractNumber();
        }
        if (isalpha(input[pos])) {
            return extractIdentifier();
        }
        switch (input[pos]) {
            case '=': ++pos; return Token(TokenType::ASSIGN);
            case '+': ++pos; return Token(TokenType::PLUS);
            case '-': {
                if (isdigit(input[++pos])) return extractNumber(true);
                return Token(TokenType::MINUS);
            }
            case '*': {
                if (input[++pos] == '*') {
                    ++pos;
                    return Token(TokenType::DBL_ASTER);
                }
                return Token(TokenType::ASTER);
            }
            case '/': {
                if (input[++pos] == '/') {
                    ++pos;
                    return Token(TokenType::DBL_SLASH);
                }
                return Token(TokenType::SLASH);
            }
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

Token Lexer::extractNumber(bool negative) {
    size_t start = pos;
    while (pos < input.length() && (isdigit(input[pos]) || input[pos] == '.')) {
        ++pos;
    }
    if (negative) {
        return Token(TokenType::NUMBER, -(std::stod(input.substr(start, pos - start))));
    }
    return Token(TokenType::NUMBER, std::stod(input.substr(start, pos - start)));
}

Token Lexer::extractIdentifier() {
    size_t start = pos;
    while (pos < input.length() && (isalnum(input[pos]) || input[pos] == '_')) {
        ++pos;
    }
    return Token(TokenType::IDENTIFIER, input.substr(start, pos - start));
}

void Lexer::reset(const std::string& newInput) {
    input = newInput;
    pos = 0;
}
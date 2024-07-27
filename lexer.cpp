#include "lexer.h"

Token Lexer::getNextToken() {
    while (pos < length) {
        if (isspace(input[pos])) {
            ++pos;
            continue;
        }
        if (isdigit(input[pos])) {
            return extractNumber();
        }
        if (isalpha(input[pos])) {
            if (input.substr(pos, 4) == "true" && !std::isalnum(input[pos + 4])) {
                pos += 4;
                return Token(TokenType::TRUE, true);
            } else if (input.substr(pos, 5) == "false" && !std::isalnum(input[pos + 5])) {
                pos += 5;
                return Token(TokenType::FALSE, false);
            }
            return extractIdentifier();
        }
        switch (input[pos]) {
            case '=':
                if (input[++pos] == '=') {
                    ++pos;
                    return Token(TokenType::EQUAL);
                }
                return Token(TokenType::ASSIGN);
            case '&':
                ++pos;
                return Token(TokenType::AND);
            case '|':
                ++pos;
                return Token(TokenType::OR);
            case '!':
                if (input[++pos] == '=') {
                    ++pos;
                    return Token(TokenType::NOTEQ);
                }
                return Token(TokenType::NOT);
            case '>':
                if (input[++pos] == '=') {
                    ++pos;
                    return Token(TokenType::GTEQ);
                }
                return Token(TokenType::GT);
            case '<':
                if (input[++pos] == '=') {
                    ++pos;
                    return Token(TokenType::LTEQ);
                }
                return Token(TokenType::LT);
            case '"':
                return extractString();
            case '_':
                ++pos;
                return Token(TokenType::UNDERSCORE);
            case '+':
                ++pos;
                return Token(TokenType::PLUS);
            case '-': {
                if (isdigit(input[++pos])) return extractNumber(true);
                return Token(TokenType::MINUS);
            }
            case '%':
                ++pos;
                return Token(TokenType::MOD);
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
            case '(':
                ++pos;
                return Token(TokenType::LPAREN);
            case ')':
                ++pos;
                return Token(TokenType::RPAREN);
            default:
                throw std::runtime_error(std::string("Unexpected character: ") + input[pos]);
        }
    }
    return Token(TokenType::END);
}

Token Lexer::extractNumber(bool negative) {
    size_t start = pos;
    while (pos < length && (isdigit(input[pos]) || input[pos] == '.')) {
        ++pos;
    }
    double number = std::stod(input.substr(start, pos - start));
    return Token(TokenType::NUMBER, negative ? -number : number);
}

Token Lexer::extractIdentifier() {
    size_t start = pos;
    while (pos < length && (isalnum(input[pos]) || input[pos] == '_')) {
        ++pos;
    }
    return Token(TokenType::IDENTIFIER, input.substr(start, pos - start));
}

Token Lexer::extractString() {
    std::string str;
    ++pos; // Skip opening quote
    while (pos < length && input[pos] != '"') {
        str += input[pos++];
    }
    if (pos == length) {
        throw std::runtime_error("Unterminated string literal");
    }
    ++pos; // Skip closing quote
    return Token(TokenType::STRING, str);
}

void Lexer::reset(const std::string &newInput) {
    input = newInput;
    pos = 0;
    length = input.length();
}
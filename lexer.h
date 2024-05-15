#ifndef CPP_INTERPRETER_LEXER_H
#define CPP_INTERPRETER_LEXER_H

#include <iostream>
#include <string>
#include <cctype>
#include <vector>
#include <memory>


enum class TokenType {
    NUMBER,
    PLUS,
    MINUS,
    ASTER,
    SLASH,
    LPAREN,
    RPAREN,
    END
};

class Token {
public:
    TokenType type;
    double value;

    explicit Token(TokenType type, double value = 0.0) : type(type), value(value) {}
};


class Lexer {
    std::string input;
    size_t pos;

    Token extractNumber();

public:
    void reset(const std::string& newInput);
    explicit Lexer(const std::string& input) : input(input), pos(0) {}
    Token getNextToken();
};

#endif

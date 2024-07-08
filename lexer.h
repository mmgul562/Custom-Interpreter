#ifndef CPP_INTERPRETER_LEXER_H
#define CPP_INTERPRETER_LEXER_H

#include <iostream>
#include <string>
#include <cctype>
#include <utility>
#include <vector>
#include <memory>


enum class TokenType {
    NUMBER,
    IDENTIFIER,
    ASSIGN,
    PLUS,
    MINUS,
    ASTER,
    DBL_ASTER,
    SLASH,
    DBL_SLASH,
    LPAREN,
    RPAREN,
    END
};

class Token {
public:
    TokenType type;
    double value{};
    std::string name;

    Token(TokenType type, std::string name) : type(type), name(std::move(name)) {}
    explicit Token(TokenType type, double value = 0.0) : type(type), value(value) {}
};


class Lexer {
private:
    std::string input;

    Token extractNumber(bool negative=false);
    Token extractIdentifier();

public:
    size_t pos;

    void reset(const std::string& newInput);
    explicit Lexer(std::string input) : input(std::move(input)), pos(0) {}
    Token getNextToken();
};

#endif

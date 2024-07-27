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
    STRING,
    TRUE,
    FALSE,
    EQUAL,
    NOTEQ,
    GT,
    LT,
    GTEQ,
    LTEQ,
    NOT,
    AND,
    OR,
    ASSIGN,
    UNDERSCORE,
    PLUS,
    MINUS,
    MOD,
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

    union Value {
        double numberValue;
        bool boolValue;

        Value() : numberValue(0.0) {}
    } value;

    std::string stringValue;

    Token(TokenType type) : type(type) {}

    Token(TokenType type, double value) : type(type) { this->value.numberValue = value; }

    Token(TokenType type, bool value) : type(type) { this->value.boolValue = value; }

    Token(TokenType type, std::string value) : type(type), stringValue(std::move(value)) {}
};

class Lexer {
private:
    std::string input;

    Token extractNumber(bool negative = false);

    Token extractIdentifier();

    Token extractString();

public:
    size_t pos;
    size_t length;

    void reset(const std::string &newInput);

    explicit Lexer(std::string input) : input(std::move(input)), pos(0), length(input.length()) {}

    Token getNextToken();
};

#endif
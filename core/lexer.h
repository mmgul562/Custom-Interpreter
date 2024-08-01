#ifndef CPP_INTERPRETER_LEXER_H
#define CPP_INTERPRETER_LEXER_H

#include <iostream>
#include <string>
#include <vector>
#include <variant>
#include <memory>


using Value = std::variant<double, std::string, bool>;

inline void printValue(const Value& value) {
    std::visit([](const auto& v) { std::cout << v << std::endl; }, value);
}

enum class TokenType {
    // VALUES
    NUMBER,
    IDENTIFIER,
    STRING,
    TRUE,
    FALSE,
    // COMPARISON
    EQUAL,
    NOTEQ,
    GT,
    LT,
    GTEQ,
    LTEQ,
    // LOGICAL
    NOT,
    AND,
    OR,
    // MATHEMATICAL
    UNDERSCORE,
    PLUS,
    MINUS,
    MOD,
    ASTER,
    DBL_ASTER,
    SLASH,
    DBL_SLASH,
    // STATEMENTS
    ASSIGN,
    IF,
    ELSE,
    THEN,
    STOP,
    // GENERAL
    SEMICOLON,
    BACKSLASH,
    LPAREN,
    RPAREN,
    EOL,    // end of line
    END
};

std::string getTokenTypeName(TokenType type);

class Token {
public:
    TokenType type;
    Value value;

    explicit Token(TokenType type) : type(type) {}
    Token(TokenType type, Value value) : type(type), value(std::move(value)) {}
};

class Lexer {
private:
    std::string input;

    Token extractNumber(bool negative = false);
    Token extractIdentifier();
    Token extractString();

public:
    size_t pos;
    size_t oldPos;
    size_t length;

    explicit Lexer(std::string input) : input(std::move(input)), pos(0), oldPos(0), length(input.length()) {}

    void reset(const std::string &newInput);
    size_t isLineContinuation() const;
    Token getNextToken();
    TokenType peekNextTokenType();
};

#endif

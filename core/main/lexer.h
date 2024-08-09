#ifndef CPP_INTERPRETER_LEXER_H
#define CPP_INTERPRETER_LEXER_H

#include "../value.h"


enum class TokenType {
    // VALUES
    NUMBER,
    IDENTIFIER,
    STRING,
    TRUE,
    FALSE,
    // CONVERSION
    QUOTE,
    HASH,
    QMARK,
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
    FOR,
    IN,
    WHILE,
    DO,
    DEF,
    AS,
    BREAK,
    CONTINUE,
    RETURN,
    STOP,
    // GENERAL
    SEMICOLON,
    COLON,
    COMMA,
    DOT,
    DBL_DOT,
    LBRACE,
    RBRACE,
    LBRACKET,
    RBRACKET,
    LPAREN,
    RPAREN,
    EOL,    // end of line
    END
};

std::string getTypeName(TokenType type);

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

    Token extractNumber();
    Token extractIdentifier();
    Token extractString();

public:
    size_t pos;
    size_t length;

    explicit Lexer(std::string input) : input(std::move(input)), pos(0), length(input.length()) {}

    void reset(const std::string &newInput);
    Token getNextToken();
    TokenType peekNextTokenType();
};

#endif

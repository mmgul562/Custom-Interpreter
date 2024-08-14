#ifndef CPP_INTERPRETER_LEXER_H
#define CPP_INTERPRETER_LEXER_H

#include "../value.h"


enum class TokenType {
    // VALUES
    INT,
    FLOAT,
    IDENTIFIER,
    STRING,
    TRUE,
    FALSE,
    // TYPES
    INT_T,
    FLOAT_T,
    STR_T,
    BOOL_T,
    // COMPARISON
    EQUAL,
    NOTEQ,
    GT,
    LT,
    GTEQ,
    LTEQ,
    // LOGICAL
    QMARK,
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
    ASSIGN_NEW,
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
private:
    TokenType type;
    Value value;

public:
    explicit Token(TokenType type) : type(type) {}

    Token(TokenType type, Value value) : type(type), value(std::move(value)) {}

    TokenType getType() const { return type; }

    Value getValue() const { return value; }
};


class Lexer {
private:
    std::string input;
    size_t length;

public:
    size_t pos;

private:
    Token extractNumber();

    Token extractIdentifier();

    Token extractString();

public:
    explicit Lexer(std::string input) : input(std::move(input)), pos(0), length(input.length()) {}

    void reset(const std::string &newInput);

    Token getNextToken();

    TokenType peekNextTokenType();
};

#endif

#ifndef CPP_INTERPRETER_PARSER_H
#define CPP_INTERPRETER_PARSER_H

#include "ast.h"


class Parser {
private:
    Lexer &lexer;
    Token currentToken = lexer.getNextToken();

    std::unique_ptr<ASTNode> parseComparison();

    std::unique_ptr<ASTNode> parseExpression();

    std::unique_ptr<ASTNode> parseTerm();

    std::unique_ptr<ASTNode> parseFactor();

public:
    explicit Parser(Lexer &lexer) : lexer(lexer) {}

    void advanceToken();

    std::unique_ptr<ASTNode> parseStatement();
};

#endif
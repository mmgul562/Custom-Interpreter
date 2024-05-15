#ifndef CPP_INTERPRETER_PARSER_H
#define CPP_INTERPRETER_PARSER_H

#include "lexer.h"


enum class ASTNodeType {
    NUMBER,
    BINARY_OP
};

class ASTNode {
public:
    virtual ~ASTNode() = default;
    virtual double evaluate() const = 0;
};


class NumberNode : public ASTNode {
public:
    double value;

    explicit NumberNode(double value) : value(value) {}

    double evaluate() const override;
};


class BinaryOpNode : public ASTNode {
public:
    TokenType op;
    std::unique_ptr<ASTNode> left;
    std::unique_ptr<ASTNode> right;

    BinaryOpNode(TokenType op, std::unique_ptr<ASTNode> left, std::unique_ptr<ASTNode> right)
            : op(op), left(std::move(left)), right(std::move(right)) {}

    double evaluate() const override;
};


class Parser {
    Lexer& lexer;
    Token currentToken = lexer.getNextToken();

    void advanceToken();
    std::unique_ptr<ASTNode> parseFactor();
    std::unique_ptr<ASTNode> parseTerm();

public:
    explicit Parser(Lexer& lexer) : lexer(lexer) {}
    std::unique_ptr<ASTNode> parseExpression();
    void reset();
};


#endif

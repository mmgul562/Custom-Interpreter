#ifndef CPP_INTERPRETER_PARSER_H
#define CPP_INTERPRETER_PARSER_H

#include "lexer.h"
#include <cmath>
#include <unordered_map>
#include <utility>
#include <typeinfo>


//enum class ASTNodeType {
//    NUMBER,
//    BINARY_OP,
//    ASSIGN_NODE,
//};

bool isIdentified(const std::string &name);

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


class VariableNode : public ASTNode {
public:
    std::string name;
    std::shared_ptr<ASTNode> value;

    VariableNode(std::string name, std::shared_ptr<ASTNode> value);
    explicit VariableNode(std::string name);

    double evaluate() const override;
};


class Parser {
    Lexer& lexer;
    Token currentToken = lexer.getNextToken();

    std::unique_ptr<ASTNode> parseExpression();
    std::unique_ptr<ASTNode> parseTerm();
    std::unique_ptr<ASTNode> parseFactor();

public:
    explicit Parser(Lexer& lexer) : lexer(lexer) {}
    void advanceToken();
    std::unique_ptr<ASTNode> parseStatement();
};


#endif

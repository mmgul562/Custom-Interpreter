#ifndef CPP_INTERPRETER_AST_H
#define CPP_INTERPRETER_AST_H

#include "lexer.h"
#include <variant>
#include <cmath>
#include <utility>
#include <typeinfo>
#include <unordered_map>

using Value = std::variant<double, std::string, bool>;

bool isIdentified(const std::string &name);

class ASTNode {
public:
    virtual ~ASTNode() = default;

    virtual Value evaluate() const = 0;
};

class NumberNode : public ASTNode {
public:
    double value;

    explicit NumberNode(double value) : value(value) {}

    Value evaluate() const override;
};

class StringNode : public ASTNode {
public:
    std::string value;

    explicit StringNode(std::string value) : value(std::move(value)) {}

    Value evaluate() const override;
};

class BoolNode : public ASTNode {
public:
    bool value;

    explicit BoolNode(bool value) : value(value) {}

    Value evaluate() const override;
};

class BinaryOpNode : public ASTNode {
public:
    TokenType op;
    std::unique_ptr<ASTNode> left;
    std::unique_ptr<ASTNode> right;

    BinaryOpNode(TokenType op, std::unique_ptr<ASTNode> left, std::unique_ptr<ASTNode> right)
            : op(op), left(std::move(left)), right(std::move(right)) {}

    Value evaluate() const override;
};

class UnaryOpNode : public ASTNode {
public:
    TokenType op;
    std::unique_ptr<ASTNode> operand;
    UnaryOpNode(TokenType op, std::unique_ptr<ASTNode> operand)
            : op(op), operand(std::move(operand)) {}

    Value evaluate() const override;
};

class VariableNode : public ASTNode {
public:
    std::string name;
    Value value;

    VariableNode(std::string name, Value value);

    explicit VariableNode(std::string name);

    Value evaluate() const override;
};


#endif
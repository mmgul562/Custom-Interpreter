#ifndef CPP_INTERPRETER_AST_H
#define CPP_INTERPRETER_AST_H

#include "../scope.h"
#include "lexer.h"
#include <cmath>
#include <utility>
#include <typeinfo>


class ASTNode {
public:
    virtual ~ASTNode() = default;

    virtual Value evaluate(std::shared_ptr<Scope> scope) const = 0;
};

class NumberNode : public ASTNode {
public:
    double value;

    explicit NumberNode(double value) : value(value) {}

    Value evaluate(std::shared_ptr<Scope> scope) const override;
};

class StringNode : public ASTNode {
public:
    std::string value;

    explicit StringNode(std::string value) : value(std::move(value)) {}

    Value evaluate(std::shared_ptr<Scope> scope) const override;
};

class BoolNode : public ASTNode {
public:
    bool value;

    explicit BoolNode(bool value) : value(value) {}

    Value evaluate(std::shared_ptr<Scope> scope) const override;
};

class UnaryOpNode : public ASTNode {
public:
    TokenType op;
    std::unique_ptr<ASTNode> operand;

    UnaryOpNode(TokenType op, std::unique_ptr<ASTNode> operand)
            : op(op), operand(std::move(operand)) {}

    Value evaluate(std::shared_ptr<Scope> scope) const override;
};

class BinaryOpNode : public ASTNode {
public:
    TokenType op;
    std::unique_ptr<ASTNode> left;
    std::unique_ptr<ASTNode> right;

    BinaryOpNode(TokenType op, std::unique_ptr<ASTNode> left, std::unique_ptr<ASTNode> right)
            : op(op), left(std::move(left)), right(std::move(right)) {}

    Value evaluate(std::shared_ptr<Scope> scope) const override;
};

class AssignmentNode : public ASTNode {
public:
    std::string name;
    std::unique_ptr<ASTNode> valueNode;

    AssignmentNode(std::string name, std::unique_ptr<ASTNode> valueNode)
            : name(std::move(name)), valueNode(std::move(valueNode)) {}

    Value evaluate(std::shared_ptr<Scope> scope) const override;
};

class VariableNode : public ASTNode {
public:
    std::string name;
    std::unique_ptr<ASTNode> valueNode;

    VariableNode(std::string name, std::unique_ptr<ASTNode> valueNode)
            : name(std::move(name)), valueNode(std::move(valueNode)) {}

    explicit VariableNode(std::string name) : name(std::move(name)), valueNode(nullptr) {}

    Value evaluate(std::shared_ptr<Scope> scope) const override;
};

class ListNode : public ASTNode {
public:
    std::vector<std::unique_ptr<ASTNode>> elements;

    explicit ListNode(std::vector<std::unique_ptr<ASTNode>> elements)
        : elements(std::move(elements)) {}

    Value evaluate(std::shared_ptr<Scope> scope) const override;
};

class IndexAccessNode : public ASTNode {
public:
    std::unique_ptr<ASTNode> container;
    std::unique_ptr<ASTNode> index;

    IndexAccessNode(std::unique_ptr<ASTNode> list, std::unique_ptr<ASTNode> index)
        : container(std::move(list)), index(std::move(index)) {}

    Value evaluate(std::shared_ptr<Scope> scope) const override;
};

class IndexAssignmentNode : public ASTNode {
public:
    std::unique_ptr<ASTNode> access;
    std::unique_ptr<ASTNode> value;

    IndexAssignmentNode(std::unique_ptr<ASTNode> access, std::unique_ptr<ASTNode> value)
        : access(std::move(access)), value(std::move(value)) {}

    Value evaluate(std::shared_ptr<Scope> scope) const override;
};

class DictNode : public ASTNode {
public:
    std::vector<std::pair<std::unique_ptr<ASTNode>, std::unique_ptr<ASTNode>>> elements;

    explicit DictNode(std::vector<std::pair<std::unique_ptr<ASTNode>, std::unique_ptr<ASTNode>>> elements)
        : elements(std::move(elements)) {}

    Value evaluate(std::shared_ptr<Scope> scope) const override;
};

void updateNestedContainer(const std::unique_ptr<ASTNode>& node, const Value& updatedValue, std::shared_ptr<Scope> scope);

class ContainerMethodCallNode : public ASTNode {
public:
    std::unique_ptr<ASTNode> container;
    std::string methodName;
    std::vector<std::unique_ptr<ASTNode>> arguments;

    ContainerMethodCallNode(std::unique_ptr<ASTNode> container, std::string methodName, std::vector<std::unique_ptr<ASTNode>> arguments)
            : container(std::move(container)), methodName(std::move(methodName)), arguments(std::move(arguments)) {}

    Value evaluate(std::shared_ptr<Scope> scope) const override;
};

class BlockNode : public ASTNode {
public:
    std::vector<std::unique_ptr<ASTNode>> statements;

    explicit BlockNode(std::vector<std::unique_ptr<ASTNode>> statements)
            : statements(std::move(statements)) {}

    Value evaluate(std::shared_ptr<Scope> scope) const override;
};

class IfElseNode : public ASTNode {
public:
    std::unique_ptr<ASTNode> condition;
    std::unique_ptr<BlockNode> ifBlock;
    std::unique_ptr<BlockNode> elseBlock;

    IfElseNode(std::unique_ptr<ASTNode> condition,
               std::unique_ptr<BlockNode> ifBlock,
               std::unique_ptr<BlockNode> elseBlock = nullptr)
            : condition(std::move(condition)),
              ifBlock(std::move(ifBlock)),
              elseBlock(std::move(elseBlock)) {}

    Value evaluate(std::shared_ptr<Scope> scope) const override;
};

#endif
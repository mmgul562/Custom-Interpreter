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

    virtual std::unique_ptr<ASTNode> clone() const = 0;
    virtual Value evaluate(std::shared_ptr<Scope> scope) const = 0;
};

class NumberNode : public ASTNode {
public:
    double value;

    explicit NumberNode(double value) : value(value) {}

    std::unique_ptr<ASTNode> clone() const override {
        return std::make_unique<NumberNode>(*this);
    }
    Value evaluate(std::shared_ptr<Scope> scope) const override;
};

class StringNode : public ASTNode {
public:
    std::string value;

    explicit StringNode(std::string value) : value(std::move(value)) {}

    std::unique_ptr<ASTNode> clone() const override {
        return std::make_unique<StringNode>(*this);
    }
    Value evaluate(std::shared_ptr<Scope> scope) const override;
};

class BoolNode : public ASTNode {
public:
    bool value;

    explicit BoolNode(bool value) : value(value) {}

    std::unique_ptr<ASTNode> clone() const override {
        return std::make_unique<BoolNode>(*this);
    }
    Value evaluate(std::shared_ptr<Scope> scope) const override;
};

class UnaryOpNode : public ASTNode {
public:
    TokenType op;
    std::unique_ptr<ASTNode> operand;

    UnaryOpNode(TokenType op, std::unique_ptr<ASTNode> operand)
            : op(op), operand(std::move(operand)) {}

    std::unique_ptr<ASTNode> clone() const override {
        return std::make_unique<UnaryOpNode>(op, operand->clone());
    }
    Value evaluate(std::shared_ptr<Scope> scope) const override;
};

class BinaryOpNode : public ASTNode {
public:
    TokenType op;
    std::unique_ptr<ASTNode> left;
    std::unique_ptr<ASTNode> right;

    BinaryOpNode(TokenType op, std::unique_ptr<ASTNode> left, std::unique_ptr<ASTNode> right)
            : op(op), left(std::move(left)), right(std::move(right)) {}

    std::unique_ptr<ASTNode> clone() const override {
        return std::make_unique<BinaryOpNode>(op, left->clone(), right->clone());
    }
    Value evaluate(std::shared_ptr<Scope> scope) const override;
};

class AssignmentNode : public ASTNode {
public:
    std::string name;
    std::unique_ptr<ASTNode> valueNode;

    AssignmentNode(std::string name, std::unique_ptr<ASTNode> valueNode)
            : name(std::move(name)), valueNode(std::move(valueNode)) {}

    std::unique_ptr<ASTNode> clone() const override {
        return std::make_unique<AssignmentNode>(name, valueNode->clone());
    }
    Value evaluate(std::shared_ptr<Scope> scope) const override;
};

class VariableNode : public ASTNode {
public:
    std::string name;
    std::unique_ptr<ASTNode> valueNode;

    VariableNode(std::string name, std::unique_ptr<ASTNode> valueNode)
            : name(std::move(name)), valueNode(std::move(valueNode)) {}

    explicit VariableNode(std::string name) : name(std::move(name)), valueNode(nullptr) {}

    std::unique_ptr<ASTNode> clone() const override {
        if (valueNode) {
            return std::make_unique<VariableNode>(name, valueNode->clone());
        } else {
            return std::make_unique<VariableNode>(name);
        }
    }
    Value evaluate(std::shared_ptr<Scope> scope) const override;
};

class ListNode : public ASTNode {
public:
    std::vector<std::unique_ptr<ASTNode>> elements;

    explicit ListNode(std::vector<std::unique_ptr<ASTNode>> elements)
            : elements(std::move(elements)) {}

    std::unique_ptr<ASTNode> clone() const override {
        std::vector<std::unique_ptr<ASTNode>> clonedElements;
        for (const auto& element : elements) {
            clonedElements.push_back(element->clone());
        }
        return std::make_unique<ListNode>(std::move(clonedElements));
    }
    Value evaluate(std::shared_ptr<Scope> scope) const override;
};

class DictNode : public ASTNode {
public:
    std::vector<std::pair<std::unique_ptr<ASTNode>, std::unique_ptr<ASTNode>>> elements;

    explicit DictNode(std::vector<std::pair<std::unique_ptr<ASTNode>, std::unique_ptr<ASTNode>>> elements)
            : elements(std::move(elements)) {}

    std::unique_ptr<ASTNode> clone() const override {
        std::vector<std::pair<std::unique_ptr<ASTNode>, std::unique_ptr<ASTNode>>> clonedElements;
        for (const auto& element : elements) {
            clonedElements.emplace_back(element.first->clone(), element.second->clone());
        }
        return std::make_unique<DictNode>(std::move(clonedElements));
    }
    Value evaluate(std::shared_ptr<Scope> scope) const override;
};

class IndexAccessNode : public ASTNode {
public:
    std::unique_ptr<ASTNode> container;
    std::unique_ptr<ASTNode> index;

    IndexAccessNode(std::unique_ptr<ASTNode> list, std::unique_ptr<ASTNode> index)
            : container(std::move(list)), index(std::move(index)) {}

    std::unique_ptr<ASTNode> clone() const override {
        return std::make_unique<IndexAccessNode>(container->clone(), index->clone());
    }
    Value evaluate(std::shared_ptr<Scope> scope) const override;
};

class IndexAssignmentNode : public ASTNode {
public:
    std::unique_ptr<ASTNode> access;
    std::unique_ptr<ASTNode> value;

    IndexAssignmentNode(std::unique_ptr<ASTNode> access, std::unique_ptr<ASTNode> value)
            : access(std::move(access)), value(std::move(value)) {}

    std::unique_ptr<ASTNode> clone() const override {
        return std::make_unique<IndexAssignmentNode>(access->clone(), value->clone());
    }
    Value evaluate(std::shared_ptr<Scope> scope) const override;
};

void
updateNestedContainer(const std::unique_ptr<ASTNode> &node, const Value &updatedValue, std::shared_ptr<Scope> scope);

class ContainerMethodCallNode : public ASTNode {
public:
    std::unique_ptr<ASTNode> container;
    std::string methodName;
    std::vector<std::unique_ptr<ASTNode>> arguments;

    ContainerMethodCallNode(std::unique_ptr<ASTNode> container, std::string methodName,
                            std::vector<std::unique_ptr<ASTNode>> arguments)
            : container(std::move(container)), methodName(std::move(methodName)), arguments(std::move(arguments)) {}

    std::unique_ptr<ASTNode> clone() const override {
        std::vector<std::unique_ptr<ASTNode>> clonedArguments;
        for (const auto& arg : arguments) {
            clonedArguments.push_back(arg->clone());
        }
        return std::make_unique<ContainerMethodCallNode>(container->clone(), methodName, std::move(clonedArguments));
    }
    Value evaluate(std::shared_ptr<Scope> scope) const override;
};

class BlockNode : public ASTNode {
public:
    std::vector<std::unique_ptr<ASTNode>> statements;

    explicit BlockNode(std::vector<std::unique_ptr<ASTNode>> statements)
            : statements(std::move(statements)) {}
    BlockNode(const BlockNode& other) {
        for (const auto& stmt : other.statements) {
            statements.push_back(stmt->clone());
        }
    }

    std::unique_ptr<ASTNode> clone() const override {
        return std::make_unique<BlockNode>(*this);
    }
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

    std::unique_ptr<ASTNode> clone() const override {
        return std::make_unique<IfElseNode>(condition->clone(), std::make_unique<BlockNode>(*ifBlock), elseBlock ? std::make_unique<BlockNode>(*elseBlock) : nullptr);
    }
    Value evaluate(std::shared_ptr<Scope> scope) const override;
};

class ForLoopNode : public ASTNode {
public:
    std::string variableName;
    std::unique_ptr<ASTNode> startExpr;
    std::unique_ptr<ASTNode> endExpr;
    std::unique_ptr<ASTNode> stepExpr;
    std::unique_ptr<BlockNode> body;
    bool isRangeLoop;

    ForLoopNode(std::string variableName, std::unique_ptr<ASTNode> startExpr,
                std::unique_ptr<ASTNode> endExpr, std::unique_ptr<ASTNode> stepExpr,
                std::unique_ptr<BlockNode> body, bool isRangeLoop)
            : variableName(std::move(variableName)), startExpr(std::move(startExpr)),
              endExpr(std::move(endExpr)), stepExpr(std::move(stepExpr)),
              body(std::move(body)), isRangeLoop(isRangeLoop) {}


    std::unique_ptr<ASTNode> clone() const override {
        return std::make_unique<ForLoopNode>(variableName, startExpr->clone(), endExpr->clone(), stepExpr ? stepExpr->clone() : nullptr, std::make_unique<BlockNode>(*body), isRangeLoop);
    }
    Value evaluate(std::shared_ptr<Scope> scope) const override;
};

class WhileLoopNode : public ASTNode {
public:
    std::unique_ptr<ASTNode> condition;
    std::unique_ptr<BlockNode> body;

    WhileLoopNode(std::unique_ptr<ASTNode> boolExpr,
                  std::unique_ptr<BlockNode> body)
            : condition(std::move(boolExpr)),
              body(std::move(body)) {}

    std::unique_ptr<ASTNode> clone() const override {
        return std::make_unique<WhileLoopNode>(condition->clone(), std::make_unique<BlockNode>(*body));
    }
    Value evaluate(std::shared_ptr<Scope> scope) const override;
};

class ControlFlowNode : public ASTNode {
public:
    bool isBreak;   // false for continue

    explicit ControlFlowNode(bool isBreak) : isBreak(isBreak) {}

    std::unique_ptr<ASTNode> clone() const override {
        return std::make_unique<ControlFlowNode>(*this);
    }
    Value evaluate(std::shared_ptr<Scope> scope) const override;
};

class ReturnNode : public ASTNode {
public:
    std::unique_ptr<ASTNode> expression;

    explicit ReturnNode(std::unique_ptr<ASTNode> expr) : expression(std::move(expr)) {}

    std::unique_ptr<ASTNode> clone() const override {
        return std::make_unique<ReturnNode>(expression ? expression->clone() : nullptr);
    }
    Value evaluate(std::shared_ptr<Scope> scope) const override;
};

class FunctionDeclarationNode : public ASTNode {
public:
    std::string name;
    std::vector<std::string> parameters;
    std::unique_ptr<BlockNode> body;

    FunctionDeclarationNode(std::string name, std::vector<std::string> parameters, std::unique_ptr<BlockNode> body)
        : name(std::move(name)), parameters(std::move(parameters)), body(std::move(body)) {}
    FunctionDeclarationNode(const FunctionDeclarationNode& other)
        : name(other.name), parameters(other.parameters), body(std::make_unique<BlockNode>(*other.body)) {}

    std::unique_ptr<ASTNode> clone() const override {
        return std::make_unique<FunctionDeclarationNode>(*this);
    }
    Value evaluate(std::shared_ptr<Scope> scope) const override;
};

class FunctionCallNode : public ASTNode {
public:
    std::string name;
    std::vector<std::unique_ptr<ASTNode>> arguments;

    FunctionCallNode(std::string name, std::vector<std::unique_ptr<ASTNode>> arguments)
        : name(std::move(name)), arguments(std::move(arguments)) {}

    std::unique_ptr<ASTNode> clone() const override {
        std::vector<std::unique_ptr<ASTNode>> clonedArguments;
        for (const auto& arg : arguments) {
            clonedArguments.push_back(arg->clone());
        }
        return std::make_unique<FunctionCallNode>(name, std::move(clonedArguments));
    }
    Value evaluate(std::shared_ptr<Scope> scope) const override;
};

#endif
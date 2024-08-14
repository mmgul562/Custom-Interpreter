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


class FloatNode : public ASTNode {
private:
    double value;

public:
    explicit FloatNode(double value) : value(value) {}

    std::unique_ptr<ASTNode> clone() const override;

    Value evaluate(std::shared_ptr<Scope> scope) const override;
};


class IntNode : public ASTNode {
private:
    long value;

public:
    explicit IntNode(long value) : value(value) {}

    std::unique_ptr<ASTNode> clone() const override;

    Value evaluate(std::shared_ptr<Scope> scope) const override;
};


class StringNode : public ASTNode {
private:
    std::string value;

public:
    explicit StringNode(std::string value) : value(std::move(value)) {}

    std::unique_ptr<ASTNode> clone() const override;

    Value evaluate(std::shared_ptr<Scope> scope) const override;
};


class BoolNode : public ASTNode {
private:
    bool value;

public:
    explicit BoolNode(bool value) : value(value) {}

    std::unique_ptr<ASTNode> clone() const override;

    Value evaluate(std::shared_ptr<Scope> scope) const override;
};


class TypeCastNode : public ASTNode {
private:
    TokenType type;
    std::unique_ptr<ASTNode> var;

public:
    TypeCastNode(TokenType type, std::unique_ptr<ASTNode> var) : type(type), var(std::move(var)) {}

    std::unique_ptr<ASTNode> clone() const override;

    Value evaluate(std::shared_ptr<Scope> scope) const override;
};


class UnaryOpNode : public ASTNode {
private:
    TokenType op;
    std::unique_ptr<ASTNode> operand;

public:
    UnaryOpNode(TokenType op, std::unique_ptr<ASTNode> operand)
            : op(op), operand(std::move(operand)) {}

    std::unique_ptr<ASTNode> clone() const override;

    Value evaluate(std::shared_ptr<Scope> scope) const override;
};


class BinaryOpVisitor {
private:
    TokenType op;

public:
    explicit BinaryOpVisitor(TokenType op) : op(op) {}

    Value operator()(double lhs, double rhs) const;

    Value operator()(long lhs, long rhs) const;

    Value operator()(const std::string &lhs, const std::string &rhs) const;

    Value operator()(bool lhs, bool rhs) const;

    template<typename T, typename U>
    Value operator()(const T &, const U &) const;
};


class BinaryOpNode : public ASTNode {
private:
    TokenType op;
    std::unique_ptr<ASTNode> left;
    std::unique_ptr<ASTNode> right;

public:
    BinaryOpNode(TokenType op, std::unique_ptr<ASTNode> left, std::unique_ptr<ASTNode> right)
            : op(op), left(std::move(left)), right(std::move(right)) {}

    std::unique_ptr<ASTNode> clone() const override;

    Value evaluate(std::shared_ptr<Scope> scope) const override;
};


class AssignmentNode : public ASTNode {
private:
    std::string name;
    bool reassign;
    std::unique_ptr<ASTNode> valueNode;

public:
    AssignmentNode(std::string name, bool reassign, std::unique_ptr<ASTNode> valueNode)
            : name(std::move(name)), reassign(reassign), valueNode(std::move(valueNode)) {}

    std::unique_ptr<ASTNode> clone() const override;

    Value evaluate(std::shared_ptr<Scope> scope) const override;
};


class VariableNode : public ASTNode {
private:
    std::string name;
    std::unique_ptr<ASTNode> valueNode;

public:
    explicit VariableNode(std::string name) : name(std::move(name)), valueNode(nullptr) {}

    std::unique_ptr<ASTNode> clone() const override;

    Value evaluate(std::shared_ptr<Scope> scope) const override;

    const std::string &getName() const { return name; }
};


class ListNode : public ASTNode {
private:
    std::vector<std::unique_ptr<ASTNode>> elements;

public:
    explicit ListNode(std::vector<std::unique_ptr<ASTNode>> elements)
            : elements(std::move(elements)) {}

    std::unique_ptr<ASTNode> clone() const override;

    Value evaluate(std::shared_ptr<Scope> scope) const override;
};


class DictNode : public ASTNode {
private:
    std::vector<std::pair<std::unique_ptr<ASTNode>, std::unique_ptr<ASTNode>>> elements;

public:
    explicit DictNode(std::vector<std::pair<std::unique_ptr<ASTNode>, std::unique_ptr<ASTNode>>> elements)
            : elements(std::move(elements)) {}

    std::unique_ptr<ASTNode> clone() const override;

    Value evaluate(std::shared_ptr<Scope> scope) const override;
};


class IndexAccessNode : public ASTNode {
private:
    std::unique_ptr<ASTNode> container;
    std::unique_ptr<ASTNode> index;

public:
    IndexAccessNode(std::unique_ptr<ASTNode> list, std::unique_ptr<ASTNode> index)
            : container(std::move(list)), index(std::move(index)) {}

    std::unique_ptr<ASTNode> clone() const override;

    Value evaluate(std::shared_ptr<Scope> scope) const override;

    const std::unique_ptr<ASTNode> &getContainer() const { return container; }

    const std::unique_ptr<ASTNode> &getIndex() const { return index; }
};


void updateNestedContainer(const std::unique_ptr<ASTNode> &node, const Value &updated, std::shared_ptr<Scope> scope);


class IndexAssignmentNode : public ASTNode {
private:
    std::unique_ptr<ASTNode> access;
    std::unique_ptr<ASTNode> value;

public:
    IndexAssignmentNode(std::unique_ptr<ASTNode> access, std::unique_ptr<ASTNode> value)
            : access(std::move(access)), value(std::move(value)) {}

    std::unique_ptr<ASTNode> clone() const override;

    Value evaluate(std::shared_ptr<Scope> scope) const override;
};


class MethodCallNode : public ASTNode {
private:
    std::unique_ptr<ASTNode> container;
    std::string methodName;
    std::vector<std::unique_ptr<ASTNode>> arguments;

public:
    MethodCallNode(std::unique_ptr<ASTNode> container, std::string methodName,
                   std::vector<std::unique_ptr<ASTNode>> arguments)
            : container(std::move(container)), methodName(std::move(methodName)), arguments(std::move(arguments)) {}

    std::unique_ptr<ASTNode> clone() const override;

    Value evaluate(std::shared_ptr<Scope> scope) const override;
};


class BlockNode : public ASTNode {
private:
    std::vector<std::unique_ptr<ASTNode>> statements;

public:
    explicit BlockNode(std::vector<std::unique_ptr<ASTNode>> statements)
            : statements(std::move(statements)) {}

    BlockNode(const BlockNode &other) {
        for (const auto &stmt: other.statements) {
            statements.push_back(stmt->clone());
        }
    }

    std::unique_ptr<ASTNode> clone() const override;

    Value evaluate(std::shared_ptr<Scope> scope) const override;
};


class IfElseNode : public ASTNode {
private:
    std::unique_ptr<ASTNode> condition;
    std::unique_ptr<BlockNode> ifBlock;
    std::unique_ptr<BlockNode> elseBlock;

public:
    IfElseNode(std::unique_ptr<ASTNode> condition,
               std::unique_ptr<BlockNode> ifBlock,
               std::unique_ptr<BlockNode> elseBlock = nullptr)
            : condition(std::move(condition)),
              ifBlock(std::move(ifBlock)),
              elseBlock(std::move(elseBlock)) {}

    std::unique_ptr<ASTNode> clone() const override;

    Value evaluate(std::shared_ptr<Scope> scope) const override;
};


class ForLoopNode : public ASTNode {
private:
    std::string variableName;
    std::unique_ptr<ASTNode> startExpr;
    std::unique_ptr<ASTNode> endExpr;
    std::unique_ptr<ASTNode> stepExpr;
    std::unique_ptr<BlockNode> body;
    bool isRangeLoop;

public:
    ForLoopNode(std::string variableName, std::unique_ptr<ASTNode> startExpr,
                std::unique_ptr<ASTNode> endExpr, std::unique_ptr<ASTNode> stepExpr,
                std::unique_ptr<BlockNode> body, bool isRangeLoop)
            : variableName(std::move(variableName)), startExpr(std::move(startExpr)),
              endExpr(std::move(endExpr)), stepExpr(std::move(stepExpr)),
              body(std::move(body)), isRangeLoop(isRangeLoop) {}

    std::unique_ptr<ASTNode> clone() const override;

    Value evaluate(std::shared_ptr<Scope> scope) const override;
};


class WhileLoopNode : public ASTNode {
private:
    std::unique_ptr<ASTNode> condition;
    std::unique_ptr<BlockNode> body;

public:
    WhileLoopNode(std::unique_ptr<ASTNode> boolExpr,
                  std::unique_ptr<BlockNode> body)
            : condition(std::move(boolExpr)),
              body(std::move(body)) {}

    std::unique_ptr<ASTNode> clone() const override;

    Value evaluate(std::shared_ptr<Scope> scope) const override;
};


class ControlFlowNode : public ASTNode {
private:
    bool isBreak;   // false for continue

public:
    explicit ControlFlowNode(bool isBreak) : isBreak(isBreak) {}

    std::unique_ptr<ASTNode> clone() const override;

    Value evaluate(std::shared_ptr<Scope> scope) const override;
};


class ReturnNode : public ASTNode {
private:
    std::unique_ptr<ASTNode> expression;

public:
    explicit ReturnNode(std::unique_ptr<ASTNode> expr) : expression(std::move(expr)) {}

    std::unique_ptr<ASTNode> clone() const override;

    Value evaluate(std::shared_ptr<Scope> scope) const override;
};


class FunctionDeclarationNode : public ASTNode {
private:
    std::string name;
    std::vector<std::string> parameters;
    bool hasArgs;
    std::unique_ptr<BlockNode> body;

public:
    FunctionDeclarationNode(std::string name, std::vector<std::string> parameters, bool hasArgs,
                            std::unique_ptr<BlockNode> body)
            : name(std::move(name)), parameters(std::move(parameters)), hasArgs(hasArgs), body(std::move(body)) {}

    FunctionDeclarationNode(const FunctionDeclarationNode &other)
            : name(other.name), parameters(other.parameters), hasArgs(other.hasArgs),
              body(std::make_unique<BlockNode>(*other.body)) {}

    std::unique_ptr<ASTNode> clone() const override;

    Value evaluate(std::shared_ptr<Scope> scope) const override;

    bool getHasArgs() const { return hasArgs; }

    const std::vector<std::string> &getParameters() const { return parameters; }

    const std::unique_ptr<BlockNode> &getBody() const { return body; }
};


class FunctionCallNode : public ASTNode {
private:
    std::string name;
    std::vector<std::unique_ptr<ASTNode>> arguments;

public:
    FunctionCallNode(std::string name, std::vector<std::unique_ptr<ASTNode>> arguments)
            : name(std::move(name)), arguments(std::move(arguments)) {}

    std::unique_ptr<ASTNode> clone() const override;

    Value evaluate(std::shared_ptr<Scope> scope) const override;
};


#endif
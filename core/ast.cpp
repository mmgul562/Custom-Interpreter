#include "ast.h"


Value NumberNode::evaluate(std::shared_ptr<Scope> scope) const {
    return value;
}

Value StringNode::evaluate(std::shared_ptr<Scope> scope) const {
    return value;
}

Value BoolNode::evaluate(std::shared_ptr<Scope> scope) const {
    return value;
}

Value UnaryOpNode::evaluate(std::shared_ptr<Scope> scope) const {
    auto operandValue = operand->evaluate(scope);
    switch (op) {
        case TokenType::NOT:
            if (std::holds_alternative<bool>(operandValue)) {
                return !std::get<bool>(operandValue);
            }
            throw std::runtime_error("NOT operation can only be applied to boolean values");
        case TokenType::UNDERSCORE:
            if (std::holds_alternative<double>(operandValue)) {
                return std::abs(std::get<double>(operandValue));
            }
        default:
            throw std::runtime_error("Unexpected unary operator: " + getTokenTypeName(op));
    }
}

Value BinaryOpNode::evaluate(std::shared_ptr<Scope> scope) const {
    auto leftValue = left->evaluate(scope);
    auto rightValue = right->evaluate(scope);

    if (std::holds_alternative<double>(leftValue) && std::holds_alternative<double>(rightValue)) {
        double lhs = std::get<double>(leftValue);
        double rhs = std::get<double>(rightValue);

        switch (op) {
            case TokenType::EQUAL:
                return lhs == rhs;
            case TokenType::NOTEQ:
                return lhs != rhs;
            case TokenType::GT:
                return lhs > rhs;
            case TokenType::GTEQ:
                return lhs >= rhs;
            case TokenType::LT:
                return lhs < rhs;
            case TokenType::LTEQ:
                return lhs <= rhs;
            case TokenType::PLUS:
                return lhs + rhs;
            case TokenType::MINUS:
                return lhs - rhs;
            case TokenType::MOD:
                if (lhs == int(lhs) && rhs == int(rhs))
                    return double(int(lhs) % int(rhs));
                throw std::runtime_error("Modulus operator '%' should be used with integers only");
            case TokenType::ASTER:
                return lhs * rhs;
            case TokenType::DBL_ASTER:
                return std::pow(lhs, rhs);
            case TokenType::SLASH:
                return lhs / rhs;
            case TokenType::DBL_SLASH:
                return std::floor(lhs / rhs);
            default:
                throw std::runtime_error("Unexpected operator for number values: " + getTokenTypeName(op));
        }
    } else if (std::holds_alternative<std::string>(leftValue) && std::holds_alternative<std::string>(rightValue)) {
        std::string lhs = std::get<std::string>(leftValue);
        std::string rhs = std::get<std::string>(rightValue);

        switch (op) {
            case TokenType::PLUS:
                return lhs + rhs;
            case TokenType::EQUAL:
                return lhs == rhs;
            case TokenType::NOTEQ:
                return lhs != rhs;
            case TokenType::GT:
                return lhs.length() > rhs.length();
            case TokenType::GTEQ:
                return lhs.length() >= rhs.length();
            case TokenType::LT:
                return lhs.length() < rhs.length();
            case TokenType::LTEQ:
                return lhs.length() <= rhs.length();
            default:
                throw std::runtime_error("Unexpected operator for string values: " + getTokenTypeName(op));
        }
    } else if (std::holds_alternative<bool>(leftValue) && std::holds_alternative<bool>(rightValue)) {
        bool lhs = std::get<bool>(leftValue);
        bool rhs = std::get<bool>(rightValue);
        switch (op) {
            case TokenType::EQUAL:
                return lhs == rhs;
            case TokenType::NOTEQ:
                return lhs != rhs;
            case TokenType::AND:
                return lhs && rhs;
            case TokenType::OR:
                return lhs || rhs;
            default:
                throw std::runtime_error("Unexpected operator for boolean values: " + getTokenTypeName(op));
        }
    }
    throw std::runtime_error("Unsupported binary operation: " + getTokenTypeName(op));
}

Value AssignmentNode::evaluate(std::shared_ptr<Scope> scope) const {
    Value value = valueNode->evaluate(scope);
    if (scope->hasVariableInCurrentOrParentScope(name)) {
        scope->assignVariable(name, value);
    } else {
        scope->setVariable(name, value);
    }
    return value;
}

Value VariableNode::evaluate(std::shared_ptr<Scope> scope) const {
    if (valueNode) {
        Value value = valueNode->evaluate(scope);
        if (scope->hasVariableInCurrentOrParentScope(name)) {
            scope->assignVariable(name, value);
        } else {
            scope->setVariable(name, value);
        }
        return value;
    } else {
        if (!scope->hasVariableInCurrentOrParentScope(name)) {
            throw std::runtime_error("Undefined variable: " + name);
        }
        return scope->getVariable(name);
    }
}

Value BlockNode::evaluate(std::shared_ptr<Scope> scope) const {
    auto blockScope = std::make_shared<Scope>(scope);
    Value lastValue;
    for (const auto& statement : statements) {
        lastValue = statement->evaluate(blockScope);
    }
    return lastValue;
}

Value IfElseNode::evaluate(std::shared_ptr<Scope> scope) const {
    Value cond = condition->evaluate(scope);
    if (std::holds_alternative<bool>(cond)) {
        if (std::get<bool>(condition->evaluate(scope))) {
            return ifBlock->evaluate(scope);
        } else if (elseBlock) {
            return elseBlock->evaluate(scope);
        }
    } else {
        throw std::runtime_error("Expected boolean expression after 'if'");
    }
    return {};
}

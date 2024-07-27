#include "ast.h"


std::unordered_map<std::string, Value> variableTable;

bool isIdentified(const std::string &name) {
    return variableTable.find(name) != variableTable.end();
}

Value NumberNode::evaluate() const {
    return value;
}

Value StringNode::evaluate() const {
    return value;
}

Value BoolNode::evaluate() const {
    return value;
}

Value UnaryOpNode::evaluate() const {
    auto operandValue = operand->evaluate();
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
            throw std::runtime_error("Unexpected unary operator");
    }
}

Value BinaryOpNode::evaluate() const {
    auto leftValue = left->evaluate();
    auto rightValue = right->evaluate();

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
                throw std::runtime_error("Unexpected binary operator");
        }
    } else if (std::holds_alternative<std::string>(leftValue) && std::holds_alternative<std::string>(rightValue) &&
               op == TokenType::PLUS) {
        return std::get<std::string>(leftValue) + std::get<std::string>(rightValue);
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
                throw std::runtime_error("Unexpected operator for boolean values");
        }
    }
    throw std::runtime_error("Unsupported binary operation");
}

VariableNode::VariableNode(std::string name,
                           Value value) : name(std::move(name)), value(std::move(value)) {
    variableTable[this->name] = this->value;
}

VariableNode::VariableNode(std::string name) : name(std::move(name)) {
    value = variableTable[this->name];
}

Value VariableNode::evaluate() const {
    return value;
}

#include "ast.h"


Value NumberNode::evaluate(std::shared_ptr<Scope> scope) const {
    return Value(value);
}

Value StringNode::evaluate(std::shared_ptr<Scope> scope) const {
    return Value(value);
}

Value BoolNode::evaluate(std::shared_ptr<Scope> scope) const {
    return Value(value);
}

Value UnaryOpNode::evaluate(std::shared_ptr<Scope> scope) const {
    auto operandValue = operand->evaluate(scope);
    switch (op) {
        case TokenType::NOT:
            if (operandValue.isBase() && std::holds_alternative<bool>(operandValue.asBase())) {
                return Value(!std::get<bool>(operandValue.asBase()));
            }
            throw TypeError("NOT operation can only be applied to boolean values");
        case TokenType::UNDERSCORE:
            if (operandValue.isBase() && std::holds_alternative<double>(operandValue.asBase())) {
                return Value(std::abs(std::get<double>(operandValue.asBase())));
            }
            throw TypeError("UNDERSCORE (absolute) operator can only be applied to numbers");
        default:
            throw InterpreterError("Unexpected unary operator: " + getTokenTypeName(op));
    }
}

Value BinaryOpNode::evaluate(std::shared_ptr<Scope> scope) const {
    auto leftValue = left->evaluate(scope);
    auto rightValue = right->evaluate(scope);

    if (leftValue.isBase() && rightValue.isBase()) {
        const auto& leftBase = leftValue.asBase();
        const auto& rightBase = rightValue.asBase();

        if (std::holds_alternative<double>(leftBase) && std::holds_alternative<double>(rightBase)) {
            double lhs = std::get<double>(leftBase);
            double rhs = std::get<double>(rightBase);

            switch (op) {
                case TokenType::EQUAL: return Value(lhs == rhs);
                case TokenType::NOTEQ: return Value(lhs != rhs);
                case TokenType::GT: return Value(lhs > rhs);
                case TokenType::GTEQ: return Value(lhs >= rhs);
                case TokenType::LT: return Value(lhs < rhs);
                case TokenType::LTEQ: return Value(lhs <= rhs);
                case TokenType::PLUS: return Value(lhs + rhs);
                case TokenType::MINUS: return Value(lhs - rhs);
                case TokenType::MOD: return Value(std::fmod(lhs, rhs));
                case TokenType::ASTER: return Value(lhs * rhs);
                case TokenType::DBL_ASTER: return Value(std::pow(lhs, rhs));
                case TokenType::SLASH: return Value(lhs / rhs);
                case TokenType::DBL_SLASH: return Value(std::floor(lhs / rhs));
                default:
                    throw InterpreterError("Unexpected operator for number values: " + getTokenTypeName(op));
            }
        } else if (std::holds_alternative<std::string>(leftBase) && std::holds_alternative<std::string>(rightBase)) {
            const auto& lhs = std::get<std::string>(leftBase);
            const auto& rhs = std::get<std::string>(rightBase);

            switch (op) {
                case TokenType::PLUS: return Value(lhs + rhs);
                case TokenType::EQUAL: return Value(lhs == rhs);
                case TokenType::NOTEQ: return Value(lhs != rhs);
                case TokenType::GT: return Value(lhs.length() > rhs.length());
                case TokenType::GTEQ: return Value(lhs.length() >= rhs.length());
                case TokenType::LT: return Value(lhs.length() < rhs.length());
                case TokenType::LTEQ: return Value(lhs.length() <= rhs.length());
                default:
                    throw InterpreterError("Unexpected operator for string values: " + getTokenTypeName(op));
            }
        } else if (std::holds_alternative<bool>(leftBase) && std::holds_alternative<bool>(rightBase)) {
            bool lhs = std::get<bool>(leftBase);
            bool rhs = std::get<bool>(rightBase);
            switch (op) {
                case TokenType::EQUAL: return Value(lhs == rhs);
                case TokenType::NOTEQ: return Value(lhs != rhs);
                case TokenType::AND: return Value(lhs && rhs);
                case TokenType::OR: return Value(lhs || rhs);
                default:
                    throw InterpreterError("Unexpected operator for boolean values: " + getTokenTypeName(op));
            }
        }
    }
    throw InterpreterError("Unexpected binary operation: " + getTokenTypeName(op));
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
        return scope->getVariable(name);
    }
}

Value ListNode::evaluate(std::shared_ptr<Scope> scope) const {
    std::vector<Value> result;
    for (const auto& element : elements) {
        result.push_back(element->evaluate(scope));
    }
    return Value(result);
}

Value& IndexAccessNode::evaluateReference(std::shared_ptr<Scope> scope) const {
    const Value& listValue = list->evaluate(scope);
    Value indexValue = index->evaluate(scope);

    if (!listValue.isList()) {
        throw TypeError("Cannot access index of non-list value");
    }
    if (!indexValue.isBase() || !std::holds_alternative<double>(indexValue.asBase())) {
        throw TypeError("List index must be a number");
    }

    int idx = static_cast<int>(std::get<double>(indexValue.asBase()));
    auto& listData = listValue.asList();

    if (idx < 0 || idx >= listData.size()) {
        throw IndexError("Index out of range");
    }
    return *listData[idx];
}

Value IndexAccessNode::evaluate(std::shared_ptr<Scope> scope) const {
    return evaluateReference(scope);
}

Value ListAssignmentNode::evaluate(std::shared_ptr<Scope> scope) const {
    auto* indexAccessNode = dynamic_cast<IndexAccessNode*>(listAccess.get());
    if (!indexAccessNode) {
        throw InterpreterError("Invalid list assignment");
    }

    Value& elementRef = indexAccessNode->evaluateReference(scope);
    Value newValue = value->evaluate(scope);

    elementRef = newValue;

    if (auto* varNode = dynamic_cast<const VariableNode*>(indexAccessNode->list.get())) {
        scope->setVariable(varNode->name, indexAccessNode->list->evaluate(scope));
    }
    return newValue;
}

Value ListMethodCallNode::evaluate(std::shared_ptr<Scope> scope) const {
    Value listValue = list->evaluate(scope);
    if (!listValue.isList()) {
        throw TypeError("Cannot call method on non-list value");
    }

    Value result;
    if (methodName == "length") {
        if (!arguments.empty()) {
            throw SyntaxError("length() method doesn't take any arguments");
        }
        result = Value(static_cast<double>(listValue.length()));
    } else if (methodName == "append") {
        if (arguments.size() != 1) {
            throw SyntaxError("append() method takes exactly one argument");
        }
        Value argValue = arguments[0]->evaluate(scope);
        listValue.append(argValue);
        result = listValue;
    } else if (methodName == "remove") {
        if (arguments.size() != 1) {
            throw SyntaxError("remove() method takes exactly one argument");
        }
        Value indexValue = arguments[0]->evaluate(scope);
        if (!indexValue.isBase() || !std::holds_alternative<double>(indexValue.asBase())) {
            throw TypeError("remove() method argument must be a number");
        }
        int idx = static_cast<int>(std::get<double>(indexValue.asBase()));
        listValue.remove(idx);
        result = listValue;
    } else if (methodName == "put") {
        if (arguments.size() != 2) {
            throw SyntaxError("put() method takes exactly two arguments");
        }
        Value indexValue = arguments[0]->evaluate(scope);
        if (!indexValue.isBase() || !std::holds_alternative<double>(indexValue.asBase())) {
            throw TypeError("put() method first argument must be a number");
        }
        int idx = static_cast<int>(std::get<double>(indexValue.asBase()));
        Value argValue = arguments[1]->evaluate(scope);
        listValue.put(idx, argValue);
        result = listValue;
    } else {
        throw NameError("Unknown list method: " + methodName);
    }

    if (auto* indexAccessNode = dynamic_cast<const IndexAccessNode*>(list.get())) {
        Value parentListValue = indexAccessNode->list->evaluate(scope);
        Value indexValue = indexAccessNode->index->evaluate(scope);
        if (!indexValue.isBase() || !std::holds_alternative<double>(indexValue.asBase())) {
            throw TypeError("List index must be a number");
        }
        int idx = static_cast<int>(std::get<double>(indexValue.asBase()));
        if (result.isList()) {
            parentListValue.updateListElement(idx, result);
        }
        if (auto* varNode = dynamic_cast<const VariableNode*>(indexAccessNode->list.get())) {
            scope->assignVariable(varNode->name, parentListValue);
        }
    } else if (auto* varNode = dynamic_cast<const VariableNode*>(list.get())) {
        if (result.isList()) {
            scope->assignVariable(varNode->name, result);
        }
    }

    return result;
}

Value BlockNode::evaluate(std::shared_ptr<Scope> scope) const {
    auto blockScope = scope->createChildScope();
    Value lastValue;
    for (const auto& statement : statements) {
        lastValue = statement->evaluate(blockScope);
    }
    return lastValue;
}

Value IfElseNode::evaluate(std::shared_ptr<Scope> scope) const {
    Value cond = condition->evaluate(scope);
    if (cond.isBase() && std::holds_alternative<bool>(cond.asBase())) {
        if (std::get<bool>(cond.asBase())) {
            return ifBlock->evaluate(scope);
        } else if (elseBlock) {
            return elseBlock->evaluate(scope);
        }
    } else {
        throw SyntaxError("Expected boolean expression after 'if'");
    }
    return Value();
}
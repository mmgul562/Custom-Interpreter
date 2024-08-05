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

Value IndexAccessNode::evaluate(std::shared_ptr<Scope> scope) const {
    Value containerValue = container->evaluate(scope);
    Value indexValue = index->evaluate(scope);

    if (containerValue.isList()) {
        if (!indexValue.isBase() || !std::holds_alternative<double>(indexValue.asBase())) {
            throw TypeError("List index must be a number");
        }
        int idx = static_cast<int>(std::get<double>(indexValue.asBase()));
        return *containerValue.asList()[idx];
    } else if (containerValue.isDict()) {
        if (!indexValue.isBase()) {
            throw TypeError("Dictionary key must be a basic type");
        }
        auto& dict = containerValue.asDict();
        auto it = dict.find(indexValue.asBase());
        if (it == dict.end()) {
            throw NameError("Key '" + to_string(indexValue.asBase()) + "' not found in the dictionary");
        }
        return *it->second;
    } else {
        throw TypeError("Indexing can only be performed on containers");
    }
}

Value IndexAssignmentNode::evaluate(std::shared_ptr<Scope> scope) const {
    auto* indexAccessNode = dynamic_cast<IndexAccessNode*>(access.get());
    if (!indexAccessNode) {
        throw InterpreterError("Invalid index assignment");
    }

    Value containerValue = indexAccessNode->container->evaluate(scope);
    Value indexValue = indexAccessNode->index->evaluate(scope);
    Value newValue = value->evaluate(scope);

    if (containerValue.isList()) {
        if (!indexValue.isBase() || !std::holds_alternative<double>(indexValue.asBase())) {
            throw TypeError("List index must be a number");
        }
        int idx = static_cast<int>(std::get<double>(indexValue.asBase()));
        containerValue.updateListElement(idx, newValue);
    } else if (containerValue.isDict()) {
        if (!indexValue.isBase()) {
            throw TypeError("Dictionary key must be a basic type");
        }
        containerValue.setDictElement(indexValue.asBase(), newValue);
    } else {
        throw TypeError("Index assignment can only be performed on container values");
    }
    updateNestedContainer(indexAccessNode->container, containerValue, scope);

    return newValue;
}

void updateNestedContainer(const std::unique_ptr<ASTNode>& node, const Value& updatedValue, std::shared_ptr<Scope> scope) {
    if (auto* indexAccessNode = dynamic_cast<const IndexAccessNode*>(node.get())) {
        Value parentContainerValue = indexAccessNode->container->evaluate(scope);
        Value indexValue = indexAccessNode->index->evaluate(scope);

        if (parentContainerValue.isList()) {
            if (!indexValue.isBase() || !std::holds_alternative<double>(indexValue.asBase())) {
                throw TypeError("List index must be a number");
            }
            int idx = static_cast<int>(std::get<double>(indexValue.asBase()));
            parentContainerValue.updateListElement(idx, updatedValue);
        } else if (parentContainerValue.isDict()) {
            if (!indexValue.isBase()) {
                throw TypeError("Dictionary key must be a basic type");
            }
            parentContainerValue.setDictElement(indexValue.asBase(), updatedValue);
        }
        updateNestedContainer(indexAccessNode->container, parentContainerValue, scope);
    } else if (auto* varNode = dynamic_cast<const VariableNode*>(node.get())) {
        scope->assignVariable(varNode->name, updatedValue);
    }
}

Value ContainerMethodCallNode::evaluate(std::shared_ptr<Scope> scope) const {
    Value containerValue = container->evaluate(scope);

    if (containerValue.isList()) {
        if (methodName == "length") {
            if (!arguments.empty()) {
                throw SyntaxError("length() method doesn't take any arguments");
            }
            return Value(static_cast<double>(containerValue.length()));
        } else if (methodName == "append") {
            if (arguments.size() != 1) {
                throw SyntaxError("append() method takes exactly one argument");
            }
            Value argValue = arguments[0]->evaluate(scope);
            containerValue.append(argValue);
        } else if (methodName == "remove") {
            if (arguments.size() != 1) {
                throw SyntaxError("remove() method takes exactly one argument");
            }
            Value indexValue = arguments[0]->evaluate(scope);
            if (!indexValue.isBase() || !std::holds_alternative<double>(indexValue.asBase())) {
                throw TypeError("remove() method argument must be a number");
            }
            int idx = static_cast<int>(std::get<double>(indexValue.asBase()));
            containerValue.remove(idx);
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
            containerValue.put(idx, argValue);
        } else {
            throw NameError("Unknown list method: " + methodName);
        }
    } else if (containerValue.isDict()) {
        if (methodName == "size") {
            if (!arguments.empty()) {
                throw SyntaxError("size() method doesn't take any arguments");
            }
            return Value(static_cast<double>(containerValue.dictSize()));
        } else if (methodName == "remove") {
            if (arguments.size() != 1) {
                throw SyntaxError("remove() method takes exactly one argument");
            }
            Value keyValue = arguments[0]->evaluate(scope);
            if (!keyValue.isBase()) {
                throw TypeError("Dictionary key must be a basic type");
            }
            containerValue.removeKey(keyValue.asBase());
        } else if (methodName == "exists") {
            if (arguments.size() != 1) {
                throw SyntaxError("exists() method takes exactly one argument");
            }
            Value keyValue = arguments[0]->evaluate(scope);
            if (!keyValue.isBase()) {
                throw TypeError("Dictionary key must be a basic type");
            }
            return Value(containerValue.keyExists(keyValue.asBase()));
        } else {
            throw NameError("Unknown dictionary method: " + methodName);
        }
    } else {
        throw TypeError("Cannot call method on non-container value");
    }
    updateNestedContainer(container, containerValue, scope);

    return containerValue;
}

Value DictNode::evaluate(std::shared_ptr<Scope> scope) const {
    ValueDict dict;
    for (const auto& [keyNode, valueNode] : elements) {
        Value key = keyNode->evaluate(scope);
        if (!key.isBase()) {
            throw TypeError("Dictionary key must be a basic type");
        }
        Value value = valueNode->evaluate(scope);
        dict[key.asBase()] = std::make_shared<Value>(value);
    }
    return Value(std::move(dict));
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
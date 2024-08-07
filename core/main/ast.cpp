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
            throw InterpreterError("Unexpected unary operator: " + getTypeName(op));
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
                    throw InterpreterError("Unexpected operator for number values: " + getTypeName(op));
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
                    throw InterpreterError("Unexpected operator for string values: " + getTypeName(op));
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
                    throw InterpreterError("Unexpected operator for boolean values: " + getTypeName(op));
            }
        }
    }
    throw InterpreterError("Unexpected binary operation: " + getTypeName(op));
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

Value IndexAccessNode::evaluate(std::shared_ptr<Scope> scope) const {
    Value containerValue = container->evaluate(scope);
    Value indexValue = index->evaluate(scope);

    if (containerValue.isList()) {
        if (!indexValue.isBase() || !std::holds_alternative<double>(indexValue.asBase())) {
            throw TypeError("List index must be a number");
        }
        int idx = static_cast<int>(std::get<double>(indexValue.asBase()));
        if (idx >= containerValue.asList().size()) {
            throw IndexError("Index out of range");
        }
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
        if (methodName == "len") {
            if (!arguments.empty()) {
                throw SyntaxError("len() method doesn't take any arguments");
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

Value ForLoopNode::evaluate(std::shared_ptr<Scope> scope) const {
    auto loopScope = scope->createChildScope();
    Value lastValue;

    if (isRangeLoop) {
        Value startValue = startExpr->evaluate(scope);
        Value endValue = endExpr->evaluate(scope);

        if (!startValue.isBase() || !std::holds_alternative<double>(startValue.asBase()) ||
            !endValue.isBase() || !std::holds_alternative<double>(endValue.asBase())) {
            throw TypeError("Loop range must be numbers");
        }

        int start = static_cast<int>(std::get<double>(startValue.asBase()));
        int end = static_cast<int>(std::get<double>(endValue.asBase()));
        int step = 1;

        if (stepExpr) {
            Value stepValue = stepExpr->evaluate(scope);
            if (!stepValue.isBase() || !std::holds_alternative<double>(stepValue.asBase())) {
                throw TypeError("Loop step must be a number");
            }
            step = static_cast<int>(std::get<double>(stepValue.asBase()));
            if (step == 0) {
                throw ValueError("Loop step cannot be zero");
            }
        } else {
            step = (start <= end) ? 1 : -1;
        }

        if ((step > 0 && start > end) || (step < 0 && start < end)) {
            throw ValueError("Invalid loop range and step combination");
        }
        for (int i = start; (step > 0) ? (i <= end) : (i >= end); i += step) {
            loopScope->setVariable(variableName, Value(static_cast<double>(i)));
            try {
                lastValue = body->evaluate(loopScope);
            } catch (const ControlFlowException &e) {
                if (e.what() == std::string("BREAK")) break;
                if (e.what() == std::string("CONTINUE")) continue;
            }
        }
    } else {
        Value iterableValue = startExpr->evaluate(scope);
        if (!iterableValue.isDict()) {
            throw TypeError("Cannot iterate: not a dictionary");
        }
        std::vector<ValueBase> keys = iterableValue.getDictKeys();
        for (const auto& key : keys) {
            loopScope->setVariable(variableName, Value(key));
            try {
                lastValue = body->evaluate(loopScope);
            } catch (const ControlFlowException &e) {
                if (e.what() == std::string("BREAK")) break;
                if (e.what() == std::string("CONTINUE")) continue;
            }
        }
    }
    return lastValue;
}

Value WhileLoopNode::evaluate(std::shared_ptr<Scope> scope) const {
    Value cond = condition->evaluate(scope);
    Value lastValue;
    int maxIterations = 999999;

    if (cond.isBase() && std::holds_alternative<bool>(cond.asBase())) {
        while (std::get<bool>(cond.asBase()) && maxIterations > 0) {
            try {
                lastValue = body->evaluate(scope);
            } catch (const ControlFlowException &e) {
                if (e.what() == std::string("BREAK")) break;
                if (e.what() == std::string("CONTINUE")) continue;
            }
            cond = condition->evaluate(scope);
            maxIterations--;
        }
    } else {
        throw SyntaxError("Expected boolean expression after 'while'");
    }
    return lastValue;
}

Value ControlFlowNode::evaluate(std::shared_ptr<Scope> scope) const {
    if (isBreak) {
        throw ControlFlowException("BREAK");
    } else {
        throw ControlFlowException("CONTINUE");
    }
}
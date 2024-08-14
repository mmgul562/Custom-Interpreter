#include "../../util/errors.h"
#include "../../util/functions.h"
#include "../../util/utf8string.h"
#include "ast.h"


std::unique_ptr<ASTNode> FloatNode::clone() const {
    return std::make_unique<FloatNode>(*this);
}

Value FloatNode::evaluate(std::shared_ptr<Scope> scope) const {
    return Value(value);
}


std::unique_ptr<ASTNode> IntNode::clone() const {
    return std::make_unique<IntNode>(*this);
}

Value IntNode::evaluate(std::shared_ptr<Scope> scope) const {
    return Value(value);
}


std::unique_ptr<ASTNode> StringNode::clone() const {
    return std::make_unique<StringNode>(*this);
}

Value StringNode::evaluate(std::shared_ptr<Scope> scope) const {
    return Value(value);
}


std::unique_ptr<ASTNode> BoolNode::clone() const {
    return std::make_unique<BoolNode>(*this);
}

Value BoolNode::evaluate(std::shared_ptr<Scope> scope) const {
    return Value(value);
}


std::unique_ptr<ASTNode> TypeCastNode::clone() const {
    return std::make_unique<TypeCastNode>(type, var->clone());
}

std::string toString(const Value &value) {
    if (value.isBase()) {
        const auto &base = value.asBase();
        if (std::holds_alternative<double>(base)) {
            return std::to_string(std::get<double>(base));
        } else if (std::holds_alternative<long>(base)) {
            return std::to_string(std::get<long>(base));
        } else if (std::holds_alternative<bool>(base)) {
            return std::get<bool>(base) ? "true" : "false";
        } else if (std::holds_alternative<std::string>(base)) {
            return std::get<std::string>(base);
        }
    }
    throw TypeError("Cannot convert non-basic types to string");
}

double toFloat(const Value &value) {
    if (value.isBase()) {
        const auto &base = value.asBase();
        if (std::holds_alternative<double>(base)) {
            return std::get<double>(base);
        } else if (std::holds_alternative<long>(base)) {
            return static_cast<double>(std::get<long>(base));
        } else if (std::holds_alternative<bool>(base)) {
            return std::get<bool>(base) ? 1.0 : 0.0;
        } else if (std::holds_alternative<std::string>(base)) {
            std::string s = std::get<std::string>(base);
            try {
                return std::stod(s);
            } catch (const std::invalid_argument &) {
                throw ConversionError("Cannot convert string to float: " + s);
            } catch (const std::out_of_range &) {
                throw ConversionError("Number out of range: " + s);
            }
        }
    }
    throw TypeError("Cannot convert non-basic types to float");
}

long toInt(const Value &value) {
    if (value.isBase()) {
        const auto &base = value.asBase();
        if (std::holds_alternative<double>(base)) {
            return static_cast<long>(std::get<double>(base));
        } else if (std::holds_alternative<long>(base)) {
            return std::get<long>(base);
        } else if (std::holds_alternative<bool>(base)) {
            return std::get<bool>(base) ? 1 : 0;
        } else if (std::holds_alternative<std::string>(base)) {
            try {
                return std::stol(std::get<std::string>(base));
            } catch (const std::invalid_argument &) {
                throw ConversionError("Cannot convert string to int: " + std::get<std::string>(base));
            } catch (const std::out_of_range &) {
                throw ConversionError("Number out of range: " + std::get<std::string>(base));
            }
        }
    }
    throw TypeError("Cannot convert non-basic types to int");
}

bool toBool(const Value &value, bool qmark) {
    if (value.isBase()) {
        const auto &base = value.asBase();
        if (std::holds_alternative<double>(base)) {
            return std::get<double>(base) != 0.0;
        } else if (std::holds_alternative<long>(base)) {
            return std::get<long>(base) != 0;
        } else if (std::holds_alternative<bool>(base)) {
            return std::get<bool>(base);
        } else if (std::holds_alternative<std::string>(base)) {
            return !std::get<std::string>(base).empty();
        }
    } else if (qmark) {
        if (value.isList()) {
            return !value.asList().empty();
        } else if (value.isDict()) {
            return !value.asDict().empty();
        }
    }
    throw TypeError("Cannot convert non-basic types to boolean");
}

Value TypeCastNode::evaluate(std::shared_ptr<Scope> scope) const {
    auto value = var->evaluate(scope);
    switch (type) {
        case TokenType::INT_T:
            return Value(toInt(value));
        case TokenType::FLOAT_T:
            return Value(toFloat(value));
        case TokenType::BOOL_T:
            return Value(toBool(value, false));
        case TokenType::STR_T:
            return Value(toString(value));
        default:
            throw ConversionError("Invalid conversion type: " + getTypeName(type));
    }
}


std::unique_ptr<ASTNode> UnaryOpNode::clone() const {
    return std::make_unique<UnaryOpNode>(op, operand->clone());
}

Value UnaryOpNode::evaluate(std::shared_ptr<Scope> scope) const {
    auto operandValue = operand->evaluate(scope);
    switch (op) {
        case TokenType::NOT:
            if (operandValue.isBase() && std::holds_alternative<bool>(operandValue.asBase())) {
                return Value(!std::get<bool>(operandValue.asBase()));
            }
            throw TypeError("NOT operator can only be used with boolean values");
        case TokenType::MINUS:
            if (operandValue.isBase()) {
                ValueBase base = operandValue.asBase();
                if (std::holds_alternative<double>(base)) {
                    return Value(-std::get<double>(base));
                } else if (std::holds_alternative<long>(base)) {
                    return Value(-std::get<long>(base));
                }
            }
            throw TypeError("MINUS operator can only be used with numbers");
        case TokenType::UNDERSCORE:
            if (operandValue.isBase()) {
                if (std::holds_alternative<double>(operandValue.asBase())) {
                    return Value(std::abs(std::get<double>(operandValue.asBase())));
                } else if (std::holds_alternative<long>(operandValue.asBase())) {
                    return Value(std::abs(std::get<long>(operandValue.asBase())));
                }
            }
            throw TypeError("UNDERSCORE (absolute) operator can only be used with numbers");
        case TokenType::QMARK:
            return Value(toBool(operandValue, true));
        default:
            throw InterpreterError("Unexpected unary operator: " + getTypeName(op));
    }
}


Value BinaryOpVisitor::operator()(double lhs, double rhs) const {
    switch (op) {
        case TokenType::EQUAL:
            return Value(lhs == rhs);
        case TokenType::NOTEQ:
            return Value(lhs != rhs);
        case TokenType::GT:
            return Value(lhs > rhs);
        case TokenType::GTEQ:
            return Value(lhs >= rhs);
        case TokenType::LT:
            return Value(lhs < rhs);
        case TokenType::LTEQ:
            return Value(lhs <= rhs);
        case TokenType::PLUS:
            return Value(lhs + rhs);
        case TokenType::MINUS:
            return Value(lhs - rhs);
        case TokenType::MOD:
            return Value(std::fmod(lhs, rhs));
        case TokenType::ASTER:
            return Value(lhs * rhs);
        case TokenType::DBL_ASTER:
            return Value(std::pow(lhs, rhs));
        case TokenType::SLASH:
            return Value(lhs / rhs);
        case TokenType::DBL_SLASH:
            return Value(std::floor(lhs / rhs));
        default:
            throw InterpreterError("Unexpected binary operator for float values: " + getTypeName(op));
    }
}

Value BinaryOpVisitor::operator()(long lhs, long rhs) const {
    switch (op) {
        case TokenType::EQUAL:
            return Value(lhs == rhs);
        case TokenType::NOTEQ:
            return Value(lhs != rhs);
        case TokenType::GT:
            return Value(lhs > rhs);
        case TokenType::GTEQ:
            return Value(lhs >= rhs);
        case TokenType::LT:
            return Value(lhs < rhs);
        case TokenType::LTEQ:
            return Value(lhs <= rhs);
        case TokenType::PLUS:
            return Value(lhs + rhs);
        case TokenType::MINUS:
            return Value(lhs - rhs);
        case TokenType::MOD:
            return Value(lhs % rhs);
        case TokenType::ASTER:
            return Value(lhs * rhs);
        case TokenType::DBL_ASTER:
            return Value(static_cast<long>(std::pow(lhs, rhs)));
        case TokenType::SLASH: case TokenType::DBL_SLASH:
            return Value(lhs / rhs);
        default:
            throw InterpreterError("Unexpected binary operator for int values: " + getTypeName(op));
    }
}

Value BinaryOpVisitor::operator()(const std::string &lhs, const std::string &rhs) const {
    switch (op) {
        case TokenType::PLUS:
            return Value(lhs + rhs);
        case TokenType::EQUAL:
            return Value(lhs == rhs);
        case TokenType::NOTEQ:
            return Value(lhs != rhs);
        case TokenType::GT:
            return Value(lhs.length() > rhs.length());
        case TokenType::GTEQ:
            return Value(lhs.length() >= rhs.length());
        case TokenType::LT:
            return Value(lhs.length() < rhs.length());
        case TokenType::LTEQ:
            return Value(lhs.length() <= rhs.length());
        default:
            throw InterpreterError("Unexpected binary operator for string values: " + getTypeName(op));
    }
}

Value BinaryOpVisitor::operator()(bool lhs, bool rhs) const {
    switch (op) {
        case TokenType::EQUAL:
            return Value(lhs == rhs);
        case TokenType::NOTEQ:
            return Value(lhs != rhs);
        case TokenType::AND:
            return Value(lhs && rhs);
        case TokenType::OR:
            return Value(lhs || rhs);
        default:
            throw InterpreterError("Unexpected binary operator for boolean values: " + getTypeName(op));
    }
}

template<typename T, typename U>
Value BinaryOpVisitor::operator()(const T &, const U &) const {
    throw InterpreterError("Unexpected binary operator: " + getTypeName(op));
}

std::unique_ptr<ASTNode> BinaryOpNode::clone() const {
    return std::make_unique<BinaryOpNode>(op, left->clone(), right->clone());
}

Value BinaryOpNode::evaluate(std::shared_ptr<Scope> scope) const {
    auto leftValue = left->evaluate(scope);
    auto rightValue = right->evaluate(scope);

    if (leftValue.isBase() && rightValue.isBase()) {
        const auto &leftBase = leftValue.asBase();
        const auto &rightBase = rightValue.asBase();

        return std::visit(BinaryOpVisitor{op}, leftBase, rightBase);
    }
    throw InterpreterError("Unexpected binary operator: " + getTypeName(op));
}


std::unique_ptr<ASTNode> AssignmentNode::clone() const {
    return std::make_unique<AssignmentNode>(name, reassign, valueNode->clone());
}

Value AssignmentNode::evaluate(std::shared_ptr<Scope> scope) const {
    Value value = valueNode->evaluate(scope);
    if (reassign) {
        scope->assignVariable(name, value);
    } else {
        scope->setVariable(name, value);
    }
    return value;
}


std::unique_ptr<ASTNode> VariableNode::clone() const {
    return std::make_unique<VariableNode>(name);
}

Value VariableNode::evaluate(std::shared_ptr<Scope> scope) const {
    return scope->getVariable(name);
}


std::unique_ptr<ASTNode> ListNode::clone() const {
    std::vector<std::unique_ptr<ASTNode>> clonedElements;
    for (const auto &element: elements) {
        clonedElements.push_back(element->clone());
    }
    return std::make_unique<ListNode>(std::move(clonedElements));
}

Value ListNode::evaluate(std::shared_ptr<Scope> scope) const {
    std::vector<Value> result;
    for (const auto &element: elements) {
        result.push_back(element->evaluate(scope));
    }
    return Value(result);
}


std::unique_ptr<ASTNode> DictNode::clone() const {
    std::vector<std::pair<std::unique_ptr<ASTNode>, std::unique_ptr<ASTNode>>> clonedElements;
    for (const auto &element: elements) {
        clonedElements.emplace_back(element.first->clone(), element.second->clone());
    }
    return std::make_unique<DictNode>(std::move(clonedElements));
}

Value DictNode::evaluate(std::shared_ptr<Scope> scope) const {
    ValueDict dict;
    for (const auto &[keyNode, valueNode]: elements) {
        Value key = keyNode->evaluate(scope);
        if (!key.isBase()) {
            throw TypeError("Dictionary key must be a basic type");
        }
        Value value = valueNode->evaluate(scope);
        dict[key.asBase()] = std::make_shared<Value>(value);
    }
    return Value(std::move(dict));
}


std::unique_ptr<ASTNode> IndexAccessNode::clone() const {
    return std::make_unique<IndexAccessNode>(container->clone(), index->clone());
}

Value IndexAccessNode::evaluate(std::shared_ptr<Scope> scope) const {
    Value containerValue = container->evaluate(scope);
    Value indexValue = index->evaluate(scope);

    if (containerValue.isList()) {
        if (!indexValue.isBase() || !std::holds_alternative<long>(indexValue.asBase())) {
            throw TypeError("List index must be an integer");
        }
        long idx = std::get<long>(indexValue.asBase());
        if (idx >= containerValue.asList().size() || idx < 0) {
            throw IndexError("Index (" + std::to_string(idx) + ") out of range");
        }
        return *containerValue.asList()[idx];
    } else if (containerValue.isDict()) {
        if (!indexValue.isBase()) {
            throw TypeError("Dictionary key must be a basic type");
        }
        auto &dict = containerValue.asDict();
        auto it = dict.find(indexValue.asBase());
        if (it == dict.end()) {
            throw NameError("Key '" + indexValue.toString() + "' not found in the dictionary");
        }
        return *it->second;
    } else if (std::holds_alternative<std::string>(containerValue.asBase())) {
        auto &s = std::get<std::string>(containerValue.asBase());
        if (!indexValue.isBase() || !std::holds_alternative<long>(indexValue.asBase())) {
            throw TypeError("String index must be an integer");
        }
        long idx = std::get<long>(indexValue.asBase());
        if (idx >= getStrLen(s) || idx < 0) {
            throw IndexError("Index (" + std::to_string(idx) + ") out of range");
        }
        return Value(getStrChar(s, idx));
    } else {
        throw TypeError("Indexing can only be performed on lists, dictionaries and strings");
    }
}


void updateNestedContainer(const std::unique_ptr<ASTNode> &node, const Value &updated, std::shared_ptr<Scope> scope) {
    if (auto *indexAccessNode = dynamic_cast<const IndexAccessNode *>(node.get())) {
        Value parentContainerValue = indexAccessNode->getContainer()->evaluate(scope);
        Value indexValue = indexAccessNode->getIndex()->evaluate(scope);

        if (parentContainerValue.isList()) {
            if (!indexValue.isBase() || !std::holds_alternative<long>(indexValue.asBase())) {
                throw TypeError("List index must be an integer");
            }
            auto idx = static_cast<size_t>(std::get<long>(indexValue.asBase()));
            parentContainerValue.updateListElement(idx, updated);
        } else if (parentContainerValue.isDict()) {
            if (!indexValue.isBase()) {
                throw TypeError("Dictionary key must be a basic type");
            }
            parentContainerValue.setDictElement(indexValue.asBase(), updated);
        }
        updateNestedContainer(indexAccessNode->getContainer(), parentContainerValue, scope);
    } else if (auto *varNode = dynamic_cast<const VariableNode *>(node.get())) {
        scope->assignVariable(varNode->getName(), updated);
    }
}


std::unique_ptr<ASTNode> IndexAssignmentNode::clone() const {
    return std::make_unique<IndexAssignmentNode>(access->clone(), value->clone());
}

Value IndexAssignmentNode::evaluate(std::shared_ptr<Scope> scope) const {
    auto *indexAccessNode = dynamic_cast<IndexAccessNode *>(access.get());
    if (!indexAccessNode) {
        throw InterpreterError("Invalid index assignment");
    }

    Value containerValue = indexAccessNode->getContainer()->evaluate(scope);
    Value indexValue = indexAccessNode->getIndex()->evaluate(scope);
    Value newValue = value->evaluate(scope);

    if (containerValue.isList()) {
        if (!indexValue.isBase() || !std::holds_alternative<long>(indexValue.asBase())) {
            throw TypeError("List index must be an integer");
        }
        long idx = std::get<long>(indexValue.asBase());
        containerValue.updateListElement(idx, newValue);
    } else if (containerValue.isDict()) {
        if (!indexValue.isBase()) {
            throw TypeError("Dictionary key must be a basic type");
        }
        containerValue.setDictElement(indexValue.asBase(), newValue);
    } else {
        throw TypeError("Index assignment can only be performed on lists and dictionaries");
    }
    updateNestedContainer(indexAccessNode->getContainer(), containerValue, scope);
    return newValue;
}


std::unique_ptr<ASTNode> MethodCallNode::clone() const {
    std::vector<std::unique_ptr<ASTNode>> clonedArguments;
    for (const auto &arg: arguments) {
        clonedArguments.push_back(arg->clone());
    }
    return std::make_unique<MethodCallNode>(container->clone(), methodName, std::move(clonedArguments));
}

Value MethodCallNode::evaluate(std::shared_ptr<Scope> scope) const {
    Value containerValue = container->evaluate(scope);

    if (containerValue.isList()) {
        if (methodName == "len") {
            return listlen(containerValue, arguments);
        } else if (methodName == "append") {
            listappend(containerValue, arguments, scope);
        } else if (methodName == "remove") {
            listremove(containerValue, arguments, scope);
        } else if (methodName == "put") {
            listput(containerValue, arguments, scope);
        } else {
            throw NameError("Unknown list method: " + methodName);
        }
    } else if (containerValue.isDict()) {
        if (methodName == "size") {
            return dictsize(containerValue, arguments);
        } else if (methodName == "remove") {
            dictremove(containerValue, arguments, scope);
        } else if (methodName == "exists") {
            return dictexists(containerValue, arguments, scope);
        } else {
            throw NameError("Unknown dictionary method: " + methodName);
        }
    } else if (containerValue.isBase() && std::holds_alternative<std::string>(containerValue.asBase())) {
        if (methodName == "len") {
            return slen(containerValue, arguments);
        } else if (methodName == "ltrim") {
            sltrim(containerValue, arguments, scope);
        } else if (methodName == "rtrim") {
            srtrim(containerValue, arguments, scope);
        } else {
            throw NameError("Unknown string method: " + methodName);
        }
    } else {
        throw TypeError("Methods can only be called on lists, dictionaries and strings");
    }
    updateNestedContainer(container, containerValue, scope);
    return containerValue;
}


std::unique_ptr<ASTNode> BlockNode::clone() const {
    return std::make_unique<BlockNode>(*this);
}

Value BlockNode::evaluate(std::shared_ptr<Scope> scope) const {
    auto blockScope = scope->createChildScope();
    Value lastValue;
    for (const auto &statement: statements) {
        try {
            lastValue = statement->evaluate(blockScope);
        } catch (const ReturnException &e) {
            throw;
        }
    }
    return lastValue;
}


std::unique_ptr<ASTNode> IfElseNode::clone() const {
    return std::make_unique<IfElseNode>(condition->clone(), std::make_unique<BlockNode>(*ifBlock),
                                        elseBlock ? std::make_unique<BlockNode>(*elseBlock) : nullptr);
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
        throw TypeError("Expected boolean expression after 'if'");
    }
    return Value();
}


std::unique_ptr<ASTNode> ForLoopNode::clone() const {
    auto clonedStartExpr = startExpr ? startExpr->clone() : nullptr;
    auto clonedEndExpr = endExpr ? endExpr->clone() : nullptr;
    auto clonedStepExpr = stepExpr ? stepExpr->clone() : nullptr;
    auto clonedBody = body ? std::make_unique<BlockNode>(*body) : nullptr;
    if (!clonedStartExpr || !clonedBody) {
        throw std::runtime_error("Cannot clone a ForLoopNode without cloning startExpr and body");
    }
    return std::make_unique<ForLoopNode>(variableName, std::move(clonedStartExpr), std::move(clonedEndExpr),
                                         std::move(clonedStepExpr), std::move(clonedBody), isRangeLoop);
}

Value ForLoopNode::evaluate(std::shared_ptr<Scope> scope) const {
    auto loopScope = scope->createChildScope();
    Value lastValue;

    if (isRangeLoop) {
        Value startValue = startExpr->evaluate(scope);
        Value endValue = endExpr->evaluate(scope);

        if (!startValue.isBase() || !std::holds_alternative<long>(startValue.asBase()) ||
            !endValue.isBase() || !std::holds_alternative<long>(endValue.asBase())) {
            throw TypeError("Loop range must be integers");
        }

        long start = std::get<long>(startValue.asBase());
        long end = std::get<long>(endValue.asBase());
        long step;

        if (stepExpr) {
            Value stepValue = stepExpr->evaluate(scope);
            if (!stepValue.isBase() || !std::holds_alternative<long>(stepValue.asBase())) {
                throw TypeError("Loop step must be an integer");
            }
            step = std::get<long>(stepValue.asBase());
            if (step == 0) {
                throw ValueError("Loop step cannot be zero");
            }
        } else {
            step = (start <= end) ? 1 : -1;
        }

        if ((step > 0 && start > end) || (step < 0 && start < end)) {
            throw ValueError("Invalid loop range and step combination");
        }
        for (long i = start; (step > 0) ? (i <= end) : (i >= end); i += step) {
            loopScope->setVariable(variableName, Value(i));
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
        for (const auto &key: keys) {
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


std::unique_ptr<ASTNode> WhileLoopNode::clone() const {
    return std::make_unique<WhileLoopNode>(condition->clone(), std::make_unique<BlockNode>(*body));
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
        throw TypeError("Expected boolean expression after 'while'");
    }
    return lastValue;
}


std::unique_ptr<ASTNode> ControlFlowNode::clone() const {
    return std::make_unique<ControlFlowNode>(*this);
}

Value ControlFlowNode::evaluate(std::shared_ptr<Scope> scope) const {
    if (isBreak) {
        throw ControlFlowException("BREAK");
    } else {
        throw ControlFlowException("CONTINUE");
    }
}


std::unique_ptr<ASTNode> ReturnNode::clone() const {
    return std::make_unique<ReturnNode>(expression ? expression->clone() : nullptr);
}

Value ReturnNode::evaluate(std::shared_ptr<Scope> scope) const {
    if (expression) {
        Value result = expression->evaluate(scope);
        throw ReturnException(result);
    }
    throw ReturnException(Value());
}


std::unique_ptr<ASTNode> FunctionDeclarationNode::clone() const {
    return std::make_unique<FunctionDeclarationNode>(*this);
}

Value FunctionDeclarationNode::evaluate(std::shared_ptr<Scope> scope) const {
    if (name == "print" || name == "type" || name == "roundf" || name == "round" || name == "floor" || name == "ceil") {
        throw NameError("Function " + name + "() is a built-in function and cannot be redefined");
    }
    scope->setFunction(name, std::make_shared<FunctionDeclarationNode>(*this));
    return Value();
}


std::unique_ptr<ASTNode> FunctionCallNode::clone() const {
    std::vector<std::unique_ptr<ASTNode>> clonedArguments;
    for (const auto &arg: arguments) {
        clonedArguments.push_back(arg->clone());
    }
    return std::make_unique<FunctionCallNode>(name, std::move(clonedArguments));
}

Value FunctionCallNode::evaluate(std::shared_ptr<Scope> scope) const {
    std::shared_ptr<FunctionDeclarationNode> func = scope->getFunction(name);
    size_t argSize = arguments.size();
    if (name == "print") {
        return print(arguments, scope);
    } else if (name == "type") {
        return type(arguments, scope);
    } else if (name == "roundf") {
        return roundf(arguments, scope);
    } else if (name == "round") {
        return roundi(arguments, scope);
    } else if (name == "floor") {
        return floori(arguments, scope);
    } else if (name == "ceil") {
        return ceili(arguments, scope);
    } else if (!func) {
        throw NameError("Unidentified function: " + name);
    }

    bool hasArgs = func->getHasArgs();
    size_t paramSize = func->getParameters().size();
    if (hasArgs && paramSize - 1 > argSize) {
        throw ValueError(
                "Function " + name + "() expects at least " + std::to_string(paramSize - 1) + " arguments, but got " +
                std::to_string(argSize));
    } else if (!hasArgs && paramSize != argSize) {
        throw ValueError(
                "Function " + name + "() expects exactly " + std::to_string(paramSize) + " arguments, but got " +
                std::to_string(argSize));
    }

    auto childScope = scope->createChildScope();
    int i;
    if (paramSize > 0) {
        for (i = 0; i < paramSize - 1; ++i) {
            Value argumentValue = arguments[i]->evaluate(scope);
            childScope->setVariable(func->getParameters()[i], argumentValue);
        }
        if (!hasArgs) {
            Value argumentValue = arguments[i]->evaluate(scope);
            childScope->setVariable(func->getParameters()[i], argumentValue);
        } else {
            std::vector<Value> args;
            for (int j = i; j < argSize; ++j) {
                args.push_back(arguments[j]->evaluate(scope));
            }
            Value argumentList = Value(args);
            childScope->setVariable(func->getParameters()[i], argumentList);
        }
    }
    try {
        return func->getBody()->evaluate(childScope);
    } catch (const ReturnException &e) {
        return e.returnValue;
    }
}


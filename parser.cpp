#include "parser.h"

std::unordered_map<std::string, Value> variableTable;

bool isIdentified(const std::string &name) {
    return variableTable.find(name) != variableTable.end();
}

// NODES

Value NumberNode::evaluate() const {
    return value;
}

Value StringNode::evaluate() const {
    return value;
}

Value BoolNode::evaluate() const {
    return value;
}

Value BinaryOpNode::evaluate() const {
    auto leftValue = left->evaluate();
    auto rightValue = right->evaluate();

    if (std::holds_alternative<double>(leftValue) && std::holds_alternative<double>(rightValue)) {
        double lhs = std::get<double>(leftValue);
        double rhs = std::get<double>(rightValue);

        switch (op) {
            case TokenType::PLUS:
                return lhs + rhs;
            case TokenType::MINUS:
                return lhs - rhs;
            case TokenType::ASTER:
                return lhs * rhs;
            case TokenType::DBL_ASTER:
                return std::pow(lhs, rhs);
            case TokenType::SLASH:
                return lhs / rhs;
            case TokenType::DBL_SLASH:
                return std::floor(lhs / rhs);
            default:
                throw std::runtime_error("Unexpected operator");
        }
    } else if (std::holds_alternative<std::string>(leftValue) && std::holds_alternative<std::string>(rightValue) &&
               op == TokenType::PLUS) {
        return std::get<std::string>(leftValue) + std::get<std::string>(rightValue);
    }
    throw std::runtime_error("Unsupported binary operation");
}

VariableNode::VariableNode(std::string name, Value value) : name(std::move(name)), value(std::move(value)) {
    variableTable[this->name] = this->value;
}

VariableNode::VariableNode(std::string name) : name(std::move(name)) {
    value = variableTable[this->name];
}

Value VariableNode::evaluate() const {
    return value;
}

// PARSER

void Parser::advanceToken() {
    currentToken = lexer.getNextToken();
}

std::unique_ptr<ASTNode> Parser::parseStatement() {
    if (currentToken.type == TokenType::IDENTIFIER) {
        std::string varName = currentToken.stringValue;
        advanceToken();
        if (currentToken.type == TokenType::ASSIGN) {
            advanceToken();
            auto valueNode = parseExpression();
            if (currentToken.type != TokenType::END) {
                throw std::runtime_error("Unexpected token after assignment");
            }
            return std::make_unique<VariableNode>(varName, valueNode->evaluate());
        } else {
            lexer.pos = 0;
            advanceToken();
        }
    }
    auto exp = parseExpression();
    if (currentToken.type != TokenType::END) {
        throw std::runtime_error("Unexpected end of expression");
    }
    return exp;
}

std::unique_ptr<ASTNode> Parser::parseExpression() {
    auto node = parseTerm();
    while (currentToken.type == TokenType::PLUS || currentToken.type == TokenType::MINUS) {
        TokenType op = currentToken.type;
        advanceToken();
        node = std::make_unique<BinaryOpNode>(op, std::move(node), parseTerm());
    }
    return node;
}

std::unique_ptr<ASTNode> Parser::parseTerm() {
    auto node = parseFactor();
    while (currentToken.type == TokenType::ASTER || currentToken.type == TokenType::SLASH
           || currentToken.type == TokenType::DBL_ASTER || currentToken.type == TokenType::DBL_SLASH) {
        TokenType op = currentToken.type;
        advanceToken();
        node = std::make_unique<BinaryOpNode>(op, std::move(node), parseFactor());
    }
    return node;
}

std::unique_ptr<ASTNode> Parser::parseFactor() {
    if (currentToken.type == TokenType::NUMBER) {
        auto node = std::make_unique<NumberNode>(currentToken.value.numberValue);
        advanceToken();
        return node;
    }
    if (currentToken.type == TokenType::STRING) {
        auto node = std::make_unique<StringNode>(currentToken.stringValue);
        advanceToken();
        return node;
    }
    if (currentToken.type == TokenType::TRUE || currentToken.type == TokenType::FALSE) {
        auto node = std::make_unique<BoolNode>(currentToken.value.boolValue);
        advanceToken();
        return node;
    }
    if (currentToken.type == TokenType::IDENTIFIER) {
        if (isIdentified(currentToken.stringValue)) {
            auto node = std::make_unique<VariableNode>(currentToken.stringValue);
            advanceToken();
            return node;
        }
        throw std::runtime_error("Unidentified variable");
    }
    if (currentToken.type == TokenType::LPAREN) {
        advanceToken();
        auto node = parseExpression();
        if (currentToken.type != TokenType::RPAREN) {
            throw std::runtime_error("Closing parentheses ')' not found");
        }
        advanceToken();
        return node;
    }
    throw std::runtime_error("Unexpected token");
}
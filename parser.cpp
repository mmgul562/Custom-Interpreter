#include "parser.h"


std::unordered_map<std::string, std::shared_ptr<ASTNode>> variableTable;

bool isIdentified(const std::string &name) {
    return variableTable.find(name) != variableTable.end();
}

// NODES

double NumberNode::evaluate() const {
    return value;
}

double BinaryOpNode::evaluate() const {
    double leftVal = left->evaluate();
    double rightVal = right->evaluate();
    switch (op) {
        case TokenType::PLUS: return leftVal + rightVal;
        case TokenType::MINUS: return leftVal - rightVal;
        case TokenType::ASTER: return leftVal * rightVal;
        case TokenType::DBL_ASTER: return pow(leftVal, rightVal);
        case TokenType::SLASH: return leftVal / rightVal;
        case TokenType::DBL_SLASH: return floor(leftVal / rightVal);
        default:
            throw std::runtime_error("Unexpected operator");
    }
}

VariableNode::VariableNode(std::string name, std::shared_ptr<ASTNode> value) {
    variableTable[name] = value;
    this->value = std::move(value);
    this->name = std::move(name);
}

VariableNode::VariableNode(std::string name) {
    this->value = variableTable[name];
    this->name = std::move(name);
}

double VariableNode::evaluate() const {
    return value->evaluate();
}

// PARSER

void Parser::advanceToken() {
    currentToken = lexer.getNextToken();
}

std::unique_ptr<ASTNode> Parser::parseStatement() {
    if (currentToken.type == TokenType::IDENTIFIER) {
        std::string varName = currentToken.name;
        advanceToken();
        if (currentToken.type == TokenType::ASSIGN) {
            advanceToken();
            auto value = parseExpression();
            if (currentToken.type != TokenType::END) {
                throw std::runtime_error("Unexpected token after assignment");
            }
            return std::make_unique<VariableNode>(varName, std::move(value));
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
        auto node = std::make_unique<NumberNode>(currentToken.value);
        advanceToken();
        return node;
    }
    if (currentToken.type == TokenType::IDENTIFIER) {
        if (isIdentified(currentToken.name)) {
            auto node = std::make_unique<VariableNode>(currentToken.name);
            advanceToken();
            return node;
        }
        throw std::runtime_error("Unidentified variable");
    }
    if (currentToken.type == TokenType::LPAREN) {
        advanceToken();
        auto node = parseExpression();
        if (currentToken.type != TokenType::RPAREN) {
            throw std::runtime_error("Closing parentheses ')' not found in given expression");
        }
        advanceToken();
        return node;
    }
    throw std::runtime_error("Unexpected token");
}

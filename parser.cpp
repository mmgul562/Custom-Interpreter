#include "parser.h"


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
        case TokenType::SLASH: return leftVal / rightVal;
        default:
            throw std::runtime_error("Unexpected operator");
    }
}

void Parser::advanceToken() {
    currentToken = lexer.getNextToken();
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
    while (currentToken.type == TokenType::ASTER || currentToken.type == TokenType::SLASH) {
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
    if (currentToken.type == TokenType::LPAREN) {
        advanceToken();
        auto node = parseExpression();
        if (currentToken.type != TokenType::RPAREN) {
            throw std::runtime_error("Closing parentheses not found");
        }
        advanceToken();
        return node;
    }
    throw std::runtime_error("Unexpected token");
}

void Parser::reset() {
    advanceToken();
}

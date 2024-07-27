#include "parser.h"


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
    auto node = parseComparison();
    if (currentToken.type != TokenType::END) {
        throw std::runtime_error("Unexpected end of expression");
    }
    return node;
}

std::unique_ptr<ASTNode> Parser::parseComparison() {
    auto node = parseExpression();
    while (currentToken.type == TokenType::EQUAL || currentToken.type == TokenType::NOTEQ
           || currentToken.type == TokenType::GT || currentToken.type == TokenType::GTEQ
           || currentToken.type == TokenType::LT || currentToken.type == TokenType::LTEQ) {
        TokenType op = currentToken.type;
        advanceToken();
        node = std::make_unique<BinaryOpNode>(op, std::move(node), parseExpression());
    }
    return node;
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
           || currentToken.type == TokenType::DBL_ASTER || currentToken.type == TokenType::DBL_SLASH
           || currentToken.type == TokenType::AND || currentToken.type == TokenType::OR
           || currentToken.type == TokenType::MOD) {
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
    if (currentToken.type == TokenType::NOT || currentToken.type == TokenType::UNDERSCORE) {
        TokenType op = currentToken.type;
        advanceToken();
        return std::make_unique<UnaryOpNode>(op, parseFactor());
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
        auto node = parseComparison();
        if (currentToken.type != TokenType::RPAREN) {
            throw std::runtime_error("Closing parentheses ')' not found");
        }
        advanceToken();
        return node;
    }
    throw std::runtime_error("Unexpected token");
}

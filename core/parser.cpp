#include "parser.h"


void Parser::skipNewLines() {
    while (currentToken.type == TokenType::EOL) {
        advanceToken();
    }
}

bool Parser::expectToken(TokenType type) {
    if (type != TokenType::EOL) skipNewLines();
    if (currentToken.type == type) {
        advanceToken();
        return true;
    }
    return false;
}

void Parser::advanceToken() {
    currentToken = lexer.getNextToken();
}

bool Parser::isStatementComplete() {
    int nestedLevel = 0;
    Token tempToken = currentToken;

    while (tempToken.type != TokenType::END) {
        if (tempToken.type == TokenType::IF && lexer.peekNextTokenType() != TokenType::EOL) {
            nestedLevel++;
        } else if (tempToken.type == TokenType::THEN) {
        } else if (tempToken.type == TokenType::STOP) {
            if (nestedLevel == 0) {
                return true;
            }
            nestedLevel--;
        } else if (tempToken.type == TokenType::SEMICOLON) {
            if (nestedLevel == 0) {
                return true;
            }
        } else if (nestedLevel == 0) {
            return true;
        }
        tempToken = lexer.getNextToken();
    }
    return nestedLevel == 0;
}

// parsing

std::vector<std::unique_ptr<ASTNode>> Parser::parse() {
    std::vector<std::unique_ptr<ASTNode>> statements;
    while (currentToken.type != TokenType::END) {
        if (expectToken(TokenType::EOL) || expectToken(TokenType::SEMICOLON)) {
            continue;
        }
        statements.push_back(parseStatement());
        if (currentToken.type != TokenType::SEMICOLON &&
            currentToken.type != TokenType::EOL &&
            currentToken.type != TokenType::END) {
            throw std::runtime_error("Expected ';' or new line after statement");
        }
    }
    return statements;
}

std::unique_ptr<ASTNode> Parser::parseStatement() {
    if (currentToken.type == TokenType::IF) {
        return parseIfStatement();
    } else if (currentToken.type == TokenType::IDENTIFIER) {
        if (lexer.peekNextTokenType() == TokenType::ASSIGN) {
            return parseAssignment();
        } else {
            return parseExpression_1();
        }
    } else {
        return parseExpression_1();
    }
}

std::unique_ptr<ASTNode> Parser::parseIfStatement() {
    advanceToken();
    auto condition = parseExpression_1();

    if (!expectToken(TokenType::THEN)) {
        throw std::runtime_error("Expected 'then' after if condition");
    }
    auto ifBlock = parseBlock();

    std::unique_ptr<BlockNode> elseBlock = nullptr;
    if (expectToken(TokenType::ELSE)) {
        elseBlock = parseBlock();
    }
    if (currentToken.type != TokenType::STOP) {
        throw std::runtime_error("Expected 'stop' at the end of if statement");
    }
    advanceToken();
    return std::make_unique<IfElseNode>(std::move(condition), std::move(ifBlock), std::move(elseBlock));
}

std::unique_ptr<BlockNode> Parser::parseBlock() {
    std::vector<std::unique_ptr<ASTNode>> statements;
    int nestedLevel = 0;

    while (true) {
        if (expectToken(TokenType::EOL)) {
            continue;
        }
        if (currentToken.type == TokenType::IF) {
            nestedLevel++;
            statements.push_back(parseIfStatement());
            nestedLevel--;
            continue;
        }
        if (currentToken.type == TokenType::STOP || currentToken.type == TokenType::END || currentToken.type == TokenType::ELSE) {
            if (nestedLevel == 0) {
                break;
            }
        }
        statements.push_back(parseStatement());
    }
    return std::make_unique<BlockNode>(std::move(statements));
}

std::unique_ptr<ASTNode> Parser::parseAssignment() {
    std::string varName = std::get<std::string>(currentToken.value);
    advanceToken();  // skip identifier
    advanceToken();  // skip assignment
    auto valueNode = parseExpression_1();
    return std::make_unique<AssignmentNode>(varName, std::move(valueNode));
}

std::unique_ptr<ASTNode> Parser::parseExpression_1() {
    auto node = parseExpression_2();
    while (currentToken.type == TokenType::AND || currentToken.type == TokenType::OR) {
        TokenType op = currentToken.type;
        advanceToken();
        node = std::make_unique<BinaryOpNode>(op, std::move(node), parseExpression_2());
    }
    return node;
}

std::unique_ptr<ASTNode> Parser::parseExpression_2() {
    auto node = parseExpression_3();
    while (currentToken.type == TokenType::EQUAL || currentToken.type == TokenType::NOTEQ
           || currentToken.type == TokenType::GT || currentToken.type == TokenType::GTEQ
           || currentToken.type == TokenType::LT || currentToken.type == TokenType::LTEQ) {
        TokenType op = currentToken.type;
        advanceToken();
        node = std::make_unique<BinaryOpNode>(op, std::move(node), parseExpression_3());
    }
    return node;
}

std::unique_ptr<ASTNode> Parser::parseExpression_3() {
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
           || currentToken.type == TokenType::MOD) {
        TokenType op = currentToken.type;
        advanceToken();
        node = std::make_unique<BinaryOpNode>(op, std::move(node), parseFactor());
    }
    return node;
}

std::unique_ptr<ASTNode> Parser::parseFactor() {
    if (currentToken.type == TokenType::NUMBER) {
        auto node = std::make_unique<NumberNode>(std::get<double>(currentToken.value));
        advanceToken();
        return node;
    }
    if (currentToken.type == TokenType::NOT || currentToken.type == TokenType::UNDERSCORE) {
        TokenType op = currentToken.type;
        advanceToken();
        return std::make_unique<UnaryOpNode>(op, parseFactor());
    }
    if (currentToken.type == TokenType::STRING) {
        auto node = std::make_unique<StringNode>(std::get<std::string>(currentToken.value));
        advanceToken();
        return node;
    }
    if (currentToken.type == TokenType::TRUE || currentToken.type == TokenType::FALSE) {
        auto node = std::make_unique<BoolNode>(std::get<bool>(currentToken.value));
        advanceToken();
        return node;
    }
    if (currentToken.type == TokenType::IDENTIFIER) {
        auto node = std::make_unique<VariableNode>(std::get<std::string>(currentToken.value));
        advanceToken();
        return node;
    }
    if (expectToken(TokenType::LPAREN)) {
        auto node = parseStatement();
        if (!expectToken(TokenType::RPAREN)) {
            throw std::runtime_error("Closing parentheses ')' not found");
        }
        return node;
    }
    if (currentToken.type == TokenType::END) {
        throw std::runtime_error("Unexpected end of expression");
    } else {
        throw std::runtime_error("Unexpected token: " + getTokenTypeName(currentToken.type));
    }
}

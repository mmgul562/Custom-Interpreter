#include "parser.h"


bool Parser::expectToken(TokenType type) {
    if (currentToken.type == type) {
        advanceToken();
        return true;
    }
    return false;
}

void Parser::advanceToken() {
    currentToken = lexer.getNextToken();
}

// DEBUG VERSION
//void Parser::advanceToken() {
//    std::cout << "Advancing from " + getTokenTypeName(currentToken.type);
//    currentToken = lexer.getNextToken();
//    std::cout << " to " + getTokenTypeName(currentToken.type) << std::endl;
//}

bool Parser::isStatementComplete() {
    int nestedLevel = 0;
    Token tempToken = currentToken;
    size_t tempPos = lexer.pos;
    bool checkThen = false;

    while (tempToken.type != TokenType::END) {
        if (tempToken.type == TokenType::IF) {
            checkThen = true;
            nestedLevel++;
        } else if (checkThen && tempToken.type == TokenType::THEN) {
            checkThen = false;
        } else if (tempToken.type == TokenType::STOP) {
            if (nestedLevel == 0) break;
            nestedLevel--;
        } else if (nestedLevel == 0) {
            break;
        }
        tempToken = lexer.getNextToken();
    }
    lexer.pos = tempPos;
    return nestedLevel == 0 || checkThen;
}

// specific parsing

std::unique_ptr<ASTNode> Parser::parseAssignment(std::string name) {
    advanceToken();
    auto valueNode = parseExpression_1();
    return std::make_unique<AssignmentNode>(name, std::move(valueNode));
}

std::unique_ptr<ASTNode> Parser::parseIfStatement() {
    advanceToken();
    if (currentToken.type == TokenType::EOL) {
        throw SyntaxError("Expected condition after 'if'");
    }
    auto condition = parseExpression_1();

    if (!expectToken(TokenType::THEN)) {
        throw SyntaxError("Expected 'then' after if condition");
    }
    auto ifBlock = parseBlock();
    std::unique_ptr<BlockNode> elseBlock = nullptr;

    if (expectToken(TokenType::ELSE)) {
        elseBlock = parseBlock();
    }
    if  (!expectToken(TokenType::STOP)) {
        throw SyntaxError("Expected 'stop' at the end of if statement");
    }
    return std::make_unique<IfElseNode>(std::move(condition), std::move(ifBlock), std::move(elseBlock));
}

std::unique_ptr<BlockNode> Parser::parseBlock() {
    std::vector<std::unique_ptr<ASTNode>> statements;
    int nestedLevel = 0;

    while (true) {
        if (expectToken(TokenType::EOL) || expectToken(TokenType::SEMICOLON)) {
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

std::unique_ptr<ASTNode> Parser::parseList() {
    std::vector<std::unique_ptr<ASTNode>> elements;

    advanceToken();
    while (currentToken.type != TokenType::RBRACKET) {
        elements.push_back(parseExpression_1());
        if (expectToken(TokenType::COMMA)) {
            continue;
        }
        if (currentToken.type != TokenType::RBRACKET) {
            throw SyntaxError("Expected ',' or ']' when creating a list");
        }
    }
    advanceToken();
    return std::make_unique<ListNode>(std::move(elements));
}

std::unique_ptr<ASTNode> Parser::parseListMethodCall(std::unique_ptr<ASTNode> left) {
    advanceToken();
    if (currentToken.type != TokenType::IDENTIFIER) {
        throw SyntaxError("Expected method name after '.'");
    }
    std::string methodName = std::get<std::string>(currentToken.value.asBase());
    advanceToken();

    if (!expectToken(TokenType::LPAREN)) {
        throw SyntaxError("Expected '(' after method name");
    }
    std::vector<std::unique_ptr<ASTNode>> arguments;
    if (currentToken.type != TokenType::RPAREN) {
        do {
            arguments.push_back(parseExpression_1());
        } while (expectToken(TokenType::COMMA));
    }
    if (!expectToken(TokenType::RPAREN)) {
        throw SyntaxError("Expected ')' after method arguments");
    }
    return std::make_unique<ListMethodCallNode>(std::move(left), methodName, std::move(arguments));
}

std::unique_ptr<ASTNode> Parser::parseIndexAccess(std::unique_ptr<ASTNode> left) {
    advanceToken();
    auto index = parseExpression_1();
    if (!expectToken(TokenType::RBRACKET)) {
        throw SyntaxError("Expected ']' after index");
    }
    return std::make_unique<IndexAccessNode>(std::move(left), std::move(index));
}

// general parsing

std::vector<std::unique_ptr<ASTNode>> Parser::parse() {
    std::vector<std::unique_ptr<ASTNode>> statements;
    while (currentToken.type != TokenType::END) {
        if (expectToken(TokenType::EOL) || expectToken(TokenType::SEMICOLON)) {
            continue;
        }
        statements.push_back(parseStatement());
        if (currentToken.type != TokenType::SEMICOLON &&
            currentToken.type != TokenType::EOL) {
            throw SyntaxError("Expected ';' or new line after statement but got " + getTokenTypeName(currentToken.type) + " instead");
        }
    }
    return statements;
}

std::unique_ptr<ASTNode> Parser::parseStatement() {
    if (currentToken.type == TokenType::IF) {
        return parseIfStatement();
    } else if (currentToken.type == TokenType::IDENTIFIER) {
        std::string identifierName = std::get<std::string>(currentToken.value.asBase());
        TokenType nextToken = lexer.peekNextTokenType();

        if (nextToken == TokenType::ASSIGN) {
            advanceToken();
            return parseAssignment(identifierName);
        }
    }
    return parseExpression_1();
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
    TokenType type = currentToken.type;
    if (type == TokenType::LBRACKET) {
        return parseList();
    }
    if (type == TokenType::NOT || type == TokenType::UNDERSCORE) {
        advanceToken();
        return std::make_unique<UnaryOpNode>(type, parseFactor());
    }
    std::unique_ptr<ASTNode> node = nullptr;
    if (type == TokenType::NUMBER) {
        ValueBase value = currentToken.value.asBase();
        node = std::make_unique<NumberNode>(std::get<double>(value));
        advanceToken();
    } else if (type == TokenType::STRING) {
        ValueBase value = currentToken.value.asBase();
        node = std::make_unique<StringNode>(std::get<std::string>(value));
        advanceToken();
    } else if (type == TokenType::TRUE || type == TokenType::FALSE) {
        ValueBase value = currentToken.value.asBase();
        node = std::make_unique<BoolNode>(std::get<bool>(value));
        advanceToken();
    } else if (type == TokenType::IDENTIFIER) {
        ValueBase value = currentToken.value.asBase();
        node = std::make_unique<VariableNode>(std::get<std::string>(value));
        advanceToken();
    } else if (expectToken(TokenType::LPAREN)) {
        node = parseStatement();
        if (!expectToken(TokenType::RPAREN)) {
            throw ParserError("Closing parentheses ')' not found");
        }
    }
    while (currentToken.type == TokenType::LBRACKET || currentToken.type == TokenType::DOT) {
        if (currentToken.type == TokenType::LBRACKET) {
            node = parseIndexAccess(std::move(node));

            if (expectToken(TokenType::ASSIGN)) {
                auto value = parseExpression_1();
                node = std::make_unique<ListAssignmentNode>(std::move(node), std::move(value));
            }
        } else {
            node = parseListMethodCall(std::move(node));
        }
    }
    if (!node) {
        throw ParserError("Unexpected token: " + getTokenTypeName(currentToken.type));
    }
    return node;
}
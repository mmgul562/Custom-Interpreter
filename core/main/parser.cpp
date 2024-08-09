#include "parser.h"
#include "../../util/errors.h"


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
//    std::cout << "Advancing from " + getTypeName(currentToken.type);
//    currentToken = lexer.getNextToken();
//    std::cout << " to " + getTypeName(currentToken.type) << std::endl;
//}

bool Parser::isStatementComplete() {
    int nestedLevel = 0;
    Token tempToken = currentToken;
    TokenType type = tempToken.type;
    size_t tempPos = lexer.pos;
    bool checkThen = false, checkDo = false, checkAs = false;

    while (type != TokenType::END) {
        type = tempToken.type;
        switch (type) {
            case TokenType::IF: {
                checkThen = true;
                ++nestedLevel;
                break;
            }
            case TokenType::FOR :
            case TokenType::WHILE : {
                checkDo = true;
                ++nestedLevel;
                break;
            }
            case TokenType::DEF : {
                checkAs = true;
                ++nestedLevel;
                break;
            }
            case TokenType::THEN : {
                checkThen = false;
                break;
            }
            case TokenType::DO : {
                checkDo = false;
                break;
            }
            case TokenType::AS : {
                checkAs = false;
                break;
            }
            case TokenType::STOP : {
                nestedLevel--;
                break;
            }
            default: {
            }
        }
        if (nestedLevel == 0) break;
        try {
            tempToken = lexer.getNextToken();
        } catch (const LexerError &e) {
            throw;
        }
    }
    lexer.pos = tempPos;
    return nestedLevel == 0 || checkThen || checkDo || checkAs;
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
    if (!expectToken(TokenType::STOP)) {
        throw SyntaxError("Expected 'stop' at the end of if statement");
    }
    return std::make_unique<IfElseNode>(std::move(condition), std::move(ifBlock), std::move(elseBlock));
}

std::unique_ptr<ASTNode> Parser::parseForLoop() {
    advanceToken();
    if (currentToken.type != TokenType::IDENTIFIER) {
        throw SyntaxError("Expected loop-variable name after 'for'");
    }
    std::string variableName = std::get<std::string>(currentToken.value.asBase());

    advanceToken();
    if (!expectToken(TokenType::IN)) {
        throw SyntaxError("Expected 'in' after loop-variable name");
    }

    auto startExpr = parseExpression_1();
    std::unique_ptr<ASTNode> endExpr = nullptr;
    std::unique_ptr<ASTNode> stepExpr = nullptr;
    bool isRangeLoop = false;

    if (expectToken(TokenType::DBL_DOT)) {
        isRangeLoop = true;
        endExpr = parseExpression_1();

        if (expectToken(TokenType::COLON)) {
            stepExpr = parseExpression_1();
        }
    }
    if (!expectToken(TokenType::DO)) {
        throw SyntaxError("Expected 'do' after for loop iterable");
    }
    auto body = parseBlock();

    if (!expectToken(TokenType::STOP)) {
        throw SyntaxError("Expected 'stop' at the end of for loop");
    }
    return std::make_unique<ForLoopNode>(variableName, std::move(startExpr), std::move(endExpr), std::move(stepExpr),
                                         std::move(body), isRangeLoop);
}

std::unique_ptr<ASTNode> Parser::parseWhileLoop() {
    advanceToken();
    if (currentToken.type == TokenType::EOL) {
        throw SyntaxError("Expected condition after 'while'");
    }

    auto condition = parseExpression_1();
    if (!expectToken(TokenType::DO)) {
        throw SyntaxError("Expected 'do' after while condition");
    }

    auto body = parseBlock();
    if (!expectToken(TokenType::STOP)) {
        throw SyntaxError("Expected 'stop' at the end of while loop");
    }
    return std::make_unique<WhileLoopNode>(std::move(condition), std::move(body));
}

std::unique_ptr<BlockNode> Parser::parseBlock() {
    std::vector<std::unique_ptr<ASTNode>> statements;

    while (currentToken.type != TokenType::END) {
        if (expectToken(TokenType::EOL) || expectToken(TokenType::SEMICOLON)) {
            continue;
        }
        if (currentToken.type == TokenType::STOP || currentToken.type == TokenType::ELSE) {
            break;
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

std::unique_ptr<ASTNode> Parser::parseContainerMethodCall(std::unique_ptr<ASTNode> left) {
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
    return std::make_unique<ContainerMethodCallNode>(std::move(left), methodName, std::move(arguments));
}

std::unique_ptr<ASTNode> Parser::parseIndexAccess(std::unique_ptr<ASTNode> left) {
    advanceToken();
    auto index = parseExpression_1();
    if (!expectToken(TokenType::RBRACKET)) {
        throw SyntaxError("Expected ']' after index");
    }
    return std::make_unique<IndexAccessNode>(std::move(left), std::move(index));
}

std::unique_ptr<ASTNode> Parser::parseDict() {
    std::vector<std::pair<std::unique_ptr<ASTNode>, std::unique_ptr<ASTNode>>> elements;

    advanceToken();
    while (currentToken.type != TokenType::RBRACE) {
        auto key = parseExpression_1();
        if (!expectToken(TokenType::COLON)) {
            throw SyntaxError("Expected ':' after key when creating a dictionary");
        }
        auto value = parseExpression_1();
        elements.emplace_back(std::move(key), std::move(value));
        if (expectToken(TokenType::COMMA)) {
            continue;
        }
        if (currentToken.type != TokenType::RBRACE) {
            throw SyntaxError("Expected ',' or '}' when creating a dictionary");
        }
    }
    advanceToken();
    return std::make_unique<DictNode>(std::move(elements));
}

std::unique_ptr<ASTNode> Parser::parseFunctionDeclaration() {
    advanceToken();
    if (currentToken.type != TokenType::IDENTIFIER) {
        throw SyntaxError("Expected function name after 'def'");
    }
    std::string functionName = std::get<std::string>(currentToken.value.asBase());

    advanceToken();
    if (!expectToken(TokenType::LPAREN)) {
        throw SyntaxError("Expected '(' after function name");
    }

    std::vector<std::string> parameters;
    while (currentToken.type != TokenType::RPAREN) {
        if (currentToken.type != TokenType::IDENTIFIER) {
            throw SyntaxError("Expected function parameter name");
        }
        parameters.push_back(std::get<std::string>(currentToken.value.asBase()));
        advanceToken();
        if (currentToken.type == TokenType::RPAREN) break;
        if (!expectToken(TokenType::COMMA)) {
            throw SyntaxError("Expected ',' between function parameters");
        }
    }
    if (!expectToken(TokenType::RPAREN)) {
        throw SyntaxError("Expected ')' after function parameters' names");
    }
    if (!expectToken(TokenType::AS)) {
        throw SyntaxError("Expected 'as' after function parameters");
    }
    auto body = parseBlock();
    if (!expectToken(TokenType::STOP)) {
        throw SyntaxError("Expected 'stop' after function body");
    }
    return std::make_unique<FunctionDeclarationNode>(functionName, std::move(parameters), std::move(body));
}

std::unique_ptr<ASTNode> Parser::parseFunctionCall(const std::string &name) {
    advanceToken();
    std::vector<std::unique_ptr<ASTNode>> arguments;
    while (currentToken.type != TokenType::RPAREN) {
        arguments.push_back(parseExpression_1());
        if (currentToken.type == TokenType::RPAREN) break;
        if (!expectToken(TokenType::COMMA)) {
            throw SyntaxError("Expected ',' between function arguments");
        }
    }
    if (!expectToken(TokenType::RPAREN)) {
        throw SyntaxError("Expected ')' after function arguments");
    }
    return std::make_unique<FunctionCallNode>(name, std::move(arguments));
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
            throw SyntaxError(
                    "Expected ';' or new line after statement but got " + getTypeName(currentToken.type) + " instead");
        }
    }
    return statements;
}

std::unique_ptr<ASTNode> Parser::parseStatement() {
    TokenType type = currentToken.type;
    switch (type) {
        case TokenType::IF:
            return parseIfStatement();
        case TokenType::DEF:
            return parseFunctionDeclaration();
        case TokenType::FOR:
            return parseForLoop();
        case TokenType::WHILE:
            return parseWhileLoop();
        case TokenType::BREAK:
            advanceToken();
            return std::make_unique<ControlFlowNode>(true);
        case TokenType::CONTINUE:
            advanceToken();
            return std::make_unique<ControlFlowNode>(false);
        case TokenType::RETURN:
            advanceToken();
            if (currentToken.type == TokenType::SEMICOLON || currentToken.type == TokenType::EOL) {
                return std::make_unique<ReturnNode>(nullptr);
            }
            return std::make_unique<ReturnNode>(parseExpression_1());
        case TokenType::IDENTIFIER: {
            std::string identifierName = std::get<std::string>(currentToken.value.asBase());
            TokenType nextType = lexer.peekNextTokenType();
            if (nextType == TokenType::ASSIGN) {
                advanceToken();
                return parseAssignment(identifierName);
            } else if (nextType == TokenType::LPAREN) {
                advanceToken();
                return parseFunctionCall(identifierName);
            }
        }
        default:
            return parseExpression_1();
    }
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
    } else if (type == TokenType::LBRACE) {
        return parseDict();
    } else if (type == TokenType::NOT || type == TokenType::UNDERSCORE || type == TokenType::QUOTE
               || type == TokenType::HASH || type == TokenType::QMARK || type == TokenType::MINUS) {
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
        ValueBase base = currentToken.value.asBase();
        node = std::make_unique<VariableNode>(std::get<std::string>(base));
        advanceToken();
        while (currentToken.type == TokenType::LBRACKET || currentToken.type == TokenType::DOT) {
            if (currentToken.type == TokenType::LBRACKET) {
                node = parseIndexAccess(std::move(node));

                if (expectToken(TokenType::ASSIGN)) {
                    auto value = parseExpression_1();
                    node = std::make_unique<IndexAssignmentNode>(std::move(node), std::move(value));
                }
            } else {
                node = parseContainerMethodCall(std::move(node));
            }
        }
    } else if (expectToken(TokenType::LPAREN)) {
        node = parseStatement();
        if (!expectToken(TokenType::RPAREN)) {
            throw SyntaxError("Expected closing parentheses ')' but got " + getTypeName(currentToken.type) + " instead");
        }
    }
    if (!node) {
        throw ParserError("Unexpected token: " + getTypeName(currentToken.type));
    }
    return node;
}
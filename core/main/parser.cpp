#include "parser.h"
#include "../../util/errors.h"


bool Parser::expectToken(TokenType type) {
    if (getType() == type) {
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
    TokenType type = tempToken.getType();
    size_t tempPos = lexer.pos;
    bool checkThen = false, checkDo = false, checkAs = false;

    while (type != TokenType::END) {
        type = tempToken.getType();
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
                if (nestedLevel == 0) {
                    lexer.pos = tempPos;
                    return true;
                }
                nestedLevel--;
                break;
            }
            default: {
            }
        }
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

std::unique_ptr<ASTNode> Parser::parseAssignment(const std::string &name, bool reassign) {
    advanceToken();
    auto valueNode = parseStatement();
    return std::make_unique<AssignmentNode>(name, reassign, std::move(valueNode));
}


std::unique_ptr<ASTNode> Parser::parseList() {
    std::vector<std::unique_ptr<ASTNode>> elements;

    advanceToken();
    while (getType() != TokenType::RBRACKET) {
        elements.push_back(parseLogicalAndOr());
        if (expectToken(TokenType::COMMA)) {
            continue;
        }
        if (getType() != TokenType::RBRACKET) {
            throw SyntaxError("Expected ',' or ']' when creating a list");
        }
    }
    advanceToken();
    return std::make_unique<ListNode>(std::move(elements));
}


std::unique_ptr<ASTNode> Parser::parseDict() {
    std::vector<std::pair<std::unique_ptr<ASTNode>, std::unique_ptr<ASTNode>>> elements;

    advanceToken();
    while (getType() != TokenType::RBRACE) {
        auto key = parseLogicalAndOr();
        if (!expectToken(TokenType::COLON)) {
            throw SyntaxError("Expected ':' after key when creating a dictionary");
        }
        auto value = parseLogicalAndOr();
        elements.emplace_back(std::move(key), std::move(value));
        if (expectToken(TokenType::COMMA)) {
            continue;
        }
        if (getType() != TokenType::RBRACE) {
            throw SyntaxError("Expected ',' or '}' when creating a dictionary");
        }
    }
    advanceToken();
    return std::make_unique<DictNode>(std::move(elements));
}


std::unique_ptr<ASTNode> Parser::parseIndexAccess(std::unique_ptr<ASTNode> left) {
    advanceToken();
    auto index = parseLogicalAndOr();
    if (!expectToken(TokenType::RBRACKET)) {
        throw SyntaxError("Expected ']' after index");
    }
    return std::make_unique<IndexAccessNode>(std::move(left), std::move(index));
}


std::unique_ptr<ASTNode> Parser::parseMethodCall(std::unique_ptr<ASTNode> left) {
    advanceToken();
    if (getType() != TokenType::IDENTIFIER) {
        throw SyntaxError("Expected method name after '.'");
    }
    std::string methodName = std::get<std::string>(currentToken.getValue().asBase());
    advanceToken();

    if (!expectToken(TokenType::LPAREN)) {
        throw SyntaxError("Expected '(' after method name");
    }
    std::vector<std::unique_ptr<ASTNode>> arguments;
    if (getType() != TokenType::RPAREN) {
        do {
            arguments.push_back(parseLogicalAndOr());
        } while (expectToken(TokenType::COMMA));
    }
    if (!expectToken(TokenType::RPAREN)) {
        throw SyntaxError("Expected ')' after method arguments");
    }
    return std::make_unique<MethodCallNode>(std::move(left), methodName, std::move(arguments));
}


std::unique_ptr<ASTNode> Parser::parseIfStatement() {
    advanceToken();
    if (getType() == TokenType::EOL) {
        throw SyntaxError("Expected condition after 'if'");
    }
    auto condition = parseLogicalAndOr();

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
    if (getType() != TokenType::IDENTIFIER) {
        throw SyntaxError("Expected loop-variable name after 'for'");
    }
    std::string variableName = std::get<std::string>(currentToken.getValue().asBase());

    advanceToken();
    if (!expectToken(TokenType::IN)) {
        throw SyntaxError("Expected 'in' after loop-variable name");
    }

    auto startExpr = parseLogicalAndOr();
    std::unique_ptr<ASTNode> endExpr = nullptr;
    std::unique_ptr<ASTNode> stepExpr = nullptr;
    bool isRangeLoop = false;

    if (expectToken(TokenType::DBL_DOT)) {
        isRangeLoop = true;
        endExpr = parseLogicalAndOr();

        if (expectToken(TokenType::COLON)) {
            stepExpr = parseLogicalAndOr();
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
    if (getType() == TokenType::EOL) {
        throw SyntaxError("Expected condition after 'while'");
    }

    auto condition = parseLogicalAndOr();
    if (!expectToken(TokenType::DO)) {
        throw SyntaxError("Expected 'do' after while condition");
    }

    auto body = parseBlock();
    if (!expectToken(TokenType::STOP)) {
        throw SyntaxError("Expected 'stop' at the end of while loop");
    }
    return std::make_unique<WhileLoopNode>(std::move(condition), std::move(body));
}


std::unique_ptr<ASTNode> Parser::parseFunctionDeclaration() {
    advanceToken();
    if (getType() != TokenType::IDENTIFIER) {
        throw SyntaxError("Expected function name after 'def'");
    }
    std::string functionName = std::get<std::string>(currentToken.getValue().asBase());
    bool hasArgs = false;

    advanceToken();
    if (!expectToken(TokenType::LPAREN)) {
        throw SyntaxError("Expected '(' after function name");
    }

    std::vector<std::string> parameters;
    while (getType() != TokenType::RPAREN && !hasArgs) {
        if (expectToken(TokenType::DBL_DOT)) {
            hasArgs = true;
        }
        if (getType() != TokenType::IDENTIFIER) {
            throw SyntaxError("Expected function parameter name");
        }
        parameters.push_back(std::get<std::string>(currentToken.getValue().asBase()));
        advanceToken();
        if (getType() == TokenType::RPAREN) break;
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
    return std::make_unique<FunctionDeclarationNode>(functionName, std::move(parameters), hasArgs, std::move(body));
}


std::unique_ptr<ASTNode> Parser::parseFunctionCall(const std::string &name) {
    advanceToken();
    std::vector<std::unique_ptr<ASTNode>> arguments;
    while (getType() != TokenType::RPAREN) {
        arguments.push_back(parseLogicalAndOr());
        if (getType() == TokenType::RPAREN) break;
        if (!expectToken(TokenType::COMMA)) {
            throw SyntaxError("Expected ',' between function arguments");
        }
    }
    if (!expectToken(TokenType::RPAREN)) {
        throw SyntaxError("Expected ')' after function arguments");
    }
    return std::make_unique<FunctionCallNode>(name, std::move(arguments));
}


std::unique_ptr<BlockNode> Parser::parseBlock() {
    std::vector<std::unique_ptr<ASTNode>> statements;

    while (getType() != TokenType::END) {
        if (expectToken(TokenType::EOL) || expectToken(TokenType::SEMICOLON)) {
            continue;
        }
        if (getType() == TokenType::STOP || getType() == TokenType::ELSE) {
            break;
        }
        statements.push_back(parseStatement());
    }
    return std::make_unique<BlockNode>(std::move(statements));
}

// general parsing

std::vector<std::unique_ptr<ASTNode>> Parser::parse() {
    std::vector<std::unique_ptr<ASTNode>> statements;
    while (getType() != TokenType::END) {
        if (expectToken(TokenType::EOL) || expectToken(TokenType::SEMICOLON)) {
            continue;
        }
        statements.push_back(parseStatement());
        if (getType() != TokenType::SEMICOLON &&
            getType() != TokenType::EOL) {
            throw SyntaxError(
                    "Expected ';' or new line after statement but got " + getTypeName(getType()) + " instead");
        }
    }
    return statements;
}

std::unique_ptr<ASTNode> Parser::parseStatement() {
    TokenType type = getType();
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
            if (getType() == TokenType::SEMICOLON || getType() == TokenType::EOL) {
                return std::make_unique<ReturnNode>(nullptr);
            }
            return std::make_unique<ReturnNode>(parseLogicalAndOr());
        case TokenType::IDENTIFIER: {
            std::string identifierName = std::get<std::string>(currentToken.getValue().asBase());
            TokenType nextType = lexer.peekNextTokenType();
            if (nextType == TokenType::ASSIGN) {
                advanceToken();
                return parseAssignment(identifierName, true);
            } else if (nextType == TokenType::ASSIGN_NEW) {
                advanceToken();
                return parseAssignment(identifierName, false);
            }
        }
        default:
            return parseLogicalAndOr();
    }
}

std::unique_ptr<ASTNode> Parser::parseLogicalAndOr() {
    auto node = parseComparison();
    while (getType() == TokenType::AND || getType() == TokenType::OR) {
        TokenType op = getType();
        advanceToken();
        node = std::make_unique<BinaryOpNode>(op, std::move(node), parseComparison());
    }
    return node;
}

std::unique_ptr<ASTNode> Parser::parseComparison() {
    auto node = parseMathOpLowOrder();
    while (getType() == TokenType::EQUAL || getType() == TokenType::NOTEQ
           || getType() == TokenType::GT || getType() == TokenType::GTEQ
           || getType() == TokenType::LT || getType() == TokenType::LTEQ) {
        TokenType op = getType();
        advanceToken();
        node = std::make_unique<BinaryOpNode>(op, std::move(node), parseMathOpLowOrder());
    }
    return node;
}

std::unique_ptr<ASTNode> Parser::parseMathOpLowOrder() {
    auto node = parseMathOpHighOrder();
    while (getType() == TokenType::PLUS || getType() == TokenType::MINUS) {
        TokenType op = getType();
        advanceToken();
        node = std::make_unique<BinaryOpNode>(op, std::move(node), parseMathOpHighOrder());
    }
    return node;
}

std::unique_ptr<ASTNode> Parser::parseMathOpHighOrder() {
    auto node = parseTypeCast();
    while (getType() == TokenType::ASTER || getType() == TokenType::SLASH
           || getType() == TokenType::DBL_ASTER || getType() == TokenType::DBL_SLASH
           || getType() == TokenType::MOD) {
        TokenType op = getType();
        advanceToken();
        node = std::make_unique<BinaryOpNode>(op, std::move(node), parseTypeCast());
    }
    return node;
}

std::unique_ptr<ASTNode> Parser::parseTypeCast() {
    auto node = parseFactor();
    while (expectToken(TokenType::AS)) {
        node = std::make_unique<TypeCastNode>(getType(), std::move(node));
        advanceToken();
    }
    return node;
}

std::unique_ptr<ASTNode> Parser::parseFactor() {
    TokenType type = getType();
    if (type == TokenType::LBRACKET) {
        return parseList();
    } else if (type == TokenType::LBRACE) {
        return parseDict();
    } else if (type == TokenType::NOT || type == TokenType::UNDERSCORE ||
               type == TokenType::QMARK || type == TokenType::MINUS) {
        advanceToken();
        return std::make_unique<UnaryOpNode>(type, parseFactor());
    }

    std::unique_ptr<ASTNode> node = nullptr;
    if (currentToken.getValue().isBase()) {
        ValueBase value = currentToken.getValue().asBase();
        if (type == TokenType::FLOAT) {
            node = std::make_unique<FloatNode>(std::get<double>(value));
            advanceToken();
        } else if (type == TokenType::INT) {
            node = std::make_unique<IntNode>(std::get<long>(value));
            advanceToken();
        } else if (type == TokenType::STRING) {
            node = std::make_unique<StringNode>(std::get<std::string>(value));
            advanceToken();
        } else if (type == TokenType::TRUE || type == TokenType::FALSE) {
            node = std::make_unique<BoolNode>(std::get<bool>(value));
            advanceToken();
        } else if (type == TokenType::IDENTIFIER) {
            node = std::make_unique<VariableNode>(std::get<std::string>(value));
            advanceToken();
            if (getType() == TokenType::LPAREN) {
                node = parseFunctionCall(std::get<std::string>(value));
            } else {
                while (getType() == TokenType::LBRACKET || getType() == TokenType::DOT) {
                    if (getType() == TokenType::LBRACKET) {
                        node = parseIndexAccess(std::move(node));

                        if (expectToken(TokenType::ASSIGN)) {
                            auto val = parseLogicalAndOr();
                            node = std::make_unique<IndexAssignmentNode>(std::move(node), std::move(val));
                        }
                    } else {
                        node = parseMethodCall(std::move(node));
                    }
                }
            }
        }
    } else if (expectToken(TokenType::LPAREN)) {
        node = parseStatement();
        if (!expectToken(TokenType::RPAREN)) {
            throw SyntaxError(
                    "Expected closing parentheses ')' but got " + getTypeName(getType()) + " instead");
        }
    }
    if (!node) {
        throw ParserError("Unexpected token: " + getTypeName(getType()));
    }
    return node;
}

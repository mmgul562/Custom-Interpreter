#ifndef CPP_INTERPRETER_PARSER_H
#define CPP_INTERPRETER_PARSER_H

#include "ast.h"


class Parser {
private:
    Lexer &lexer;
    std::shared_ptr<Scope> currentScope;

public:
    Token currentToken;

private:
    bool expectToken(TokenType type);

    std::unique_ptr<ASTNode> parseAssignment(const std::string &name, bool reassign);

    std::unique_ptr<ASTNode> parseList();

    std::unique_ptr<ASTNode> parseDict();

    std::unique_ptr<ASTNode> parseIndexAccess(std::unique_ptr<ASTNode> left);

    std::unique_ptr<ASTNode> parseMethodCall(std::unique_ptr<ASTNode> left);

    std::unique_ptr<ASTNode> parseIfStatement();

    std::unique_ptr<ASTNode> parseForLoop();

    std::unique_ptr<ASTNode> parseWhileLoop();

    std::unique_ptr<ASTNode> parseFunctionDeclaration();

    std::unique_ptr<ASTNode> parseFunctionCall(const std::string &name);

    std::unique_ptr<BlockNode> parseBlock();

    std::unique_ptr<ASTNode> parseStatement();

    std::unique_ptr<ASTNode> parseLogicalAndOr();

    std::unique_ptr<ASTNode> parseComparison();

    std::unique_ptr<ASTNode> parseMathOpLowOrder();

    std::unique_ptr<ASTNode> parseMathOpHighOrder();

    std::unique_ptr<ASTNode> parseTypeCast();

    std::unique_ptr<ASTNode> parseFactor();

public:
    explicit Parser(Lexer &lexer) : lexer(lexer), currentToken(lexer.getNextToken()),
                                    currentScope(std::make_shared<Scope>()) {}

    void advanceToken();

    bool isStatementComplete();

    TokenType getType() const { return currentToken.getType(); }

    std::vector<std::unique_ptr<ASTNode>> parse();
};

#endif

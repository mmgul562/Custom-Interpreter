#ifndef CPP_INTERPRETER_PARSER_H
#define CPP_INTERPRETER_PARSER_H

#include "ast.h"


class Parser {
private:
    Lexer &lexer;
    std::shared_ptr<Scope> currentScope;

    std::unique_ptr<ASTNode> parseAssignment(std::string name);
    std::unique_ptr<ASTNode> parseIfStatement();
    std::unique_ptr<ASTNode> parseForLoop();
    std::unique_ptr<ASTNode> parseWhileLoop();
    std::unique_ptr<BlockNode> parseBlock();
    std::unique_ptr<ASTNode> parseList();
    std::unique_ptr<ASTNode> parseIndexAccess(std::unique_ptr<ASTNode> left);
    std::unique_ptr<ASTNode> parseContainerMethodCall(std::unique_ptr<ASTNode> left);
    std::unique_ptr<ASTNode> parseDict();
    std::unique_ptr<ASTNode> parseFunctionDeclaration();
    std::unique_ptr<ASTNode> parseFunctionCall(const std::string& name);
    std::unique_ptr<ASTNode> parseStatement();
    std::unique_ptr<ASTNode> parseExpression_1();
    std::unique_ptr<ASTNode> parseExpression_2();
    std::unique_ptr<ASTNode> parseExpression_3();
    std::unique_ptr<ASTNode> parseTerm();
    std::unique_ptr<ASTNode> parseFactor();

public:
    Token currentToken = lexer.getNextToken();

    explicit Parser(Lexer &lexer) : lexer(lexer), currentScope(std::make_shared<Scope>()) {}

    void advanceToken();
    bool expectToken(TokenType type);
    bool isStatementComplete();
    std::vector<std::unique_ptr<ASTNode>> parse();
};

#endif

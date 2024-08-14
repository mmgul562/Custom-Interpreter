#ifndef CPP_INTERPRETER_FUNCTIONS_H
#define CPP_INTERPRETER_FUNCTIONS_H

#include <iostream>


class ASTNode;
class Scope;
class Value;

// functions

Value print(const std::vector<std::unique_ptr<ASTNode>> &arguments, std::shared_ptr<Scope> &scope);

Value type(const std::vector<std::unique_ptr<ASTNode>> &arguments, std::shared_ptr<Scope> &scope);

Value roundf(const std::vector<std::unique_ptr<ASTNode>> &arguments, std::shared_ptr<Scope> &scope);

Value roundi(const std::vector<std::unique_ptr<ASTNode>> &arguments, std::shared_ptr<Scope> &scope);

Value floori(const std::vector<std::unique_ptr<ASTNode>> &arguments, std::shared_ptr<Scope> &scope);

Value ceili(const std::vector<std::unique_ptr<ASTNode>> &arguments, std::shared_ptr<Scope> &scope);

// methods

Value listlen(Value &caller, const std::vector<std::unique_ptr<ASTNode>> &arguments);

void listappend(Value &caller, const std::vector<std::unique_ptr<ASTNode>> &arguments, std::shared_ptr<Scope> &scope);

void listremove(Value &caller, const std::vector<std::unique_ptr<ASTNode>> &arguments, std::shared_ptr<Scope> &scope);

void listput(Value &caller, const std::vector<std::unique_ptr<ASTNode>> &arguments, std::shared_ptr<Scope> &scope);

Value dictsize(Value &caller, const std::vector<std::unique_ptr<ASTNode>> &arguments);

Value dictexists(Value &caller, const std::vector<std::unique_ptr<ASTNode>> &arguments, std::shared_ptr<Scope> &scope);

void dictremove(Value &caller, const std::vector<std::unique_ptr<ASTNode>> &arguments, std::shared_ptr<Scope> &scope);

Value slen(Value &caller, const std::vector<std::unique_ptr<ASTNode>> &arguments);

void sltrim(Value &caller, const std::vector<std::unique_ptr<ASTNode>> &arguments, std::shared_ptr<Scope> &scope);

void srtrim(Value &caller, const std::vector<std::unique_ptr<ASTNode>> &arguments, std::shared_ptr<Scope> &scope);


#endif
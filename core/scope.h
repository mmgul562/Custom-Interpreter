#ifndef CPP_INTERPRETER_SCOPE_H
#define CPP_INTERPRETER_SCOPE_H

#include "value.h"
#include <unordered_map>


class FunctionDeclarationNode;

class Scope : public std::enable_shared_from_this<Scope> {
private:
    std::unordered_map<std::string, Value> variables;
    std::unordered_map<std::string, std::shared_ptr<FunctionDeclarationNode>> functions;
    std::shared_ptr<Scope> parent;

public:
    Scope() : parent(nullptr) {}

    explicit Scope(std::shared_ptr<Scope> parent) : parent(std::move(parent)) {}

    void setVariable(const std::string &name, const Value &value);

    bool hasVariable(const std::string &name) const;

    bool hasVariableInCurrentOrParentScope(const std::string &name) const;

    Value getVariable(const std::string &name) const;

    void assignVariable(const std::string &name, const Value &value);

    void setFunction(const std::string &name, std::shared_ptr<FunctionDeclarationNode> func);

    std::shared_ptr<FunctionDeclarationNode> getFunction(const std::string &name) const;

    std::shared_ptr<Scope> createChildScope();
};

#endif

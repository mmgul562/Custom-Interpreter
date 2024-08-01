#ifndef CPP_INTERPRETER_SCOPE_H
#define CPP_INTERPRETER_SCOPE_H

#include "lexer.h"
#include <unordered_map>


class Scope : public std::enable_shared_from_this<Scope> {
private:
    std::unordered_map<std::string, Value> variables;
    std::shared_ptr<Scope> parent;

public:
    Scope() : parent(nullptr) {}
    explicit Scope(std::shared_ptr<Scope> parent) : parent(std::move(parent)) {}

    void setVariable(const std::string& name, const Value& value) {
        variables[name] = value;
    }

    bool hasVariable(const std::string& name) const {
        return variables.find(name) != variables.end();
    }

    bool hasVariableInCurrentOrParentScope(const std::string& name) const {
        if (hasVariable(name)) {
            return true;
        }
        if (parent) {
            return parent->hasVariableInCurrentOrParentScope(name);
        }
        return false;
    }

    Value getVariable(const std::string& name) const {
        auto it = variables.find(name);
        if (it != variables.end()) {
            return it->second;
        }
        if (parent) {
            return parent->getVariable(name);
        }
        throw std::runtime_error("Undefined variable: " + name);
    }

    void assignVariable(const std::string& name, const Value& value) {
        if (hasVariable(name)) {
            variables[name] = value;
        } else if (parent) {
            parent->assignVariable(name, value);
        } else {
            throw std::runtime_error("Undefined variable: " + name);
        }
    }

    std::shared_ptr<Scope> createChildScope() {
        return std::make_shared<Scope>(shared_from_this());
    }
};

#endif

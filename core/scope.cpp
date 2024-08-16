#include "../util/errors.h"
#include "main/ast.h"
#include "scope.h"
#include <utility>


void Scope::setVariable(const std::string& name, const Value& value) {
    variables[name] = value;
}


bool Scope::hasVariable(const std::string& name) const {
    return variables.find(name) != variables.end();
}


bool Scope::hasVariableInCurrentOrParentScope(const std::string& name) const {
    if (hasVariable(name)) {
        return true;
    }
    if (parent) {
        return parent->hasVariableInCurrentOrParentScope(name);
    }
    return false;
}


Value Scope::getVariable(const std::string& name) const {
    auto it = variables.find(name);
    if (it != variables.end()) {
        return it->second;
    }
    if (parent) {
        return parent->getVariable(name);
    }
    throw NameError("Niezidentyfikowana zmienna: " + name);
}


void Scope::assignVariable(const std::string& name, const Value& value) {
    if (hasVariable(name)) {
        variables[name] = value;
    } else if (parent) {
        parent->assignVariable(name, value);
    } else {
        throw NameError("Niezidentyfikowana zmienna: " + name);
    }
}


void Scope::setFunction(const std::string& name, std::shared_ptr<FunctionDeclarationNode> func) {
    functions[name] = std::move(func);
}


std::shared_ptr<FunctionDeclarationNode> Scope::getFunction(const std::string& name) const {
    auto it = functions.find(name);
    if (it != functions.end()) {
        return it->second;
    }
    if (parent) {
        return parent->getFunction(name);
    }
    return nullptr;
}


std::shared_ptr<Scope> Scope::createChildScope() {
    return std::make_shared<Scope>(shared_from_this());
}

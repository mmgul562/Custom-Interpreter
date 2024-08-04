#include "scope.h"
#include "../util/errors.h"


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
    throw NameError("Undefined variable: " + name);
}

void Scope::assignVariable(const std::string& name, const Value& value) {
    if (hasVariable(name)) {
        variables[name] = value;
    } else if (parent) {
        parent->assignVariable(name, value);
    } else {
        throw NameError("Undefined variable: " + name);
    }
}

std::shared_ptr<Scope> Scope::createChildScope() {
    return std::make_shared<Scope>(shared_from_this());
}

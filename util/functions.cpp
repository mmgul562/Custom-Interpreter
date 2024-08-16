#include "../core/main/ast.h"
#include "utf8string.h"
#include "functions.h"
#include "errors.h"
#include <cmath>

#define CYAN "\x1B[36m"
#ifndef RST
#define RST  "\x1B[0m"
#endif

// functions

Value print(const std::vector<std::unique_ptr<ASTNode>> &arguments, std::shared_ptr<Scope> &scope) {
    size_t size = arguments.size();
    std::cout << CYAN;
    for (size_t i = 0; i < size; ++i) {
        Value argValue = arguments[i]->evaluate(scope);
        printValue(argValue, false);
        if (i < size - 1) {
            std::cout << " ";
        }
    }
    std::cout << RST << std::endl;
    return Value();
}


Value type(const std::vector<std::unique_ptr<ASTNode>> &arguments, std::shared_ptr<Scope> &scope) {
    if (arguments.size() != 1) {
        throw ValueError("Function type() expects exactly 1 argument, but got " + std::to_string(arguments.size()));
    }
    Value val = arguments[0]->evaluate(scope);
    if (val.isBase()) {
        ValueBase base = val.asBase();
        if (std::holds_alternative<double>(base)) return Value("float");
        if (std::holds_alternative<long>(base)) return Value("int");
        if (std::holds_alternative<bool>(base)) return Value("bool");
        if (std::holds_alternative<std::string>(base)) return Value("str");
    } else if (val.isList()) {
        return Value("list");
    } else if (val.isDict()) {
        return Value("dict");
    }
    return Value("null");
}


Value roundf(const std::vector<std::unique_ptr<ASTNode>> &arguments, std::shared_ptr<Scope> &scope) {
    if (arguments.size() != 2) {
        throw ValueError("Function roundf() expects exactly 2 arguments, but got " + std::to_string(arguments.size()));
    }
    Value precision = arguments[1]->evaluate(scope);
    if (!precision.isBase() && !std::holds_alternative<long>(precision.asBase())) {
        throw TypeError("Rounding precision must be an integer");
    }
    Value val = arguments[0]->evaluate(scope);
    if (!val.isBase() || !std::holds_alternative<double>(val.asBase())) {
        throw TypeError("Rounding can only be performed on float types");
    }
    long prec = std::get<long>(precision.asBase());
    if (prec < 0) {
        throw ValueError("Rounding precision cannot be negative");
    }
    double coef = std::pow(10, prec);
    double n = std::get<double>(val.asBase());
    return Value(std::round((n * coef)) / coef);
}


Value roundi(const std::vector<std::unique_ptr<ASTNode>> &arguments, std::shared_ptr<Scope> &scope) {
    if (arguments.size() != 1) {
        throw ValueError("Function round() expects exactly 1 argument, but got " + std::to_string(arguments.size()));
    }
    Value val = arguments[0]->evaluate(scope);
    if (!val.isBase() || !std::holds_alternative<double>(val.asBase())) {
        throw TypeError("Rounding can only be performed on float types");
    }
    return Value(static_cast<long>(std::round(std::get<double>(val.asBase()))));
}


Value floori(const std::vector<std::unique_ptr<ASTNode>> &arguments, std::shared_ptr<Scope> &scope) {
    if (arguments.size() != 1) {
        throw ValueError("Function floor() expects exactly 1 argument, but got " + std::to_string(arguments.size()));
    }
    Value val = arguments[0]->evaluate(scope);
    if (!val.isBase() || !std::holds_alternative<double>(val.asBase())) {
        throw TypeError("Rounding can only be performed on float types");
    }
    return Value(static_cast<long>(std::floor(std::get<double>(val.asBase()))));
}


Value ceili(const std::vector<std::unique_ptr<ASTNode>> &arguments, std::shared_ptr<Scope> &scope) {
    if (arguments.size() != 1) {
        throw ValueError("Function ceil() expects exactly 1 argument, but got " + std::to_string(arguments.size()));
    }
    Value val = arguments[0]->evaluate(scope);
    if (!val.isBase() || !std::holds_alternative<double>(val.asBase())) {
        throw TypeError("Rounding can only be performed on float types");
    }
    return Value(static_cast<long>(std::ceil(std::get<double>(val.asBase()))));
}

// methods

Value listlen(Value& caller, const std::vector<std::unique_ptr<ASTNode>>& arguments) {
    if (!arguments.empty()) {
        throw ValueError("Method len() doesn't expect any arguments");
    }
    return Value(static_cast<long>(caller.asList().size()));
}


void listappend(Value& caller, const std::vector<std::unique_ptr<ASTNode>> &arguments, std::shared_ptr<Scope> &scope) {
    if (arguments.size() != 1) {
        throw ValueError("Method append() expects exactly 1 argument");
    }
    Value argValue = arguments[0]->evaluate(scope);
    caller.asList().push_back(std::make_shared<Value>(argValue));
}


void listremove(Value& caller, const std::vector<std::unique_ptr<ASTNode>> &arguments, std::shared_ptr<Scope> &scope) {
    if (arguments.size() != 1) {
        throw ValueError("Method remove() expects exactly 1 argument");
    }
    Value indexValue = arguments[0]->evaluate(scope);
    if (!indexValue.isBase() || !std::holds_alternative<long>(indexValue.asBase())) {
        throw TypeError("remove() method's argument must be an integer");
    }
    long idx = std::get<long>(indexValue.asBase());
    if (idx >= caller.asList().size() || idx < 0) {
        throw IndexError("Cannot remove: index (" + std::to_string(idx) + ") out of range");
    }
    caller.asList().erase(caller.asList().begin() + idx);
}


void listput(Value& caller, const std::vector<std::unique_ptr<ASTNode>> &arguments, std::shared_ptr<Scope> &scope) {
    if (arguments.size() != 2) {
        throw ValueError("Method put() expects exactly 2 arguments");
    }
    Value indexValue = arguments[0]->evaluate(scope);
    if (!indexValue.isBase() || !std::holds_alternative<long>(indexValue.asBase())) {
        throw TypeError("put() method's first argument must be an integer");
    }
    long idx = std::get<long>(indexValue.asBase());
    Value argValue = arguments[1]->evaluate(scope);
    if (idx > caller.asList().size() || idx < 0) {
        throw IndexError("Cannot put: index (" + std::to_string(idx) + ") out of range");
    }
    caller.asList().insert(caller.asList().begin() + idx, std::make_shared<Value>(argValue));
}


Value dictsize(Value& caller, const std::vector<std::unique_ptr<ASTNode>> &arguments) {
    if (!arguments.empty()) {
        throw ValueError("Method size() doesn't expect any arguments");
    }
    return Value(static_cast<long>(caller.asDict().size()));
}


Value dictexists(Value& caller, const std::vector<std::unique_ptr<ASTNode>> &arguments, std::shared_ptr<Scope> &scope) {
    if (arguments.size() != 1) {
        throw ValueError("Method exists() expects exactly 1 argument");
    }
    Value keyValue = arguments[0]->evaluate(scope);
    if (!keyValue.isBase()) {
        throw TypeError("Dictionary key must be a basic type");
    }
    return Value(caller.asDict().find(keyValue.asBase()) != caller.asDict().end());
}


void dictremove(Value& caller, const std::vector<std::unique_ptr<ASTNode>> &arguments, std::shared_ptr<Scope> &scope) {
    if (arguments.size() != 1) {
        throw ValueError("Method remove() expects exactly 1 argument");
    }
    Value keyValue = arguments[0]->evaluate(scope);
    if (!keyValue.isBase()) {
        throw TypeError("Dictionary key must be a basic type");
    } else if (caller.asDict().erase(keyValue.asBase()) == 0) {
        throw NameError("Dictionary key not found");
    }
}


Value slen(Value& caller, const std::vector<std::unique_ptr<ASTNode>> &arguments) {
    if (!arguments.empty()) {
        throw ValueError("Method len() doesn't expect any arguments");
    }
    return Value(static_cast<long>(getStrLen(std::get<std::string>(caller.asBase()))));
}


void sltrim(Value& caller, const std::vector<std::unique_ptr<ASTNode>> &arguments, std::shared_ptr<Scope> &scope) {
    if (arguments.size() != 1) {
        throw ValueError("Method ltrim() expects exactly 1 argument");
    }
    Value argValue = arguments[0]->evaluate(scope);
    if (!argValue.isBase() || !std::holds_alternative<std::string>(argValue.asBase())) {
        throw TypeError("ltrim() method's argument must be a string");
    }
    std::string trimChars = std::get<std::string>(argValue.asBase());
    std::string base = std::get<std::string>(caller.asBase());
    std::string currChar;
    size_t start = 0, bsize = base.size(), i = 0;
    while (start < bsize) {
        currChar = getStrChar(base, i);
        if (trimChars.find(currChar) == std::string::npos) {
            break;
        }
        start += currChar.size();
        ++i;
    }
    caller = Value(base.substr(start));
}


void srtrim(Value& caller, const std::vector<std::unique_ptr<ASTNode>> &arguments, std::shared_ptr<Scope> &scope) {
    if (arguments.size() != 1) {
        throw ValueError("Method rtrim() expects exactly 1 argument");
    }
    Value argValue = arguments[0]->evaluate(scope);
    if (!argValue.isBase() || !std::holds_alternative<std::string>(argValue.asBase())) {
        throw TypeError("rtrim() method's argument must be a string");
    }
    std::string trimChars = std::get<std::string>(argValue.asBase());
    std::string base = std::get<std::string>(caller.asBase());
    std::string currChar;
    size_t end = base.size(), i = end - 1;
    while (end > 0) {
        currChar = getStrChar(base, i);
        if (trimChars.find(currChar) == std::string::npos) {
            break;
        }
        end -= currChar.size();
        --i;
    }
    caller = Value(base.substr(0, end));
}

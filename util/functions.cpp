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
        throw ValueError("Funkcja typ() oczekuje dokładnie 1 argumentu, ale otrzymała " + std::to_string(arguments.size()));
    }
    Value val = arguments[0]->evaluate(scope);
    if (val.isBase()) {
        ValueBase base = val.asBase();
        if (std::holds_alternative<double>(base)) return Value("zmienno");
        if (std::holds_alternative<long>(base)) return Value("calk");
        if (std::holds_alternative<bool>(base)) return Value("logiczna");
        if (std::holds_alternative<std::string>(base)) return Value("lancuch");
    } else if (val.isList()) {
        return Value("lista");
    } else if (val.isDict()) {
        return Value("slownik");
    }
    return Value("nic");
}

Value roundf(const std::vector<std::unique_ptr<ASTNode>> &arguments, std::shared_ptr<Scope> &scope) {
    if (arguments.size() != 2) {
        throw ValueError("Funkcja zaokraglijzp() oczekuje dokładnie 2 argumentów, ale otrzymała " + std::to_string(arguments.size()));
    }
    Value precision = arguments[1]->evaluate(scope);
    if (!precision.isBase() && !std::holds_alternative<long>(precision.asBase())) {
        throw TypeError("Precyzja zaokraglenia musi być liczbą całkowitą");
    }
    Value val = arguments[0]->evaluate(scope);
    if (!val.isBase() || !std::holds_alternative<double>(val.asBase())) {
        throw TypeError("Zaokrąglanie może być wykonywane tylko na liczbach zmiennoprzecinkowych");
    }
    long prec = std::get<long>(precision.asBase());
    if (prec < 0) {
        throw ValueError("Precyzja zaokrąglenia nie może być ujemna");
    }
    double coef = std::pow(10, prec);
    double n = std::get<double>(val.asBase());
    return Value(std::round((n * coef)) / coef);
}

Value roundi(const std::vector<std::unique_ptr<ASTNode>> &arguments, std::shared_ptr<Scope> &scope) {
    if (arguments.size() != 1) {
        throw ValueError("Funkcja zaokraglij() oczekuje dokładnie 1 argumentu, ale otrzymała " + std::to_string(arguments.size()));
    }
    Value val = arguments[0]->evaluate(scope);
    if (!val.isBase() || !std::holds_alternative<double>(val.asBase())) {
        throw TypeError("Zaokrąglanie może być wykonywane tylko na liczbach zmiennoprzecinkowych");
    }
    return Value(static_cast<long>(std::round(std::get<double>(val.asBase()))));
}

Value floori(const std::vector<std::unique_ptr<ASTNode>> &arguments, std::shared_ptr<Scope> &scope) {
    if (arguments.size() != 1) {
        throw ValueError("Funkcja polloga() oczekuje dokładnie 1 argumentu, ale otrzymała " + std::to_string(arguments.size()));
    }
    Value val = arguments[0]->evaluate(scope);
    if (!val.isBase() || !std::holds_alternative<double>(val.asBase())) {
        throw TypeError("Zaokrąglanie może być wykonywane tylko na liczbach zmiennoprzecinkowych");
    }
    return Value(static_cast<long>(std::floor(std::get<double>(val.asBase()))));
}

Value ceili(const std::vector<std::unique_ptr<ASTNode>> &arguments, std::shared_ptr<Scope> &scope) {
    if (arguments.size() != 1) {
        throw ValueError("Funkcja sufit() oczekuje dokładnie 1 argumentu, ale otrzymała " + std::to_string(arguments.size()));
    }
    Value val = arguments[0]->evaluate(scope);
    if (!val.isBase() || !std::holds_alternative<double>(val.asBase())) {
        throw TypeError("Zaokrąglanie może być wykonywane tylko na liczbach zmiennoprzecinkowych");
    }
    return Value(static_cast<long>(std::ceil(std::get<double>(val.asBase()))));
}

// methods

Value listlen(Value& caller, const std::vector<std::unique_ptr<ASTNode>>& arguments) {
    if (!arguments.empty()) {
        throw ValueError("Metoda długosc() nie oczekuje żadnych argumentów");
    }
    return Value(static_cast<long>(caller.asList().size()));
}

void listappend(Value& caller, const std::vector<std::unique_ptr<ASTNode>> &arguments, std::shared_ptr<Scope> &scope) {
    if (arguments.size() != 1) {
        throw ValueError("Metoda dodaj() oczekuje dokładnie 1 argumentu");
    }
    Value argValue = arguments[0]->evaluate(scope);
    caller.asList().push_back(std::make_shared<Value>(argValue));
}

void listremove(Value& caller, const std::vector<std::unique_ptr<ASTNode>> &arguments, std::shared_ptr<Scope> &scope) {
    if (arguments.size() != 1) {
        throw ValueError("Metoda usun() oczekuje dokładnie 1 argumentu");
    }
    Value indexValue = arguments[0]->evaluate(scope);
    if (!indexValue.isBase() || !std::holds_alternative<long>(indexValue.asBase())) {
        throw TypeError("Argument metody usun() musi być liczbą całkowitą");
    }
    long idx = std::get<long>(indexValue.asBase());
    if (idx >= caller.asList().size() || idx < 0) {
        throw IndexError("Nie można usunąć: indeks (" + std::to_string(idx) + ") poza zasięgiem");
    }
    caller.asList().erase(caller.asList().begin() + idx);
}

void listput(Value& caller, const std::vector<std::unique_ptr<ASTNode>> &arguments, std::shared_ptr<Scope> &scope) {
    if (arguments.size() != 2) {
        throw ValueError("Metoda wstaw() oczekuje dokładnie 2 argumentów");
    }
    Value indexValue = arguments[0]->evaluate(scope);
    if (!indexValue.isBase() || !std::holds_alternative<long>(indexValue.asBase())) {
        throw TypeError("Pierwszy argument metody wstaw() musi być liczbą całkowitą");
    }
    long idx = std::get<long>(indexValue.asBase());
    Value argValue = arguments[1]->evaluate(scope);
    if (idx > caller.asList().size() || idx < 0) {
        throw IndexError("Nie można wstawić: indeks (" + std::to_string(idx) + ") poza zasięgiem");
    }
    caller.asList().insert(caller.asList().begin() + idx, std::make_shared<Value>(argValue));
}

Value dictsize(Value& caller, const std::vector<std::unique_ptr<ASTNode>> &arguments) {
    if (!arguments.empty()) {
        throw ValueError("Metoda wielkosc() nie oczekuje żadnych argumentów");
    }
    return Value(static_cast<long>(caller.asDict().size()));
}

Value dictexists(Value& caller, const std::vector<std::unique_ptr<ASTNode>> &arguments, std::shared_ptr<Scope> &scope) {
    if (arguments.size() != 1) {
        throw ValueError("Metoda istnieje() oczekuje dokładnie 1 argumentu");
    }
    Value keyValue = arguments[0]->evaluate(scope);
    if (!keyValue.isBase()) {
        throw TypeError("Klucz słownika musi być typem podstawowym");
    }
    return Value(caller.asDict().find(keyValue.asBase()) != caller.asDict().end());
}

void dictremove(Value& caller, const std::vector<std::unique_ptr<ASTNode>> &arguments, std::shared_ptr<Scope> &scope) {
    if (arguments.size() != 1) {
        throw ValueError("Metoda usun() oczekuje dokładnie 1 argumentu");
    }
    Value keyValue = arguments[0]->evaluate(scope);
    if (!keyValue.isBase()) {
        throw TypeError("Klucz słownika musi być typem podstawowym");
    } else if (caller.asDict().erase(keyValue.asBase()) == 0) {
        throw NameError("Nie znaleziono klucza słownika");
    }
}

Value slen(Value& caller, const std::vector<std::unique_ptr<ASTNode>> &arguments) {
    if (!arguments.empty()) {
        throw ValueError("Metoda długosc() nie oczekuje żadnych argumentów");
    }
    return Value(static_cast<long>(getStrLen(std::get<std::string>(caller.asBase()))));
}

void sltrim(Value& caller, const std::vector<std::unique_ptr<ASTNode>> &arguments, std::shared_ptr<Scope> &scope) {
    if (arguments.size() != 1) {
        throw ValueError("Metoda ltrym() oczekuje dokładnie 1 argumentu");
    }
    Value argValue = arguments[0]->evaluate(scope);
    if (!argValue.isBase() || !std::holds_alternative<std::string>(argValue.asBase())) {
        throw TypeError("Argument metody ltrym() musi być łańcuchem");
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
        throw ValueError("Metoda ptrym() oczekuje dokładnie 1 argumentu");
    }
    Value argValue = arguments[0]->evaluate(scope);
    if (!argValue.isBase() || !std::holds_alternative<std::string>(argValue.asBase())) {
        throw TypeError("Argument metody ptrym() musi być łańcuchem");
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
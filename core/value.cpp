#include "value.h"


Value::Value(const std::vector<Value>& vec) {
    ValueList shared_vec;
    for (const auto& v : vec) {
        shared_vec.push_back(std::make_shared<Value>(v));
    }
    data = std::move(shared_vec);
}

void Value::updateListElement(size_t index, const Value& value) {
    if (!isList()) {
        throw TypeError("Cannot update: not a list");
    }
    if (index >= asList().size()) {
        throw IndexError("Cannot update: index out of range");
    }
    *asList()[index] = value;
}

size_t Value::length() const {
    if (!isList()) {
        throw TypeError("Cannot get length: not a list");
    }
    return asList().size();
}

void Value::append(const Value& value) {
    if (!isList()) {
        throw TypeError("Cannot append: not a list");
    }
    asList().push_back(std::make_shared<Value>(value));
}

void Value::remove(size_t index) {
    if (!isList()) {
        throw TypeError("Cannot remove: not a list");
    }
    if (index >= asList().size()) {
        throw IndexError("Cannot remove: index out of range");
    }
    asList().erase(asList().begin() + index);
}

void Value::put(size_t index, const Value& value) {
    if (!isList()) {
        throw TypeError("Cannot put: not a list");
    }
    if (index > asList().size()) {
        throw IndexError("Cannot put: index out of range");
    }
    asList().insert(asList().begin() + index, std::make_shared<Value>(value));
}

void printValueBase(const ValueBase& v) {
    std::visit([](const auto& x) {
        if constexpr (std::is_same_v<std::decay_t<decltype(x)>, std::string>) {
            std::cout << '"' << x << '"';
        } else {
            std::cout << x;
        }
    }, v);
}

void printValue(const Value& value) {
    value.visit([](const auto& v) {
        using T = std::decay_t<decltype(v)>;
        if constexpr (std::is_same_v<T, ValueList>) {
            std::cout << "[";
            for (size_t i = 0; i < v.size(); ++i) {
                printValue(*v[i]);
                if (i < v.size() - 1) std::cout << ", ";
            }
            std::cout << "]";
        } else if constexpr (std::is_same_v<T, ValueBase>) {
            printValueBase(v);
        }
    });
}
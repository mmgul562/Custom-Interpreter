#include "../util/errors.h"
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

void Value::setDictElement(const ValueBase& key, const Value& value) {
    if (!isDict()) {
        throw TypeError("Cannot set/update element: not a dictionary");
    }
    asDict()[key] = std::make_shared<Value>(value);
}

size_t Value::dictSize() const {
    if (!isDict()) {
        throw TypeError("Cannot get size: not a dictionary");
    }
    return asDict().size();
}

void Value::removeKey(const ValueBase& key) {
    if (!isDict()) {
        throw TypeError("Cannot remove key: not a dictionary");
    }
    if (asDict().erase(key) == 0) {
        throw NameError("Key '" + to_string(key) + "' not found in the dictionary");
    }
}

bool Value::keyExists(const ValueBase& key) const {
    if (!isDict()) {
        throw TypeError("Cannot check key existence: not a dictionary");
    }
    return asDict().find(key) != asDict().end();
}

std::vector<ValueBase> Value::getDictKeys() const {
    if (!isDict()) {
        throw TypeError("Cannot get keys: not a dictionary");
    }
    std::vector<ValueBase> keys;
    for (const auto& pair : asDict()) {
        keys.push_back(pair.first);
    }
    return keys;
}

// printing

void printList(const ValueList& list) {
    std::cout << "[";
    for (size_t i = 0; i < list.size(); ++i) {
        printValue(*list[i]);
        if (i < list.size() - 1) std::cout << ", ";
    }
    std::cout << "]";
}

void printDict(const ValueDict& dict) {
    std::cout << "{";
    bool first = true;
    for (const auto& [key, value] : dict) {
        if (!first) std::cout << ", ";
        printValueBase(key);
        std::cout << ": ";
        printValue(*value);
        first = false;
    }
    std::cout << "}";
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
            printList(v);
        } else if constexpr (std::is_same_v<T, ValueDict>) {
            printDict(v);
        } else if constexpr (std::is_same_v<T, ValueBase>) {
            printValueBase(v);
        }
    });
}

std::string to_string(const ValueBase& value) {
    return std::visit([](const auto& v) -> std::string {
        using T = std::decay_t<decltype(v)>;
        if constexpr (std::is_same_v<T, double>) {
            return std::to_string(v);
        } else if constexpr (std::is_same_v<T, std::string>) {
            return v;
        } else if constexpr (std::is_same_v<T, bool>) {
            return v ? "true" : "false";
        }
    }, value);
}
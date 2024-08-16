#include "../util/errors.h"
#include "value.h"
#include <iostream>


Value::Value(const std::vector<Value> &vec) {
    ValueList shared_vec;
    for (const auto &v: vec) {
        shared_vec.push_back(std::make_shared<Value>(v));
    }
    data = std::move(shared_vec);
}


void Value::updateListElement(size_t index, const Value &value) {
    if (!isList()) {
        throw TypeError("Cannot update: not a list");
    }
    if (index >= asList().size()) {
        throw IndexError("Cannot update: index (" + std::to_string(index) + ") out of range");
    }
    *asList()[index] = value;
}


void Value::setDictElement(const ValueBase &key, const Value &value) {
    if (!isDict()) {
        throw TypeError("Cannot set/update element: not a dictionary");
    }
    asDict()[key] = std::make_shared<Value>(value);
}


std::vector<ValueBase> Value::getDictKeys() const {
    if (!isDict()) {
        throw TypeError("Cannot get keys: not a dictionary");
    }
    std::vector<ValueBase> keys;
    for (const auto &pair: asDict()) {
        keys.push_back(pair.first);
    }
    return keys;
}

std::string toString(const ValueBase &v) {
    if (std::holds_alternative<double>(v)) {
        return std::to_string(std::get<double>(v));
    } else if (std::holds_alternative<long>(v)) {
        return std::to_string(std::get<long>(v));
    } else if (std::holds_alternative<std::string>(v)) {
        return std::get<std::string>(v);
    } else {
        return std::get<bool>(v) ? "true" : "false";
    }
}

// printing

void printList(const ValueList &list, bool quotes) {
    std::cout << "[";
    for (size_t i = 0; i < list.size(); ++i) {
        printValue(*list[i], quotes);
        if (i < list.size() - 1) std::cout << ", ";
    }
    std::cout << "]";
}


void printDict(const ValueDict &dict, bool quotes) {
    std::cout << "{";
    bool first = true;
    for (const auto &[key, value]: dict) {
        if (!first) std::cout << ", ";
        printValueBase(key, quotes);
        std::cout << ": ";
        printValue(*value, quotes);
        first = false;
    }
    std::cout << "}";
}


void printValueBase(const ValueBase &v, bool quotes) {
    if (std::holds_alternative<std::string>(v) && quotes) {
        std::cout << '"' << std::get<std::string>(v) << '"';
    } else {
        std::cout << toString(v);
    }
}


void printValue(const Value &value, bool quotes) {
    value.visit([&quotes](const auto &v) {
        using T = std::decay_t<decltype(v)>;
        if constexpr (std::is_same_v<T, std::monostate>) {
            std::cout << "null";
        } else if constexpr (std::is_same_v<T, ValueList>) {
            printList(v, quotes);
        } else if constexpr (std::is_same_v<T, ValueDict>) {
            printDict(v, quotes);
        } else if constexpr (std::is_same_v<T, ValueBase>) {
            printValueBase(v, quotes);
        }
    });
}

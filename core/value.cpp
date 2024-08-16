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
        throw TypeError("Nie można zaktualizować: wartość nie jest listą");
    }
    if (index >= asList().size()) {
        throw IndexError("Nie można zaktualizować: indeks (" + std::to_string(index) + ") poza zasięgiem");
    }
    *asList()[index] = value;
}


void Value::setDictElement(const ValueBase &key, const Value &value) {
    if (!isDict()) {
        throw TypeError("Nie można ustawić/zaktualizować: wartość nie jest słownikiem");
    }
    asDict()[key] = std::make_shared<Value>(value);
}


std::vector<ValueBase> Value::getDictKeys() const {
    if (!isDict()) {
        throw TypeError("Nie można zdobyć kluczy: wartość nie jest słownikiem");
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
        return std::get<bool>(v) ? "prawda" : "falsz";
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
    if (value.isList()) {
        printList(value.asList(), quotes);
    } else if (value.isDict()) {
        printDict(value.asDict(), quotes);
    } else if (value.isNull()) {
        std::cout << "nic";
    } else {
        printValueBase(value.asBase(), quotes);
    }
}

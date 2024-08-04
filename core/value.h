#ifndef CPP_INTERPRETER_VALUE_H
#define CPP_INTERPRETER_VALUE_H

#include "../util/errors.h"
#include <iostream>
#include <variant>
#include <vector>
#include <memory>

class Value;
using ValueBase = std::variant<double, std::string, bool>;
using ValueList = std::vector<std::shared_ptr<Value>>;

class Value {
private:
    std::variant<ValueBase, ValueList> data;

public:
    Value() : data(ValueBase{}) {}
    explicit Value(const ValueBase& v) : data(v) {}
    explicit Value(ValueBase&& v) : data(std::move(v)) {}
    explicit Value(const ValueList& v) : data(v) {}
    explicit Value(ValueList&& v) : data(std::move(v)) {}
    explicit Value(const std::vector<Value>& vec);

    Value(const Value& other) : data(other.data) {}

    Value(Value&& other) noexcept : data(std::move(other.data)) {}

    Value& operator=(const Value& other) {
        if (this != &other) {
            data = other.data;
        }
        return *this;
    }

    Value& operator=(Value&& other) noexcept {
        if (this != &other) {
            data = std::move(other.data);
        }
        return *this;
    }

    bool isBase() const { return std::holds_alternative<ValueBase>(data); }
    bool isList() const { return std::holds_alternative<ValueList>(data); }

    const ValueBase& asBase() const { return std::get<ValueBase>(data); }
    const ValueList& asList() const { return std::get<ValueList>(data); }

    ValueBase& asBase() { return std::get<ValueBase>(data); }
    ValueList& asList() { return std::get<ValueList>(data); }

    template<typename Visitor>
    auto visit(Visitor&& visitor) const {
        return std::visit(std::forward<Visitor>(visitor), data);
    }

    void setListElement(size_t index, const Value& value);
    void updateListElement(size_t index, const Value& value);
    size_t length() const;
    void append(const Value& value);
    void remove(size_t index);
    void put(size_t index, const Value& value);
};

void printValueBase(const ValueBase& v);
void printValue(const Value& value);

#endif
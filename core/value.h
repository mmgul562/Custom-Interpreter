#ifndef CPP_INTERPRETER_VALUE_H
#define CPP_INTERPRETER_VALUE_H

#include <variant>
#include <vector>
#include <unordered_map>
#include <memory>


class Value;

using ValueBase = std::variant<long, double, std::string, bool>;
using ValueList = std::vector<std::shared_ptr<Value>>;
using ValueDict = std::unordered_map<ValueBase, std::shared_ptr<Value>>;

class Value {
private:
    std::variant<std::monostate, ValueBase, ValueList, ValueDict> data = std::monostate();

public:
    Value() : data(std::monostate()) {}

    explicit Value(const ValueBase &v) : data(v) {}

    explicit Value(ValueBase &&v) : data(std::move(v)) {}

    explicit Value(const ValueList &v) : data(v) {}

    explicit Value(ValueList &&v) : data(std::move(v)) {}

    explicit Value(const std::vector<Value> &vec);

    explicit Value(const ValueDict &v) : data(v) {}

    explicit Value(ValueDict &&v) : data(std::move(v)) {}

    Value(const Value &other) : data(other.data) {}

    Value(Value &&other) noexcept: data(std::move(other.data)) {}

    Value &operator=(const Value &other) {
        if (this != &other) {
            data = other.data;
        }
        return *this;
    }

    Value &operator=(Value &&other) noexcept {
        if (this != &other) {
            data = std::move(other.data);
        }
        return *this;
    }

    bool isNull() const { return std::holds_alternative<std::monostate>(data); }

    bool isBase() const { return std::holds_alternative<ValueBase>(data); }

    bool isList() const { return std::holds_alternative<ValueList>(data); }

    bool isDict() const { return std::holds_alternative<ValueDict>(data); }

    const ValueBase &asBase() const { return std::get<ValueBase>(data); }

    const ValueList &asList() const { return std::get<ValueList>(data); }

    const ValueDict &asDict() const { return std::get<ValueDict>(data); }

    ValueBase &asBase() { return std::get<ValueBase>(data); }

    ValueList &asList() { return std::get<ValueList>(data); }

    ValueDict &asDict() { return std::get<ValueDict>(data); }

    template<typename Visitor>
    auto visit(Visitor &&visitor) const {
        return std::visit(std::forward<Visitor>(visitor), data);
    }

    void updateListElement(size_t index, const Value &value);

    void setDictElement(const ValueBase &key, const Value &value);

    std::vector<ValueBase> getDictKeys() const;
};

void printValueBase(const ValueBase &v, bool quotes);

void printValue(const Value &value, bool quotes);

void printDict(const ValueDict &dict, bool quotes);

void printList(const ValueList &list, bool quotes);

std::string toString(const ValueBase &v);


#endif

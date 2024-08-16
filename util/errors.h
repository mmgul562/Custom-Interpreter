#ifndef CPP_INTERPRETER_ERRORS_H
#define CPP_INTERPRETER_ERRORS_H

#include "../core/value.h"
#include <stdexcept>
#include <string>


class BaseError : public std::runtime_error {
public:
    explicit BaseError(const std::string &message) : std::runtime_error(message) {}
};


class InterpreterError : public BaseError {
public:
    explicit InterpreterError(const std::string &message) : BaseError("Błąd: " + message) {}
};


class SyntaxError : public BaseError {
public:
    explicit SyntaxError(const std::string &message) : BaseError("Błąd składni: " + message) {}
};


class LexerError : public BaseError {
public:
    explicit LexerError(const std::string &message) : BaseError("Błąd leksera: " + message) {}
};


class ParserError : public BaseError {
public:
    explicit ParserError(const std::string &message) : BaseError("Błąd parsera: " + message) {}
};


class TypeError : public BaseError {
public:
    explicit TypeError(const std::string &message) : BaseError("Błąd typu: " + message) {}
};


class NameError : public BaseError {
public:
    explicit NameError(const std::string &message) : BaseError("Błąd nazwy: " + message) {}
};


class IndexError : public BaseError {
public:
    explicit IndexError(const std::string &message) : BaseError("Błąd indeksu: " + message) {}
};


class ValueError : public BaseError {
public:
    explicit ValueError(const std::string &message) : BaseError("Błąd wartości: " + message) {}
};


class ConversionError : public BaseError {
public:
    explicit ConversionError(const std::string &message) : BaseError("Błąd konwersji: " + message) {}
};


// for break and continue
class ControlFlowException : public BaseError {
public:
    explicit ControlFlowException(const std::string &type) : BaseError(type) {}
};


class ReturnException : public std::exception {
public:
    Value returnValue;

    explicit ReturnException(Value value) : returnValue(std::move(value)) {}
};


#endif
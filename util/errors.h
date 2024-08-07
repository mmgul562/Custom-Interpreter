#ifndef CPP_INTERPRETER_ERRORS_H
#define CPP_INTERPRETER_ERRORS_H

#include <stdexcept>
#include <string>


class BaseError : public std::runtime_error {
public:
    explicit BaseError(const std::string& message) : std::runtime_error(message) {}
};

class InterpreterError : public BaseError {
public:
    explicit InterpreterError(const std::string& message) : BaseError("Error: " + message) {}
};

class SyntaxError : public BaseError {
public:
    explicit SyntaxError(const std::string& message) : BaseError("Syntax error: " + message) {}
};

class LexerError : public BaseError {
public:
    explicit LexerError(const std::string& message) : BaseError("Lexer error: " + message) {}
};

class ParserError : public BaseError {
public:
    explicit ParserError(const std::string& message) : BaseError("Parser error: " + message) {}
};

class TypeError : public BaseError {
public:
    explicit TypeError(const std::string& message) : BaseError("Type error: " + message) {}
};

class NameError : public BaseError {
public:
    explicit NameError(const std::string& message) : BaseError("Name error: " + message) {}
};

class IndexError : public BaseError {
public:
    explicit IndexError(const std::string& message) : BaseError("Index error: " + message) {}
};

class  ValueError : public BaseError {
public:
    explicit ValueError(const std::string& message) : BaseError("Value error: " + message) {}
};

class ConversionError : public BaseError {
public:
    explicit ConversionError(const std::string& message) : BaseError("Conversion error: " + message) {}
};

// for break and continue
class ControlFlowException : public BaseError {
public:
    explicit ControlFlowException(const std::string& type) : BaseError(type) {}
};

#endif

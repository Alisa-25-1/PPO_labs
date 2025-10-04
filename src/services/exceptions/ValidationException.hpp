#pragma once
#include <stdexcept>
#include <string>

class ValidationException : public std::runtime_error {
public:
    explicit ValidationException(const std::string& message)
        : std::runtime_error(message) {}
};

class BusinessRuleException : public std::runtime_error {
public:
    explicit BusinessRuleException(const std::string& message)
        : std::runtime_error(message) {}
};
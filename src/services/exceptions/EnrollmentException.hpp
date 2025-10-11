#pragma once
#include <stdexcept>
#include <string>

class EnrollmentException : public std::runtime_error {
public:
    explicit EnrollmentException(const std::string& message)
        : std::runtime_error(message) {}
};

class EnrollmentNotFoundException : public EnrollmentException {
public:
    explicit EnrollmentNotFoundException(const std::string& message)
        : EnrollmentException(message) {}
};

class EnrollmentFullException : public EnrollmentException {
public:
    explicit EnrollmentFullException(const std::string& message)
        : EnrollmentException(message) {}
};
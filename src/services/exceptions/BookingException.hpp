#pragma once
#include <stdexcept>
#include <string>

class BookingException : public std::runtime_error {
public:
    explicit BookingException(const std::string& message)
        : std::runtime_error(message) {}
};

class BookingConflictException : public BookingException {
public:
    explicit BookingConflictException(const std::string& message)
        : BookingException(message) {}
};

class BookingNotFoundException : public BookingException {
public:
    explicit BookingNotFoundException(const std::string& message)
        : BookingException(message) {}
};
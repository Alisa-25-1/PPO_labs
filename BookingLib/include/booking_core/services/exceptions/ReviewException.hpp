#pragma once
#include <stdexcept>
#include <string>

class ReviewException : public std::runtime_error {
public:
    explicit ReviewException(const std::string& message)
        : std::runtime_error(message) {}
};

class ReviewNotFoundException : public ReviewException {
public:
    explicit ReviewNotFoundException(const std::string& message)
        : ReviewException(message) {}
};

class ReviewAlreadyExistsException : public ReviewException {
public:
    explicit ReviewAlreadyExistsException(const std::string& message)
        : ReviewException(message) {}
};

class InvalidReviewException : public ReviewException {
public:
    explicit InvalidReviewException(const std::string& message)
        : ReviewException(message) {}
};
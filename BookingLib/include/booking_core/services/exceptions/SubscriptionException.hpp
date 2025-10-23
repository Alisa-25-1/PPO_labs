#pragma once
#include <stdexcept>
#include <string>

class SubscriptionException : public std::runtime_error {
public:
    explicit SubscriptionException(const std::string& message)
        : std::runtime_error(message) {}
};

class SubscriptionNotFoundException : public SubscriptionException {
public:
    explicit SubscriptionNotFoundException(const std::string& message)
        : SubscriptionException(message) {}
};

class SubscriptionTypeNotFoundException : public SubscriptionException {
public:
    explicit SubscriptionTypeNotFoundException(const std::string& message)
        : SubscriptionException(message) {}
};

class SubscriptionExpiredException : public SubscriptionException {
public:
    explicit SubscriptionExpiredException(const std::string& message)
        : SubscriptionException(message) {}
};

class NoVisitsLeftException : public SubscriptionException {
public:
    explicit NoVisitsLeftException(const std::string& message)
        : SubscriptionException(message) {}
};
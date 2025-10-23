#pragma once
#include <stdexcept>
#include <string>

class AuthException : public std::runtime_error {
public:
    explicit AuthException(const std::string& message)
        : std::runtime_error(message) {}
};

class InvalidCredentialsException : public AuthException {
public:
    InvalidCredentialsException()
        : AuthException("Invalid email or password") {}
};

class EmailAlreadyExistsException : public AuthException {
public:
    EmailAlreadyExistsException()
        : AuthException("Email already registered") {}
};

class AccountInactiveException : public AuthException {
public:
    AccountInactiveException()
        : AuthException("Account is inactive") {}
};
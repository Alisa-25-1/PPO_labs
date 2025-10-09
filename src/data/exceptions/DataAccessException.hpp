#ifndef DATAACCESSEXCEPTION_HPP
#define DATAACCESSEXCEPTION_HPP

#include <stdexcept>
#include <string>

class DataAccessException : public std::runtime_error {
public:
    explicit DataAccessException(const std::string& message)
        : std::runtime_error(message) {}
};

class ConnectionException : public DataAccessException {
public:
    explicit ConnectionException(const std::string& message)
        : DataAccessException(message) {}
};

class QueryException : public DataAccessException {
public:
    explicit QueryException(const std::string& message)
        : DataAccessException(message) {}
};

class EntityNotFoundException : public DataAccessException {
public:
    explicit EntityNotFoundException(const std::string& message)
        : DataAccessException(message) {}
};

#endif // DATAACCESSEXCEPTION_HPP
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

class ConstraintViolationException : public DataAccessException {
public:
    explicit ConstraintViolationException(const std::string& message)
        : DataAccessException(message) {}
};

class ForeignKeyViolationException : public ConstraintViolationException {
public:
    explicit ForeignKeyViolationException(const std::string& message)
        : ConstraintViolationException(message) {}
};

class UniqueViolationException : public ConstraintViolationException {
public:
    explicit UniqueViolationException(const std::string& message)
        : ConstraintViolationException(message) {}
};

#endif // DATAACCESSEXCEPTION_HPP
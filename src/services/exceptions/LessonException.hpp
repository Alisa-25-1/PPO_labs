#pragma once
#include <stdexcept>
#include <string>

class LessonException : public std::runtime_error {
public:
    explicit LessonException(const std::string& message)
        : std::runtime_error(message) {}
};

class LessonNotFoundException : public LessonException {
public:
    explicit LessonNotFoundException(const std::string& message)
        : LessonException(message) {}
};

class LessonConflictException : public LessonException {
public:
    explicit LessonConflictException(const std::string& message)
        : LessonException(message) {}
};

class EnrollmentException : public LessonException {
public:
    explicit EnrollmentException(const std::string& message)
        : LessonException(message) {}
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
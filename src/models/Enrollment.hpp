#ifndef ENROLLMENT_HPP
#define ENROLLMENT_HPP

#include "../types/uuid.hpp"
#include "../types/enums.hpp"
#include <chrono>

class Enrollment {
private:
    UUID id_;
    UUID clientId_;
    UUID lessonId_;
    EnrollmentStatus status_;
    std::chrono::system_clock::time_point enrollmentDate_;

public:
    Enrollment();
    Enrollment(const UUID& id, const UUID& clientId, const UUID& lessonId);
    
    // Геттеры
    UUID getId() const;
    UUID getClientId() const;
    UUID getLessonId() const;
    EnrollmentStatus getStatus() const;
    std::chrono::system_clock::time_point getEnrollmentDate() const;
    
    // Бизнес-логика
    void cancel();
    void markAttended();
    void markMissed();
    bool canBeCancelled() const;
    bool isValid() const;
    
    // Валидация
    static bool isValidEnrollment(const UUID& clientId, const UUID& lessonId);
    
    // Операторы сравнения
    bool operator==(const Enrollment& other) const;
    bool operator!=(const Enrollment& other) const;
};

#endif // ENROLLMENT_HPP
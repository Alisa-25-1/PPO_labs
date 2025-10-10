#include "Enrollment.hpp"
#include <chrono>
#include <stdexcept>

Enrollment::Enrollment() 
    : id_(UUID()), clientId_(UUID()), lessonId_(UUID()), 
      status_(EnrollmentStatus::REGISTERED),
      enrollmentDate_(std::chrono::system_clock::now()) {}

Enrollment::Enrollment(const UUID& id, const UUID& clientId, const UUID& lessonId)
    : id_(id), clientId_(clientId), lessonId_(lessonId), 
      status_(EnrollmentStatus::REGISTERED),
      enrollmentDate_(std::chrono::system_clock::now()) {
    
    if (!isValid()) {
        throw std::invalid_argument("Invalid enrollment data");
    }
}

UUID Enrollment::getId() const { return id_; }
UUID Enrollment::getClientId() const { return clientId_; }
UUID Enrollment::getLessonId() const { return lessonId_; }
EnrollmentStatus Enrollment::getStatus() const { return status_; }
std::chrono::system_clock::time_point Enrollment::getEnrollmentDate() const { return enrollmentDate_; }

void Enrollment::cancel() {
    if (status_ != EnrollmentStatus::REGISTERED) {
        throw std::logic_error("Only registered enrollments can be cancelled");
    }
    status_ = EnrollmentStatus::CANCELLED;
}

void Enrollment::markAttended() {
    if (status_ != EnrollmentStatus::REGISTERED) {
        throw std::logic_error("Only registered enrollments can be marked as attended");
    }
    status_ = EnrollmentStatus::ATTENDED;
}

void Enrollment::markMissed() {
    if (status_ != EnrollmentStatus::REGISTERED) {
        throw std::logic_error("Only registered enrollments can be marked as missed");
    }
    status_ = EnrollmentStatus::MISSED;
}

bool Enrollment::canBeCancelled() const {
    return status_ == EnrollmentStatus::REGISTERED;
}

bool Enrollment::isValid() const {
    return !id_.isNull() && id_.isValid() &&
           !clientId_.isNull() && clientId_.isValid() &&
           !lessonId_.isNull() && lessonId_.isValid() &&
           enrollmentDate_ <= std::chrono::system_clock::now();
}

bool Enrollment::isValidEnrollment(const UUID& clientId, const UUID& lessonId) {
    return !clientId.isNull() && clientId.isValid() &&
           !lessonId.isNull() && lessonId.isValid();
}

bool Enrollment::operator==(const Enrollment& other) const {
    return id_ == other.id_ && 
           clientId_ == other.clientId_ && 
           lessonId_ == other.lessonId_ && 
           status_ == other.status_;
}

bool Enrollment::operator!=(const Enrollment& other) const {
    return !(*this == other);
}
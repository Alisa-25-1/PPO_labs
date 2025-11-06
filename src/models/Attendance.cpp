#include "Attendance.hpp"
#include <stdexcept>
#include <cmath>

Attendance::Attendance() 
    : id_(UUID()), clientId_(UUID()), entityId_(UUID()), 
      type_(AttendanceType::LESSON), status_(AttendanceStatus::SCHEDULED),
      scheduledTime_(std::chrono::system_clock::now()),
      actualTime_(std::chrono::system_clock::now()),
      amountPaid_(0.0), durationMinutes_(0) {}

Attendance::Attendance(const UUID& id, const UUID& clientId, const UUID& entityId,
                       AttendanceType type, const std::chrono::system_clock::time_point& scheduledTime)
    : id_(id), clientId_(clientId), entityId_(entityId), type_(type),
      status_(AttendanceStatus::SCHEDULED), scheduledTime_(scheduledTime),
      actualTime_(scheduledTime), amountPaid_(0.0), durationMinutes_(0) {
    
    if (!isValid()) {
        throw std::invalid_argument("Invalid attendance data");
    }
}

// Геттеры
UUID Attendance::getId() const { return id_; }
UUID Attendance::getClientId() const { return clientId_; }
UUID Attendance::getEntityId() const { return entityId_; }
AttendanceType Attendance::getType() const { return type_; }
AttendanceStatus Attendance::getStatus() const { return status_; }
std::chrono::system_clock::time_point Attendance::getScheduledTime() const { return scheduledTime_; }
std::chrono::system_clock::time_point Attendance::getActualTime() const { return actualTime_; }
std::string Attendance::getNotes() const { return notes_; }
double Attendance::getAmountPaid() const { return amountPaid_; }
int Attendance::getDurationMinutes() const { return durationMinutes_; }

// Сеттеры
void Attendance::setAmountPaid(double amount) {
    if (!isValidAmount(amount)) {
        throw std::invalid_argument("Invalid amount");
    }
    amountPaid_ = amount;
}

void Attendance::setDurationMinutes(int minutes) {
    if (minutes < 0) {
        throw std::invalid_argument("Duration cannot be negative");
    }
    durationMinutes_ = minutes;
}

void Attendance::setNotes(const std::string& notes) {
    if (!isValidNotes(notes)) {
        throw std::invalid_argument("Invalid notes");
    }
    notes_ = notes;
}

// Бизнес-логика
void Attendance::markVisited(const std::string& notes) {
    status_ = AttendanceStatus::VISITED;
    actualTime_ = std::chrono::system_clock::now();
    if (isValidNotes(notes)) {
        notes_ = notes;
    }
}

void Attendance::markCancelled(const std::string& notes) {
    status_ = AttendanceStatus::CANCELLED;
    actualTime_ = std::chrono::system_clock::now();
    if (isValidNotes(notes)) {
        notes_ = notes;
    }
}

void Attendance::markNoShow(const std::string& notes) {
    status_ = AttendanceStatus::NO_SHOW;
    actualTime_ = std::chrono::system_clock::now();
    if (isValidNotes(notes)) {
        notes_ = notes;
    }
}

bool Attendance::isVisited() const { return status_ == AttendanceStatus::VISITED; }
bool Attendance::isCancelled() const { return status_ == AttendanceStatus::CANCELLED; }
bool Attendance::isNoShow() const { return status_ == AttendanceStatus::NO_SHOW; }

bool Attendance::isPaid() const {
    return amountPaid_ > 0.0;
}

double Attendance::calculateRevenue() const {
    // Для посещенных занятий/бронирований учитываем оплату
    // Для отмененных или неявок - возвраты или штрафы могут быть учтены отдельно
    if (isVisited()) {
        return amountPaid_;
    } else if (isCancelled()) {
        // Может возврат или штраф за отмену
        return -amountPaid_ * 0.1; // Пример: 10% штраф за отмену
    }
    return 0.0;
}

bool Attendance::isValid() const {
    return !id_.isNull() && id_.isValid() &&
           !clientId_.isNull() && clientId_.isValid() &&
           !entityId_.isNull() && entityId_.isValid() &&
           scheduledTime_ <= std::chrono::system_clock::now() &&
           isValidAmount(amountPaid_) &&
           durationMinutes_ >= 0;
}

bool Attendance::isValidNotes(const std::string& notes) {
    return notes.length() <= 500;
}

bool Attendance::isValidAmount(double amount) {
    return amount >= 0.0 && !std::isnan(amount) && !std::isinf(amount);
}

bool Attendance::operator==(const Attendance& other) const {
    return id_ == other.id_ && 
           clientId_ == other.clientId_ && 
           entityId_ == other.entityId_ && 
           type_ == other.type_ && 
           status_ == other.status_;
}

bool Attendance::operator!=(const Attendance& other) const {
    return !(*this == other);
}
#include "Attendance.hpp"
#include <stdexcept>

Attendance::Attendance() 
    : id_(UUID()), clientId_(UUID()), entityId_(UUID()), 
      type_(AttendanceType::LESSON), status_(AttendanceStatus::SCHEDULED),
      scheduledTime_(std::chrono::system_clock::now()),
      actualTime_(std::chrono::system_clock::now()) {}

Attendance::Attendance(const UUID& id, const UUID& clientId, const UUID& entityId,
                       AttendanceType type, const std::chrono::system_clock::time_point& scheduledTime)
    : id_(id), clientId_(clientId), entityId_(entityId), type_(type),
      status_(AttendanceStatus::SCHEDULED), scheduledTime_(scheduledTime),
      actualTime_(scheduledTime) {
    
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

bool Attendance::isValid() const {
    return !id_.isNull() && id_.isValid() &&
           !clientId_.isNull() && clientId_.isValid() &&
           !entityId_.isNull() && entityId_.isValid() &&
           scheduledTime_ <= std::chrono::system_clock::now();
}

bool Attendance::isValidNotes(const std::string& notes) {
    return notes.length() <= 500;
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
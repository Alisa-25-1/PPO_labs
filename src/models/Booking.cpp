#include "Booking.hpp"
#include <chrono>

Booking::Booking() 
    : id_(0), clientId_(0), hallId_(0), timeSlot_(TimeSlot()), 
      status_(BookingStatus::PENDING), purpose_(""),
      createdAt_(std::chrono::system_clock::now()) {}

Booking::Booking(int id, int clientId, int hallId, const TimeSlot& timeSlot, const std::string& purpose)
    : id_(id), clientId_(clientId), hallId_(hallId), timeSlot_(timeSlot), 
      purpose_(purpose), status_(BookingStatus::PENDING),
      createdAt_(std::chrono::system_clock::now()) {}

int Booking::getId() const { return id_; }
int Booking::getClientId() const { return clientId_; }
int Booking::getHallId() const { return hallId_; }
TimeSlot Booking::getTimeSlot() const { return timeSlot_; }
BookingStatus Booking::getStatus() const { return status_; }
std::string Booking::getPurpose() const { return purpose_; }
std::chrono::system_clock::time_point Booking::getCreatedAt() const { return createdAt_; }

bool Booking::isActive() const {
    return status_ == BookingStatus::CONFIRMED || status_ == BookingStatus::PENDING;
}

bool Booking::isCancelled() const {
    return status_ == BookingStatus::CANCELLED;
}

bool Booking::isCompleted() const {
    return status_ == BookingStatus::COMPLETED;
}

void Booking::confirm() {
    status_ = BookingStatus::CONFIRMED;
}

void Booking::cancel() {
    status_ = BookingStatus::CANCELLED;
}

void Booking::complete() {
    status_ = BookingStatus::COMPLETED;
}

bool Booking::overlapsWith(const Booking& other) const {
    return hallId_ == other.hallId_ && timeSlot_.overlapsWith(other.timeSlot_);
}

bool Booking::isValid() const {
    return id_ > 0 && clientId_ > 0 && hallId_ > 0 && 
           timeSlot_.isValid() && isValidPurpose(purpose_);
}

bool Booking::isValidPurpose(const std::string& purpose) {
    return !purpose.empty() && purpose.length() <= 255;
}

bool Booking::isFutureBooking(const TimeSlot& timeSlot) {
    return timeSlot.getStartTime() > std::chrono::system_clock::now();
}

// Операторы сравнения
bool Booking::operator==(const Booking& other) const {
    return id_ == other.id_ && 
           clientId_ == other.clientId_ && 
           hallId_ == other.hallId_ && 
           timeSlot_ == other.timeSlot_ && 
           status_ == other.status_ && 
           purpose_ == other.purpose_;
}

bool Booking::operator!=(const Booking& other) const {
    return !(*this == other);
}
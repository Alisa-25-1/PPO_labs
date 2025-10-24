#include "Booking.hpp"
#include <chrono>
#include <regex>
#include <stdexcept>

Booking::Booking() 
    : id_(UUID()), clientId_(UUID()), hallId_(UUID()), timeSlot_(TimeSlot()), 
      status_(BookingStatus::PENDING), purpose_(""),
      createdAt_(std::chrono::system_clock::now()) {}

Booking::Booking(const UUID& id, const UUID& clientId, const UUID& hallId, 
                 const TimeSlot& timeSlot, const std::string& purpose)
    : id_(id), clientId_(clientId), hallId_(hallId), timeSlot_(timeSlot), 
      purpose_(purpose), status_(BookingStatus::PENDING),
      createdAt_(std::chrono::system_clock::now()) {
    
    // Валидация при создании
    if (!isValid()) {
        throw std::invalid_argument("Invalid booking data provided");
    }
}

UUID Booking::getId() const { return id_; }
UUID Booking::getClientId() const { return clientId_; }
UUID Booking::getHallId() const { return hallId_; }
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
    if (!isValidStatusTransition(status_, BookingStatus::CONFIRMED)) {
        throw std::logic_error("Invalid status transition to CONFIRMED");
    }
    status_ = BookingStatus::CONFIRMED;
}

void Booking::cancel() {
    if (!isValidStatusTransition(status_, BookingStatus::CANCELLED)) {
        throw std::logic_error("Invalid status transition to CANCELLED");
    }
    status_ = BookingStatus::CANCELLED;
}

void Booking::complete() {
    if (!isValidStatusTransition(status_, BookingStatus::COMPLETED)) {
        throw std::logic_error("Invalid status transition to COMPLETED");
    }
    status_ = BookingStatus::COMPLETED;
}

bool Booking::overlapsWith(const Booking& other) const {
    return hallId_ == other.hallId_ && timeSlot_.overlapsWith(other.timeSlot_);
}

bool Booking::isValid() const {
    return !id_.isNull() && id_.isValid() && 
           !clientId_.isNull() && clientId_.isValid() && 
           !hallId_.isNull() && hallId_.isValid() && 
           timeSlot_.isValid() && 
           isValidPurpose(purpose_) &&
           createdAt_ <= std::chrono::system_clock::now();
}

bool Booking::isValidPurpose(const std::string& purpose) {
    // Проверка длины
    if (purpose.length() < 3 || purpose.length() > 500) {
        return false;
    }
    
    // Проверка на наличие только пробелов
    bool hasNonSpace = false;
    for (char c : purpose) {
        if (!std::isspace(static_cast<unsigned char>(c))) {
            hasNonSpace = true;
            break;
        }
    }
    if (!hasNonSpace) {
        return false;
    }
    
    // Расширенный список запрещенных символов для безопасности
    const std::string forbiddenChars = "<>&\"';=()[]{}|\\`~!@#$%^*+?/:\n\r\t";
    
    for (char c : purpose) {
        // Запрещаем управляющие символы (ASCII < 32, кроме пробела, табуляции, перевода строки)
        if (static_cast<unsigned char>(c) < 32 && c != ' ' && c != '\t' && c != '\n' && c != '\r') {
            return false;
        }
        
        // Запрещаем опасные символы из нашего списка
        if (forbiddenChars.find(c) != std::string::npos) {
            return false;
        }
    }
    
    return true;
}

bool Booking::isFutureBooking(const TimeSlot& timeSlot) {
    return timeSlot.getStartTime() > std::chrono::system_clock::now();
}

bool Booking::isValidDuration(int durationMinutes) {
    return durationMinutes > 0 && durationMinutes <= 8 * 60; // Максимум 8 часов
}

bool Booking::isValidStatusTransition(BookingStatus from, BookingStatus to) {
    switch (from) {
        case BookingStatus::PENDING:
            return to == BookingStatus::CONFIRMED || to == BookingStatus::CANCELLED;
        case BookingStatus::CONFIRMED:
            return to == BookingStatus::COMPLETED || to == BookingStatus::CANCELLED;
        case BookingStatus::CANCELLED:
            return false; // Отмененное бронирование нельзя изменить
        case BookingStatus::COMPLETED:
            return false; // Завершенное бронирование нельзя изменить
        default:
            return false;
    }
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
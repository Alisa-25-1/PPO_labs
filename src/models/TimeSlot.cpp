#include "TimeSlot.hpp"
#include <sstream>
#include <iomanip>
#include <stdexcept>

TimeSlot::TimeSlot() 
    : startTime_(std::chrono::system_clock::now()), durationMinutes_(60) {}

TimeSlot::TimeSlot(std::chrono::system_clock::time_point startTime, int durationMinutes)
    : startTime_(startTime), durationMinutes_(durationMinutes) {
    
    // Валидация при создании
    if (!isValid()) {
        throw std::invalid_argument("Invalid time slot data provided");
    }
}

std::chrono::system_clock::time_point TimeSlot::getStartTime() const {
    return startTime_;
}

int TimeSlot::getDurationMinutes() const {
    return durationMinutes_;
}

std::chrono::system_clock::time_point TimeSlot::getEndTime() const {
    return startTime_ + std::chrono::minutes(durationMinutes_);
}

bool TimeSlot::overlapsWith(const TimeSlot& other) const {
    auto thisStart = getStartTime();
    auto thisEnd = getEndTime();
    auto otherStart = other.getStartTime();
    auto otherEnd = other.getEndTime();
    
    return (thisStart < otherEnd) && (thisEnd > otherStart);
}

bool TimeSlot::isValid() const {
    return isValidDuration(durationMinutes_) && 
           isReasonableTimeSlot(startTime_, durationMinutes_);
}

bool TimeSlot::isValidDuration(int durationMinutes) {
    return durationMinutes > 0 && durationMinutes <= 24 * 60; // max 24 hours
}

bool TimeSlot::isReasonableTimeSlot(const std::chrono::system_clock::time_point& startTime, int duration) {
    auto now = std::chrono::system_clock::now();
    
    // Максимальное время бронирования вперед - 1 год
    auto maxFuture = now + std::chrono::hours(24 * 365);
    if (startTime > maxFuture) {
        return false;
    }
    
    // Минимальная продолжительность - 15 минут
    if (duration < 15) {
        return false;
    }
    
    return true;
}

std::string TimeSlot::toString() const {
    auto time_t = std::chrono::system_clock::to_time_t(startTime_);
    std::tm tm = *std::localtime(&time_t);
    
    std::ostringstream oss;
    oss << std::put_time(&tm, "%Y-%m-%d %H:%M") << " for " << durationMinutes_ << " minutes";
    return oss.str();
}

// Операторы сравнения
bool TimeSlot::operator==(const TimeSlot& other) const {
    return startTime_ == other.startTime_ && durationMinutes_ == other.durationMinutes_;
}

bool TimeSlot::operator!=(const TimeSlot& other) const {
    return !(*this == other);
}

bool TimeSlot::operator<(const TimeSlot& other) const {
    if (startTime_ != other.startTime_) {
        return startTime_ < other.startTime_;
    }
    return durationMinutes_ < other.durationMinutes_;
}
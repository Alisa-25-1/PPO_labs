#include "TimeSlot.hpp"
#include <sstream>
#include <iomanip>

TimeSlot::TimeSlot() 
    : startTime_(std::chrono::system_clock::now()), durationMinutes_(60) {}

TimeSlot::TimeSlot(std::chrono::system_clock::time_point startTime, int durationMinutes)
    : startTime_(startTime), durationMinutes_(durationMinutes) {}

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
    return durationMinutes_ > 0 && durationMinutes_ <= 24 * 60; // max 24 hours
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
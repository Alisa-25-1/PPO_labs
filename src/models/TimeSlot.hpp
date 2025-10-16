#ifndef TIMESLOT_HPP
#define TIMESLOT_HPP

#include <chrono>
#include <string>
#include "../data/DateTimeUtils.hpp"

class TimeSlot {
private:
    std::chrono::system_clock::time_point startTime_;
    int durationMinutes_;

public:
    TimeSlot();
    TimeSlot(std::chrono::system_clock::time_point startTime, int durationMinutes);
    
    std::chrono::system_clock::time_point getStartTime() const;
    int getDurationMinutes() const;
    std::chrono::system_clock::time_point getEndTime() const;
    
    bool overlapsWith(const TimeSlot& other) const;
    bool isValid() const;
    std::string toString() const;
    
    // Валидация
    static bool isValidDuration(int durationMinutes);
    static bool isReasonableTimeSlot(const std::chrono::system_clock::time_point& startTime, int duration);

    // Операторы сравнения
    bool operator==(const TimeSlot& other) const;
    bool operator!=(const TimeSlot& other) const;
    bool operator<(const TimeSlot& other) const;
};

#endif // TIMESLOT_HPP
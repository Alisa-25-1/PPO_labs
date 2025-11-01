#include "TimeZoneService.hpp"
#include <iomanip>
#include <sstream>

std::chrono::system_clock::time_point TimeZoneService::toLocalTime(
    const std::chrono::system_clock::time_point& utcTime,
    const std::chrono::minutes& timezoneOffset) {
    return utcTime + timezoneOffset;
}

std::chrono::system_clock::time_point TimeZoneService::toUTCTime(
    const std::chrono::system_clock::time_point& localTime,
    const std::chrono::minutes& timezoneOffset) {
    return localTime - timezoneOffset;
}

std::string TimeZoneService::formatLocalTime(
    const std::chrono::system_clock::time_point& utcTime,
    const std::chrono::minutes& timezoneOffset,
    const std::string& format) {
    
    auto localTime = toLocalTime(utcTime, timezoneOffset);
    auto time_t = std::chrono::system_clock::to_time_t(localTime);
    std::tm* tm = std::localtime(&time_t);
    
    char buffer[80];
    std::strftime(buffer, sizeof(buffer), format.c_str(), tm);
    return std::string(buffer);
}

std::string TimeZoneService::formatLocalTimeSlot(
    const std::chrono::system_clock::time_point& utcStartTime,
    int durationMinutes,
    const std::chrono::minutes& timezoneOffset) {
    
    auto localStartTime = toLocalTime(utcStartTime, timezoneOffset);
    auto localEndTime = localStartTime + std::chrono::minutes(durationMinutes);
    
    auto start_time_t = std::chrono::system_clock::to_time_t(localStartTime);
    auto end_time_t = std::chrono::system_clock::to_time_t(localEndTime);
    
    std::tm* start_tm = std::localtime(&start_time_t);
    std::tm* end_tm = std::localtime(&end_time_t);
    
    char startBuffer[80], endBuffer[80];
    std::strftime(startBuffer, sizeof(startBuffer), "%H:%M", start_tm);
    std::strftime(endBuffer, sizeof(endBuffer), "%H:%M", end_tm);
    
    return std::string(startBuffer) + " - " + std::string(endBuffer) + 
           " (" + std::to_string(durationMinutes) + " мин)";
}

bool TimeZoneService::isWithinLocalWorkingHours(
    const TimeSlot& timeSlot,
    const WorkingHours& workingHours,
    const std::chrono::minutes& timezoneOffset) {
    
    auto localStartTime = toLocalTime(timeSlot.getStartTime(), timezoneOffset);
    auto localEndTime = toLocalTime(timeSlot.getEndTime(), timezoneOffset);
    
    auto start_time_t = std::chrono::system_clock::to_time_t(localStartTime);
    auto end_time_t = std::chrono::system_clock::to_time_t(localEndTime);
    
    std::tm* start_tm = std::localtime(&start_time_t);
    std::tm* end_tm = std::localtime(&end_time_t);
    
    int startHour = start_tm->tm_hour;
    int endHour = end_tm->tm_hour;
    int openHour = workingHours.openTime.count();
    int closeHour = workingHours.closeTime.count();
    
    return startHour >= openHour && endHour <= closeHour;
}
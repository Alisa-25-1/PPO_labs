#include "DateTimeUtils.hpp"
#include <iomanip>
#include <sstream>
#include <ctime>

std::string DateTimeUtils::formatTimeForPostgres(const std::chrono::system_clock::time_point& time_point) {
    auto time_t = std::chrono::system_clock::to_time_t(time_point);
    std::tm tm = *std::gmtime(&time_t);
    
    std::ostringstream oss;
    oss << std::put_time(&tm, "%Y-%m-%d %H:%M:%S");
    return oss.str();
}

std::chrono::system_clock::time_point DateTimeUtils::parseTimeFromPostgres(const std::string& timeStr) {
    std::tm tm = {};
    std::istringstream ss(timeStr);
    ss >> std::get_time(&tm, "%Y-%m-%d %H:%M:%S");
    
    if (ss.fail()) {
        throw std::runtime_error("Failed to parse time from PostgreSQL");
    }

    auto utc_time_t = timegm(&tm); 
    
    if (utc_time_t == -1) {
        tm.tm_isdst = 0;
        utc_time_t = std::mktime(&tm);
        std::tm local_tm = *std::localtime(&utc_time_t);
        std::tm utc_tm = *std::gmtime(&utc_time_t);
        utc_time_t += (std::mktime(&local_tm) - std::mktime(&utc_tm));
    }
    
    return std::chrono::system_clock::from_time_t(utc_time_t);
}

std::chrono::system_clock::time_point DateTimeUtils::convertUTCToLocal(const std::chrono::system_clock::time_point& utcTime) {
    auto utc_time_t = std::chrono::system_clock::to_time_t(utcTime);
    
    std::tm local_tm = *std::localtime(&utc_time_t);
    auto local_time_t = std::mktime(&local_tm);
    
    return std::chrono::system_clock::from_time_t(local_time_t);
}

std::chrono::system_clock::time_point DateTimeUtils::convertLocalToUTC(const std::chrono::system_clock::time_point& localTime) {
    auto local_time_t = std::chrono::system_clock::to_time_t(localTime);
    
    std::tm utc_tm = *std::gmtime(&local_time_t);
    auto utc_time_t = std::mktime(&utc_tm);
    
    return std::chrono::system_clock::from_time_t(utc_time_t);
}

// Новые методы

std::string DateTimeUtils::formatTime(const std::chrono::system_clock::time_point& timePoint) {
    auto time_t = std::chrono::system_clock::to_time_t(timePoint);
    std::tm* tm = std::localtime(&time_t);
    
    char buffer[80];
    std::strftime(buffer, sizeof(buffer), "%H:%M", tm);
    return std::string(buffer);
}

std::string DateTimeUtils::formatDateTime(const std::chrono::system_clock::time_point& timePoint) {
    auto time_t = std::chrono::system_clock::to_time_t(timePoint);
    std::tm* tm = std::localtime(&time_t);
    
    char buffer[80];
    std::strftime(buffer, sizeof(buffer), "%d.%m.%Y %H:%M", tm);
    return std::string(buffer);
}

std::string DateTimeUtils::formatTimeSlot(const std::chrono::system_clock::time_point& startTime, int durationMinutes) {
    auto endTime = startTime + std::chrono::minutes(durationMinutes);
    
    auto start_time_t = std::chrono::system_clock::to_time_t(startTime);
    auto end_time_t = std::chrono::system_clock::to_time_t(endTime);
    
    std::tm* start_tm = std::localtime(&start_time_t);
    std::tm* end_tm = std::localtime(&end_time_t);
    
    char startBuffer[80], endBuffer[80];
    std::strftime(startBuffer, sizeof(startBuffer), "%H:%M", start_tm);
    std::strftime(endBuffer, sizeof(endBuffer), "%H:%M", end_tm);
    
    return std::string(startBuffer) + " - " + std::string(endBuffer) + " (" + std::to_string(durationMinutes) + " мин)";
}

bool DateTimeUtils::isSameDay(const std::chrono::system_clock::time_point& time1, 
                             const std::chrono::system_clock::time_point& time2) {
    auto time1_t = std::chrono::system_clock::to_time_t(time1);
    auto time2_t = std::chrono::system_clock::to_time_t(time2);
    
    std::tm tm1 = *std::localtime(&time1_t);
    std::tm tm2 = *std::localtime(&time2_t);
    
    return tm1.tm_year == tm2.tm_year &&
           tm1.tm_mon == tm2.tm_mon &&
           tm1.tm_mday == tm2.tm_mday;
}

std::chrono::system_clock::time_point DateTimeUtils::createDateTime(int year, int month, int day, 
                                                                   int hour, int minute, int second) {
    std::tm tm = {0};
    tm.tm_year = year - 1900;
    tm.tm_mon = month - 1;
    tm.tm_mday = day;
    tm.tm_hour = hour;
    tm.tm_min = minute;
    tm.tm_sec = second;
    tm.tm_isdst = -1; // Определять автоматически
    
    std::time_t time = std::mktime(&tm);
    return std::chrono::system_clock::from_time_t(time);
}

bool DateTimeUtils::isTimeInRange(const std::chrono::system_clock::time_point& time,
                                 const std::chrono::hours& startHour, 
                                 const std::chrono::hours& endHour) {
    auto time_t = std::chrono::system_clock::to_time_t(time);
    std::tm* tm = std::localtime(&time_t);
    
    int currentHour = tm->tm_hour;
    int startHourValue = startHour.count();
    int endHourValue = endHour.count();
    
    return currentHour >= startHourValue && currentHour < endHourValue;
}
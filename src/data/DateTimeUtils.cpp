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
#include "DateTimeUtils.hpp"

std::string DateTimeUtils::formatTimeForPostgres(const std::chrono::system_clock::time_point& time_point) {
    auto time_t = std::chrono::system_clock::to_time_t(time_point);
    std::tm tm = *std::gmtime(&time_t);
    std::ostringstream oss;
    oss << std::put_time(&tm, "%Y-%m-%d %H:%M:%S");
    return oss.str();
}

std::chrono::system_clock::time_point DateTimeUtils::parseTimeFromPostgres(const std::string& timeStr) {
    std::tm tm = {};
    std::istringstream iss(timeStr);
    iss >> std::get_time(&tm, "%Y-%m-%d %H:%M:%S");
    return std::chrono::system_clock::from_time_t(std::mktime(&tm));
}
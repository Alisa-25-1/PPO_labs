#ifndef DATETIMEUTILS_HPP
#define DATETIMEUTILS_HPP

#include <string>
#include <chrono>

class DateTimeUtils {
public:
    static std::string formatTimeForPostgres(const std::chrono::system_clock::time_point& time_point);
    static std::chrono::system_clock::time_point parseTimeFromPostgres(const std::string& timeStr);
    
    // Новые методы для автоматического преобразования
    static std::chrono::system_clock::time_point convertUTCToLocal(const std::chrono::system_clock::time_point& utcTime);
    static std::chrono::system_clock::time_point convertLocalToUTC(const std::chrono::system_clock::time_point& localTime);
};

#endif // DATETIMEUTILS_HPP
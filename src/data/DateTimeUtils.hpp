#ifndef DATETIMEUTILS_HPP
#define DATETIMEUTILS_HPP

#include <chrono>
#include <string>
#include <iomanip>
#include <sstream>

class DateTimeUtils {
public:
    static std::string formatTimeForPostgres(const std::chrono::system_clock::time_point& time_point);
    static std::chrono::system_clock::time_point parseTimeFromPostgres(const std::string& timeStr);
};

#endif // DATETIMEUTILS_HPP
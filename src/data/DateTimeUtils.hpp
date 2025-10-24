#ifndef DATETIMEUTILS_HPP
#define DATETIMEUTILS_HPP

#include <string>
#include <chrono>

class DateTimeUtils {
public:
    // Существующие методы
    static std::string formatTimeForPostgres(const std::chrono::system_clock::time_point& time_point);
    static std::chrono::system_clock::time_point parseTimeFromPostgres(const std::string& timeStr);
    static std::chrono::system_clock::time_point convertUTCToLocal(const std::chrono::system_clock::time_point& utcTime);
    static std::chrono::system_clock::time_point convertLocalToUTC(const std::chrono::system_clock::time_point& localTime);
    
    // Новые методы для форматирования времени
    static std::string formatTime(const std::chrono::system_clock::time_point& timePoint);
    static std::string formatDateTime(const std::chrono::system_clock::time_point& timePoint);
    static std::string formatTimeSlot(const std::chrono::system_clock::time_point& startTime, int durationMinutes);
    
    // Методы для работы с датами
    static bool isSameDay(const std::chrono::system_clock::time_point& time1, 
                         const std::chrono::system_clock::time_point& time2);
    static std::chrono::system_clock::time_point createDateTime(int year, int month, int day, 
                                                               int hour, int minute, int second = 0);
    
    // Методы для проверки временных интервалов
    static bool isTimeInRange(const std::chrono::system_clock::time_point& time,
                             const std::chrono::hours& startHour, 
                             const std::chrono::hours& endHour);
};

#endif // DATETIMEUTILS_HPP
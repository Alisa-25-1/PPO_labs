#ifndef DATETIMEUTILS_HPP
#define DATETIMEUTILS_HPP

#include <string>
#include <chrono>
#include <ctime>

class DateTimeUtils {
public:
    // Основные методы для работы с PostgreSQL
    static std::string formatTimeForPostgres(const std::chrono::system_clock::time_point& time_point);
    static std::chrono::system_clock::time_point parseTimeFromPostgres(const std::string& timeStr);
    
    // Методы для форматирования времени (работают с локальным временем системы)
    static std::string formatTime(const std::chrono::system_clock::time_point& timePoint);
    static std::string formatDateTime(const std::chrono::system_clock::time_point& timePoint);
    static std::string formatDate(const std::chrono::system_clock::time_point& timePoint);
    static std::string formatTimeSlot(const std::chrono::system_clock::time_point& startTime, int durationMinutes);
    
    // Методы для работы с датами
    static bool isSameDay(const std::chrono::system_clock::time_point& time1, 
                         const std::chrono::system_clock::time_point& time2);
    static std::chrono::system_clock::time_point createDateTime(int year, int month, int day, 
                                                               int hour, int minute, int second = 0);
    
    // Методы для проверки временных интервалов (работают с локальным временем)
    static bool isTimeInRange(const std::chrono::system_clock::time_point& time,
                             const std::chrono::hours& startHour, 
                             const std::chrono::hours& endHour);
    
    // Новый метод для форматирования времени с учетом смещения часового пояса
    static std::string formatDateTimeWithOffset(const std::chrono::system_clock::time_point& timePoint, 
                                               const std::chrono::minutes& offset);
    
    // Метод для форматирования времени с учетом смещения
    static std::string formatTimeWithOffset(const std::chrono::system_clock::time_point& timePoint, 
                                           const std::chrono::minutes& offset);

    static std::string formatTimeSlotWithOffset(const std::chrono::system_clock::time_point& startTime, 
                                           int durationMinutes,
                                           const std::chrono::minutes& offset);

    // Вспомогательные функции для работы с временем
    static std::time_t timegm(std::tm* tm);
};

#endif // DATETIMEUTILS_HPP
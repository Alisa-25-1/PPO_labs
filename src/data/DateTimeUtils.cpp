#include "DateTimeUtils.hpp"
#include <iomanip>
#include <sstream>
#include <ctime>

// Реализация timegm для систем, где она отсутствует
#ifndef _WIN32
std::time_t DateTimeUtils::timegm(std::tm* tm) {
    // На Unix-системах используем стандартную timegm
    return ::timegm(tm);
}
#else
// На Windows реализуем timegm самостоятельно
std::time_t DateTimeUtils::timegm(std::tm* tm) {
    std::time_t t = std::mktime(tm);
    if (t == -1) {
        return -1;
    }
    
    // Получаем локальное время для этой временной точки
    std::tm* local_tm = std::localtime(&t);
    if (!local_tm) {
        return -1;
    }
    
    // Вычисляем разницу между локальным временем и UTC
    int offset = local_tm->tm_hour - tm->tm_hour;
    if (offset > 12) {
        offset -= 24;
    } else if (offset < -12) {
        offset += 24;
    }
    
    // Корректируем время на смещение
    return t - offset * 3600;
}
#endif

std::string DateTimeUtils::formatTimeForPostgres(const std::chrono::system_clock::time_point& time_point) {
    auto time_t = std::chrono::system_clock::to_time_t(time_point);
    std::tm tm = *std::gmtime(&time_t);  // Используем GMT для хранения в БД
    
    std::ostringstream oss;
    oss << std::put_time(&tm, "%Y-%m-%d %H:%M:%S");
    return oss.str();
}

std::chrono::system_clock::time_point DateTimeUtils::parseTimeFromPostgres(const std::string& timeStr) {
    std::tm tm = {};
    std::istringstream ss(timeStr);
    ss >> std::get_time(&tm, "%Y-%m-%d %H:%M:%S");
    
    if (ss.fail()) {
        throw std::runtime_error("Failed to parse time from PostgreSQL: " + timeStr);
    }

    // Явно указываем, что время в UTC
    tm.tm_isdst = -1; // Не учитываем летнее время
    auto utc_time_t = timegm(&tm); // Используем timegm для UTC
    
    if (utc_time_t == -1) {
        throw std::runtime_error("Invalid time value from PostgreSQL: " + timeStr);
    }
    
    return std::chrono::system_clock::from_time_t(utc_time_t);
}

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

std::string DateTimeUtils::formatDate(const std::chrono::system_clock::time_point& timePoint) {
    auto time_t = std::chrono::system_clock::to_time_t(timePoint);
    std::tm* tm = std::localtime(&time_t);
    
    char buffer[80];
    std::strftime(buffer, sizeof(buffer), "%d.%m.%Y", tm);
    return std::string(buffer);
}

std::string DateTimeUtils::formatTimeSlot(const std::chrono::system_clock::time_point& startTime, int durationMinutes) {
    auto endTime = startTime + std::chrono::minutes(durationMinutes);
    
    // Используем gmtime для консистентности
    auto start_time_t = std::chrono::system_clock::to_time_t(startTime);
    auto end_time_t = std::chrono::system_clock::to_time_t(endTime);
    
    std::tm start_tm = *std::gmtime(&start_time_t);  // Используем gmtime
    std::tm end_tm = *std::gmtime(&end_time_t);      // Используем gmtime
    
    char startBuffer[80], endBuffer[80];
    std::strftime(startBuffer, sizeof(startBuffer), "%H:%M", &start_tm);
    std::strftime(endBuffer, sizeof(endBuffer), "%H:%M", &end_tm);
    
    return std::string(startBuffer) + " - " + std::string(endBuffer) + 
           " (" + std::to_string(durationMinutes) + " мин)";
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

std::string DateTimeUtils::formatDateTimeWithOffset(const std::chrono::system_clock::time_point& timePoint, 
                                                   const std::chrono::minutes& offset) {
    // Время уже в UTC, просто добавляем смещение для отображения
    auto localTime = timePoint + offset;
    auto time_t = std::chrono::system_clock::to_time_t(localTime);
    std::tm tm = *std::gmtime(&time_t); // Используем gmtime для консистентности
    
    char buffer[80];
    std::strftime(buffer, sizeof(buffer), "%d.%m.%Y %H:%M", &tm);
    return std::string(buffer);
}

std::string DateTimeUtils::formatTimeWithOffset(const std::chrono::system_clock::time_point& timePoint, 
                                               const std::chrono::minutes& offset) {
    auto localTime = timePoint + offset;
    auto time_t = std::chrono::system_clock::to_time_t(localTime);
    std::tm tm = *std::gmtime(&time_t);
    
    char buffer[80];
    std::strftime(buffer, sizeof(buffer), "%H:%M", &tm);
    return std::string(buffer);
}

// Добавим новый метод для форматирования временного слота с учетом смещения
std::string DateTimeUtils::formatTimeSlotWithOffset(const std::chrono::system_clock::time_point& startTime, 
                                                   int durationMinutes,
                                                   const std::chrono::minutes& offset) {
    auto localStartTime = startTime + offset;
    auto localEndTime = localStartTime + std::chrono::minutes(durationMinutes);
    
    auto start_time_t = std::chrono::system_clock::to_time_t(localStartTime);
    auto end_time_t = std::chrono::system_clock::to_time_t(localEndTime);
    
    std::tm start_tm = *std::gmtime(&start_time_t);
    std::tm end_tm = *std::gmtime(&end_time_t);
    
    char startBuffer[80], endBuffer[80];
    std::strftime(startBuffer, sizeof(startBuffer), "%H:%M", &start_tm);
    std::strftime(endBuffer, sizeof(endBuffer), "%H:%M", &end_tm);
    
    return std::string(startBuffer) + " - " + std::string(endBuffer) + 
           " (" + std::to_string(durationMinutes) + " мин)";
}
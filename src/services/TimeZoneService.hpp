#pragma once

#include "../models/Branch.hpp"
#include "../models/TimeSlot.hpp"
#include "../data/DateTimeUtils.hpp"
#include <chrono>
#include <string>

class TimeZoneService {
public:
    // Преобразование UTC времени в локальное время филиала
    static std::chrono::system_clock::time_point toLocalTime(
        const std::chrono::system_clock::time_point& utcTime,
        const std::chrono::minutes& timezoneOffset);
    
    // Преобразование локального времени филиала в UTC
    static std::chrono::system_clock::time_point toUTCTime(
        const std::chrono::system_clock::time_point& localTime,
        const std::chrono::minutes& timezoneOffset);
    
    // Форматирование времени с учетом часового пояса
    static std::string formatLocalTime(
        const std::chrono::system_clock::time_point& utcTime,
        const std::chrono::minutes& timezoneOffset,
        const std::string& format = "%H:%M");
    
    // Форматирование временного слота с учетом часового пояса
    static std::string formatLocalTimeSlot(
        const std::chrono::system_clock::time_point& utcStartTime,
        int durationMinutes,
        const std::chrono::minutes& timezoneOffset);
    
    // Проверка, находится ли временной слот в рабочем времени филиала (в локальном времени)
    static bool isWithinLocalWorkingHours(
        const TimeSlot& timeSlot,
        const WorkingHours& workingHours,
        const std::chrono::minutes& timezoneOffset);
};
#ifndef SCHEDULE_HPP
#define SCHEDULE_HPP

#include "../types/uuid.hpp"
#include "TimeSlot.hpp"
#include <vector>
#include <chrono>

// Виртуальная сущность для представления расписания
struct ScheduleSlot {
    TimeSlot timeSlot;
    UUID entityId; // ID занятия или бронирования
    std::string entityType; // "lesson" или "booking"
    std::string description;
    
    ScheduleSlot(const TimeSlot& slot, const UUID& id, const std::string& type, const std::string& desc)
        : timeSlot(slot), entityId(id), entityType(type), description(desc) {}
};

class Schedule {
private:
    UUID branchId_;
    std::chrono::system_clock::time_point startDate_;
    std::chrono::system_clock::time_point endDate_;
    std::vector<ScheduleSlot> slots_;

public:
    Schedule(const UUID& branchId, 
             const std::chrono::system_clock::time_point& startDate,
             const std::chrono::system_clock::time_point& endDate);
    
    // Геттеры
    UUID getBranchId() const;
    std::chrono::system_clock::time_point getStartDate() const;
    std::chrono::system_clock::time_point getEndDate() const;
    const std::vector<ScheduleSlot>& getSlots() const;
    
    // Бизнес-логика
    void addSlot(const ScheduleSlot& slot);
    std::vector<ScheduleSlot> getSlotsForHall(const UUID& hallId) const;
    std::vector<ScheduleSlot> getSlotsForDate(const std::chrono::system_clock::time_point& date) const;
    bool hasConflicts(const TimeSlot& timeSlot, const UUID& hallId) const;
    bool isValid() const;
};

#endif // SCHEDULE_HPP
#include "Schedule.hpp"
#include <algorithm>

Schedule::Schedule(const UUID& branchId, 
                 const std::chrono::system_clock::time_point& startDate,
                 const std::chrono::system_clock::time_point& endDate)
    : branchId_(branchId), startDate_(startDate), endDate_(endDate) {
    
    if (startDate >= endDate) {
        throw std::invalid_argument("Start date must be before end date");
    }
}

UUID Schedule::getBranchId() const { return branchId_; }
std::chrono::system_clock::time_point Schedule::getStartDate() const { return startDate_; }
std::chrono::system_clock::time_point Schedule::getEndDate() const { return endDate_; }
const std::vector<ScheduleSlot>& Schedule::getSlots() const { return slots_; }

void Schedule::addSlot(const ScheduleSlot& slot) {
    // Проверяем, что слот находится в пределах периода расписания
    if (slot.timeSlot.getStartTime() < startDate_ || 
        slot.timeSlot.getEndTime() > endDate_) {
        throw std::invalid_argument("Slot is outside schedule period");
    }
    slots_.push_back(slot);
}

std::vector<ScheduleSlot> Schedule::getSlotsForHall(const UUID& hallId) const {
    std::vector<ScheduleSlot> result;
    std::copy_if(slots_.begin(), slots_.end(), std::back_inserter(result),
        [&hallId](const ScheduleSlot& slot) {
            // В реальной реализации здесь была бы логика фильтрации по залу
            return true; // заглушка
        });
    return result;
}

std::vector<ScheduleSlot> Schedule::getSlotsForDate(
    const std::chrono::system_clock::time_point& date) const {
    
    std::vector<ScheduleSlot> result;
    std::copy_if(slots_.begin(), slots_.end(), std::back_inserter(result),
        [&date](const ScheduleSlot& slot) {
            auto slotTime = std::chrono::system_clock::to_time_t(slot.timeSlot.getStartTime());
            auto targetTime = std::chrono::system_clock::to_time_t(date);
            
            std::tm slotTm = *std::localtime(&slotTime);
            std::tm targetTm = *std::localtime(&targetTime);
            
            return slotTm.tm_year == targetTm.tm_year &&
                   slotTm.tm_mon == targetTm.tm_mon &&
                   slotTm.tm_mday == targetTm.tm_mday;
        });
    return result;
}

bool Schedule::hasConflicts(const TimeSlot& timeSlot, const UUID& hallId) const {
    return std::any_of(slots_.begin(), slots_.end(),
        [&timeSlot, &hallId](const ScheduleSlot& slot) {
            // В реальной реализации проверяли бы hallId
            return slot.timeSlot.overlapsWith(timeSlot);
        });
}

bool Schedule::isValid() const {
    return !branchId_.isNull() && branchId_.isValid() &&
           startDate_ < endDate_ &&
           std::all_of(slots_.begin(), slots_.end(),
               [](const ScheduleSlot& slot) {
                   return slot.timeSlot.isValid() && 
                          !slot.entityId.isNull() &&
                          (slot.entityType == "lesson" || slot.entityType == "booking");
               });
}
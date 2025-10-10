#ifndef SCHEDULEDTO_HPP
#define SCHEDULEDTO_HPP

#include "../types/uuid.hpp"
#include "../models/TimeSlot.hpp"
#include <string>
#include <vector>
#include <chrono>
#include <iomanip>

struct ScheduleSlotDTO {
    TimeSlot timeSlot;
    UUID entityId;
    std::string entityType; // "lesson" или "booking"
    std::string title;
    std::string description;
    std::string status;
    
    ScheduleSlotDTO(const TimeSlot& slot, const UUID& id, const std::string& type, 
                   const std::string& title, const std::string& desc, const std::string& status)
        : timeSlot(slot), entityId(id), entityType(type), title(title), 
          description(desc), status(status) {}
};

struct ScheduleRequestDTO {
    UUID branchId;
    std::chrono::system_clock::time_point startDate;
    std::chrono::system_clock::time_point endDate;
    
    bool validate() const {
        return !branchId.isNull() && branchId.isValid() &&
               startDate < endDate;
    }
};

struct ScheduleResponseDTO {
    UUID branchId;
    std::string startDate;
    std::string endDate;
    std::vector<ScheduleSlotDTO> slots;
    
    ScheduleResponseDTO(const UUID& branchId, 
                       const std::chrono::system_clock::time_point& startDate,
                       const std::chrono::system_clock::time_point& endDate)
        : branchId(branchId) {
        
        auto time_t_start = std::chrono::system_clock::to_time_t(startDate);
        std::tm tm_start = *std::localtime(&time_t_start);
        std::ostringstream oss_start;
        oss_start << std::put_time(&tm_start, "%Y-%m-%d");
        this->startDate = oss_start.str();
        
        auto time_t_end = std::chrono::system_clock::to_time_t(endDate);
        std::tm tm_end = *std::localtime(&time_t_end);
        std::ostringstream oss_end;
        oss_end << std::put_time(&tm_end, "%Y-%m-%d");
        this->endDate = oss_end.str();
    }
    
    void addSlot(const ScheduleSlotDTO& slot) {
        slots.push_back(slot);
    }
};

#endif // SCHEDULEDTO_HPP
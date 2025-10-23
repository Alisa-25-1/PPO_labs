#pragma once
#include "../repositories/ILessonRepository.hpp"
#include "../repositories/IBookingRepository.hpp"
#include "../repositories/IDanceHallRepository.hpp"
#include "../dtos/ScheduleDTO.hpp"
#include "../types/uuid.hpp"
#include "exceptions/ValidationException.hpp"
#include <memory>
#include <vector>

class ScheduleService {
private:
    std::unique_ptr<ILessonRepository> lessonRepository_;
    std::unique_ptr<IBookingRepository> bookingRepository_;
    std::unique_ptr<IDanceHallRepository> hallRepository_;

    void validateBranch(const UUID& branchId) const;
    void validateDateRange(const std::chrono::system_clock::time_point& startDate, 
                          const std::chrono::system_clock::time_point& endDate) const;

public:
    ScheduleService(
        std::unique_ptr<ILessonRepository> lessonRepo,
        std::unique_ptr<IBookingRepository> bookingRepo,
        std::unique_ptr<IDanceHallRepository> hallRepo
    );

    ScheduleResponseDTO getBranchSchedule(const UUID& branchId, 
                                         const std::chrono::system_clock::time_point& startDate,
                                         const std::chrono::system_clock::time_point& endDate);
    
    ScheduleResponseDTO getHallSchedule(const UUID& hallId,
                                       const std::chrono::system_clock::time_point& startDate,
                                       const std::chrono::system_clock::time_point& endDate);
    
    std::vector<TimeSlot> getAvailableTimeSlots(const UUID& hallId,
                                               const std::chrono::system_clock::time_point& date);
    
    bool isTimeSlotAvailable(const UUID& hallId, const TimeSlot& timeSlot) const;
};
#pragma once
#include "../repositories/IBookingRepository.hpp"
#include "../repositories/IClientRepository.hpp"
#include "../repositories/IDanceHallRepository.hpp"
#include "../repositories/IBranchRepository.hpp"
#include "../repositories/IAttendanceRepository.hpp"
#include "../repositories/ILessonRepository.hpp"
#include "AttendanceService.hpp"
#include "IBranchService.hpp" 
#include "../dtos/BookingDTO.hpp"
#include "../types/uuid.hpp"
#include "exceptions/BookingException.hpp"
#include "exceptions/ValidationException.hpp"
#include "../data/DateTimeUtils.hpp"
#include "TimeZoneService.hpp"
#include <memory>
#include <vector>
#include <iostream>

class BookingService {
private:
    std::shared_ptr<IBookingRepository> bookingRepository_;
    std::shared_ptr<IClientRepository> clientRepository_;
    std::shared_ptr<IDanceHallRepository> hallRepository_;
    std::shared_ptr<IBranchRepository> branchRepository_;
    std::shared_ptr<IBranchService> branchService_; 
    std::shared_ptr<ILessonRepository> lessonRepository_;
    std::shared_ptr<AttendanceService> attendanceService_;

    // Validation methods
    void validateBookingRequest(const BookingRequestDTO& request) const;
    void validateClient(const UUID& clientId) const;
    void validateDanceHall(const UUID& hallId) const; 
    void validateTimeSlot(const TimeSlot& timeSlot) const;
    void validateWorkingHours(const UUID& hallId, const TimeSlot& timeSlot) const;
    void checkBookingConflicts(const UUID& hallId, const TimeSlot& timeSlot, 
                              const UUID& excludeBookingId = UUID()) const;
    void checkLessonConflicts(const UUID& hallId, const TimeSlot& timeSlot) const;

    // Вспомогательные методы
    bool isWithinWorkingHours(const TimeSlot& timeSlot, 
                             const std::chrono::hours& openTime, 
                             const std::chrono::hours& closeTime) const;
    std::vector<Booking> filterBookingsByDate(const std::vector<Booking>& bookings, 
                                             const std::chrono::system_clock::time_point& date) const;
    std::vector<TimeSlot> generateAvailableSlots(const std::chrono::system_clock::time_point& date,
                                                const std::chrono::hours& openTime,
                                                const std::chrono::hours& closeTime,
                                                const std::vector<Booking>& existingBookings,
                                                const UUID& hallId) const;
    std::vector<TimeSlot> generateAvailableSlotsWithDuration(
        const std::chrono::system_clock::time_point& date,
        const std::chrono::hours& openTime,
        const std::chrono::hours& closeTime,
        const std::vector<Booking>& existingBookings,
        const UUID& hallId) const;
    std::vector<TimeSlot> generateAvailableSlotsWithTimezone(
        const std::chrono::system_clock::time_point& date,
        const WorkingHours& workingHours,
        const std::vector<Booking>& existingBookings,
        const UUID& hallId,
        const std::chrono::minutes& timezoneOffset) const;

public:
    // Constructor with dependency injection
    BookingService(
        std::shared_ptr<IBookingRepository> bookingRepo,
        std::shared_ptr<IClientRepository> clientRepo,
        std::shared_ptr<IDanceHallRepository> hallRepo,
        std::shared_ptr<IBranchRepository> branchRepo,
        std::shared_ptr<IBranchService> branchService,
        std::shared_ptr<ILessonRepository> lessonRepo,
        std::shared_ptr<AttendanceService> attendanceService
    );

    // Main business logic methods
    BookingResponseDTO createBooking(const BookingRequestDTO& request);
    BookingResponseDTO cancelBooking(const UUID& bookingId, const UUID& clientId);
    BookingResponseDTO getBooking(const UUID& bookingId);
    std::vector<BookingResponseDTO> getClientBookings(const UUID& clientId);
    std::vector<BookingResponseDTO> getDanceHallBookings(const UUID& hallId); 
    bool isTimeSlotAvailable(const UUID& hallId, const TimeSlot& timeSlot) const;
    std::vector<DanceHall> getAllHalls() const;
    std::optional<DanceHall> getHallById(const UUID& hallId) const;
    std::vector<TimeSlot> getAvailableTimeSlots(const UUID& hallId, 
                                               const std::chrono::system_clock::time_point& date) const;
    std::vector<int> getAvailableDurations(const UUID& hallId, 
                                          const std::chrono::system_clock::time_point& startTime) const;

    std::vector<Branch> getAllBranches() const;
    std::vector<DanceHall> getHallsByBranch(const UUID& branchId) const;
    
    // Business rules
    bool canClientBook(const UUID& clientId) const;
    int getClientActiveBookingsCount(const UUID& clientId) const;  
    BookingResponseDTO completeBooking(const UUID& bookingId); 

    // Вспомогательные методы
    std::optional<Branch> getBranchForHall(const UUID& hallId) const;
    std::chrono::minutes getTimezoneOffsetForHall(const UUID& hallId) const;
};
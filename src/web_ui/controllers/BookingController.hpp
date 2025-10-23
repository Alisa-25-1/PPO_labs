#pragma once

#include "../../services/BookingService.hpp"
#include "../../dtos/BookingDTO.hpp"
#include <memory>
#include <vector>

class BookingController {
private:
    std::shared_ptr<BookingService> bookingService_;

public:
    BookingController(std::shared_ptr<BookingService> bookingService);
    
    // Основные методы бронирования
    BookingResponseDTO createBooking(const BookingRequestDTO& request);
    BookingResponseDTO cancelBooking(const UUID& bookingId, const UUID& clientId);
    std::vector<BookingResponseDTO> getClientBookings(const UUID& clientId);
    std::vector<DanceHall> getAvailableHalls();
    std::vector<TimeSlot> getAvailableTimeSlots(const UUID& hallId, const std::chrono::system_clock::time_point& date);
    
    // Валидация
    bool validateBookingRequest(const BookingRequestDTO& request) const;
    bool isTimeSlotAvailable(const UUID& hallId, const TimeSlot& timeSlot) const;
    
private:
    void initializeServices();
};
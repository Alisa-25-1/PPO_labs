#pragma once
#include "../repositories/IBookingRepository.hpp"
#include "../repositories/IClientRepository.hpp"
#include "../repositories/IHallRepository.hpp"
#include "../dtos/BookingDTO.hpp"
#include "../types/uuid.hpp"
#include "exceptions/BookingException.hpp"
#include "exceptions/ValidationException.hpp"
#include <memory>

class BookingService {
private:
    std::unique_ptr<IBookingRepository> bookingRepository_;
    std::unique_ptr<IClientRepository> clientRepository_;
    std::unique_ptr<IHallRepository> hallRepository_;

    // Validation methods
    void validateBookingRequest(const BookingRequestDTO& request) const;
    void validateClient(const UUID& clientId) const;
    void validateHall(const UUID& hallId) const;
    void validateTimeSlot(const TimeSlot& timeSlot) const;
    void checkBookingConflicts(const UUID& hallId, const TimeSlot& timeSlot, 
                              const UUID& excludeBookingId = UUID()) const;

public:
    // Constructor with dependency injection
    BookingService(
        std::unique_ptr<IBookingRepository> bookingRepo,
        std::unique_ptr<IClientRepository> clientRepo,
        std::unique_ptr<IHallRepository> hallRepo
    );

    // Main business logic methods
    BookingResponseDTO createBooking(const BookingRequestDTO& request);
    BookingResponseDTO cancelBooking(const UUID& bookingId, const UUID& clientId);
    BookingResponseDTO getBooking(const UUID& bookingId);
    std::vector<BookingResponseDTO> getClientBookings(const UUID& clientId);
    std::vector<BookingResponseDTO> getHallBookings(const UUID& hallId);
    bool isTimeSlotAvailable(const UUID& hallId, const TimeSlot& timeSlot) const;

    // Business rules
    bool canClientBook(const UUID& clientId) const;
    int getClientActiveBookingsCount(const UUID& clientId) const;
};
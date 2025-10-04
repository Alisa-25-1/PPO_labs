#pragma once
#include "../repositories/IBookingRepository.hpp"
#include "../repositories/IClientRepository.hpp"
#include "../repositories/IHallRepository.hpp"
#include "../dtos/BookingDTO.hpp"
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
    void validateClient(int clientId) const;
    void validateHall(int hallId) const;
    void validateTimeSlot(const TimeSlot& timeSlot) const;
    void checkBookingConflicts(int hallId, const TimeSlot& timeSlot, int excludeBookingId = 0) const;

public:
    // Constructor with dependency injection
    BookingService(
        std::unique_ptr<IBookingRepository> bookingRepo,
        std::unique_ptr<IClientRepository> clientRepo,
        std::unique_ptr<IHallRepository> hallRepo
    );

    // Main business logic methods
    BookingResponseDTO createBooking(const BookingRequestDTO& request);
    BookingResponseDTO cancelBooking(int bookingId, int clientId);
    BookingResponseDTO getBooking(int bookingId);
    std::vector<BookingResponseDTO> getClientBookings(int clientId);
    std::vector<BookingResponseDTO> getHallBookings(int hallId);
    bool isTimeSlotAvailable(int hallId, const TimeSlot& timeSlot) const;

    // Business rules
    bool canClientBook(int clientId) const;
    int getClientActiveBookingsCount(int clientId) const;
};
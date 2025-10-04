#include "BookingService.hpp"
#include <algorithm>

// Constructor
BookingService::BookingService(
    std::unique_ptr<IBookingRepository> bookingRepo,
    std::unique_ptr<IClientRepository> clientRepo,
    std::unique_ptr<IHallRepository> hallRepo
) : bookingRepository_(std::move(bookingRepo)),
    clientRepository_(std::move(clientRepo)),
    hallRepository_(std::move(hallRepo)) {}

// Validation methods
void BookingService::validateBookingRequest(const BookingRequestDTO& request) const {
    if (!request.validate()) {
        throw ValidationException("Invalid booking request data");
    }
    
    validateClient(request.clientId);
    validateHall(request.hallId);
    validateTimeSlot(request.timeSlot);
    
    if (!Booking::isValidPurpose(request.purpose)) {
        throw ValidationException("Invalid booking purpose");
    }
}

void BookingService::validateClient(int clientId) const {
    auto client = clientRepository_->findById(clientId);
    if (!client) {
        throw ValidationException("Client not found");
    }
    
    if (!client->isActive()) {
        throw BusinessRuleException("Client account is not active");
    }
}

void BookingService::validateHall(int hallId) const {
    if (!hallRepository_->exists(hallId)) {
        throw ValidationException("Hall not found");
    }
}

void BookingService::validateTimeSlot(const TimeSlot& timeSlot) const {
    if (!timeSlot.isValid()) {
        throw ValidationException("Invalid time slot");
    }
    
    if (!Booking::isFutureBooking(timeSlot)) {
        throw BusinessRuleException("Booking must be for future time");
    }
}

void BookingService::checkBookingConflicts(int hallId, const TimeSlot& timeSlot, int excludeBookingId) const {
    auto conflictingBookings = bookingRepository_->findConflictingBookings(hallId, timeSlot);
    
    // Filter out the excluded booking (for updates)
    if (excludeBookingId > 0) {
        conflictingBookings.erase(
            std::remove_if(conflictingBookings.begin(), conflictingBookings.end(),
                [excludeBookingId](const Booking& booking) {
                    return booking.getId() == excludeBookingId;
                }),
            conflictingBookings.end()
        );
    }
    
    if (!conflictingBookings.empty()) {
        throw BookingConflictException("Time slot conflicts with existing booking");
    }
}

// Main business logic methods
BookingResponseDTO BookingService::createBooking(const BookingRequestDTO& request) {
    // Validate the request
    validateBookingRequest(request);
    
    // Check business rules
    if (!canClientBook(request.clientId)) {
        throw BusinessRuleException("Client cannot create new booking");
    }
    
    // Check for time conflicts
    checkBookingConflicts(request.hallId, request.timeSlot);
    
    // Create booking
    int newId = 1; // In real implementation, this would be generated
    Booking booking(newId, request.clientId, request.hallId, request.timeSlot, request.purpose);
    booking.confirm();
    
    if (!bookingRepository_->save(booking)) {
        throw BookingException("Failed to save booking");
    }
    
    return BookingResponseDTO(booking);
}

BookingResponseDTO BookingService::cancelBooking(int bookingId, int clientId) {
    auto booking = bookingRepository_->findById(bookingId);
    if (!booking) {
        throw BookingNotFoundException("Booking not found");
    }
    
    if (booking->getClientId() != clientId) {
        throw BusinessRuleException("Client can only cancel their own bookings");
    }
    
    if (booking->isCancelled()) {
        throw BusinessRuleException("Booking is already cancelled");
    }
    
    if (booking->isCompleted()) {
        throw BusinessRuleException("Cannot cancel completed booking");
    }
    
    booking->cancel();
    
    if (!bookingRepository_->update(*booking)) {
        throw BookingException("Failed to cancel booking");
    }
    
    return BookingResponseDTO(*booking);
}

BookingResponseDTO BookingService::getBooking(int bookingId) {
    auto booking = bookingRepository_->findById(bookingId);
    if (!booking) {
        throw BookingNotFoundException("Booking not found");
    }
    
    return BookingResponseDTO(*booking);
}

std::vector<BookingResponseDTO> BookingService::getClientBookings(int clientId) {
    validateClient(clientId);
    
    auto bookings = bookingRepository_->findByClientId(clientId);
    std::vector<BookingResponseDTO> result;
    
    for (const auto& booking : bookings) {
        result.push_back(BookingResponseDTO(booking));
    }
    
    return result;
}

std::vector<BookingResponseDTO> BookingService::getHallBookings(int hallId) {
    validateHall(hallId);
    
    auto bookings = bookingRepository_->findByHallId(hallId);
    std::vector<BookingResponseDTO> result;
    
    for (const auto& booking : bookings) {
        result.push_back(BookingResponseDTO(booking));
    }
    
    return result;
}

bool BookingService::isTimeSlotAvailable(int hallId, const TimeSlot& timeSlot) const {
    // Проверяем существование зала
    if (!hallRepository_->exists(hallId)) {
        return false;
    }
    
    // Проверяем конфликты
    try {
        checkBookingConflicts(hallId, timeSlot);
        return true;
    } catch (const BookingConflictException&) {
        return false;
    }
}

// Business rules
bool BookingService::canClientBook(int clientId) const {
    // Business rule: client can have maximum 3 active bookings
    return getClientActiveBookingsCount(clientId) < 3;
}

int BookingService::getClientActiveBookingsCount(int clientId) const {
    auto bookings = bookingRepository_->findByClientId(clientId);
    int activeCount = 0;
    
    for (const auto& booking : bookings) {
        if (booking.isActive()) {
            activeCount++;
        }
    }
    
    return activeCount;
}
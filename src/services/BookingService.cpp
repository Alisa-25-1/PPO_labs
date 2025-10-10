#include "BookingService.hpp"
#include <algorithm>

// Constructor
BookingService::BookingService(
    std::unique_ptr<IBookingRepository> bookingRepo,
    std::unique_ptr<IClientRepository> clientRepo,
    std::unique_ptr<IDanceHallRepository> hallRepo
) : bookingRepository_(std::move(bookingRepo)),
    clientRepository_(std::move(clientRepo)),
    hallRepository_(std::move(hallRepo)) {}

// Validation methods
void BookingService::validateBookingRequest(const BookingRequestDTO& request) const {
    if (!request.validate()) {
        throw ValidationException("Invalid booking request data");
    }
    
    validateClient(request.clientId);
    validateDanceHall(request.hallId);  // Исправлено
    validateTimeSlot(request.timeSlot);
    
    if (!Booking::isValidPurpose(request.purpose)) {
        throw ValidationException("Invalid booking purpose");
    }
}

void BookingService::validateClient(const UUID& clientId) const {
    auto client = clientRepository_->findById(clientId);
    if (!client) {
        throw ValidationException("Client not found");
    }
    
    if (!client->isActive()) {
        throw BusinessRuleException("Client account is not active");
    }
}

void BookingService::validateDanceHall(const UUID& hallId) const {  // Исправлено
    if (!hallRepository_->exists(hallId)) {
        throw ValidationException("Dance hall not found");
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

void BookingService::checkBookingConflicts(const UUID& hallId, const TimeSlot& timeSlot, const UUID& excludeBookingId) const {
    auto conflictingBookings = bookingRepository_->findConflictingBookings(hallId, timeSlot);
    
    // Filter out the excluded booking (for updates)
    if (!excludeBookingId.isNull()) {
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
    UUID newId = UUID::generate();
    Booking booking(newId, request.clientId, request.hallId, request.timeSlot, request.purpose);
    booking.confirm();
    
    if (!bookingRepository_->save(booking)) {
        throw BookingException("Failed to save booking");
    }
    
    return BookingResponseDTO(booking);
}

BookingResponseDTO BookingService::cancelBooking(const UUID& bookingId, const UUID& clientId) {
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

BookingResponseDTO BookingService::getBooking(const UUID& bookingId) {
    auto booking = bookingRepository_->findById(bookingId);
    if (!booking) {
        throw BookingNotFoundException("Booking not found");
    }
    
    return BookingResponseDTO(*booking);
}

std::vector<BookingResponseDTO> BookingService::getClientBookings(const UUID& clientId) {
    validateClient(clientId);
    
    auto bookings = bookingRepository_->findByClientId(clientId);
    std::vector<BookingResponseDTO> result;
    
    for (const auto& booking : bookings) {
        result.push_back(BookingResponseDTO(booking));
    }
    
    return result;
}

std::vector<BookingResponseDTO> BookingService::getDanceHallBookings(const UUID& hallId) {  // Исправлено
    validateDanceHall(hallId);  // Исправлено
    
    auto bookings = bookingRepository_->findByHallId(hallId);
    std::vector<BookingResponseDTO> result;
    
    for (const auto& booking : bookings) {
        result.push_back(BookingResponseDTO(booking));
    }
    
    return result;
}

bool BookingService::isTimeSlotAvailable(const UUID& hallId, const TimeSlot& timeSlot) const {
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
bool BookingService::canClientBook(const UUID& clientId) const {
    // Business rule: client can have maximum 3 active bookings
    return getClientActiveBookingsCount(clientId) < 3;
}

int BookingService::getClientActiveBookingsCount(const UUID& clientId) const {
    auto bookings = bookingRepository_->findByClientId(clientId);
    int activeCount = 0;
    
    for (const auto& booking : bookings) {
        if (booking.isActive()) {
            activeCount++;
        }
    }
    
    return activeCount;
}

std::vector<TimeSlot> BookingService::getAvailableTimeSlots(const UUID& hallId, 
                                                           const std::chrono::system_clock::time_point& date) const {
    validateDanceHall(hallId);
    
    // Получаем все бронирования для зала на указанную дату
    auto bookings = bookingRepository_->findByHallId(hallId);
    
    // Фильтруем бронирования по дате
    std::vector<Booking> dayBookings;
    std::copy_if(bookings.begin(), bookings.end(), std::back_inserter(dayBookings),
        [&date](const Booking& booking) {
            auto bookingTime = std::chrono::system_clock::to_time_t(booking.getTimeSlot().getStartTime());
            auto targetTime = std::chrono::system_clock::to_time_t(date);
            
            std::tm bookingTm = *std::localtime(&bookingTime);
            std::tm targetTm = *std::localtime(&targetTime);
            
            return bookingTm.tm_year == targetTm.tm_year &&
                   bookingTm.tm_mon == targetTm.tm_mon &&
                   bookingTm.tm_mday == targetTm.tm_mday;
        });
    
        // Генерируем доступные слоты (упрощенная логика)
    std::vector<TimeSlot> availableSlots;
    auto startTime = std::chrono::system_clock::to_time_t(date);
    std::tm tm = *std::localtime(&startTime);
    
    // Рабочие часы: 9:00 - 22:00
    for (int hour = 9; hour < 22; hour++) {
        tm.tm_hour = hour;
        tm.tm_min = 0;
        auto slotStart = std::chrono::system_clock::from_time_t(std::mktime(&tm));
        
        TimeSlot slot(slotStart, 60); // 1 час слот
        
        // Проверяем, нет ли конфликтов
        if (isTimeSlotAvailable(hallId, slot)) {
            availableSlots.push_back(slot);
        }
    }
    
    return availableSlots;
}
#ifndef BOOKINGDTO_HPP
#define BOOKINGDTO_HPP

#include "../types/uuid.hpp"
#include "../models/TimeSlot.hpp"
#include "../models/Booking.hpp"
#include <string>

struct BookingRequestDTO {
    UUID clientId;
    UUID hallId;
    TimeSlot timeSlot;
    std::string purpose;
    
    // Конструктор по умолчанию
    BookingRequestDTO() = default;
    
    BookingRequestDTO(const UUID& clientId, const UUID& hallId, 
                     const TimeSlot& timeSlot, const std::string& purpose);
    bool validate() const;
};

struct BookingResponseDTO {
    UUID bookingId;
    UUID clientId;
    UUID hallId;
    TimeSlot timeSlot;
    std::string status;
    std::string purpose;
    std::string createdAt;
    
    // Конструктор по умолчанию
    BookingResponseDTO() = default;
    
    BookingResponseDTO(const Booking& booking);
    
    // Дополнительный конструктор для тестовых данных
    BookingResponseDTO(const UUID& bookingId, const UUID& clientId, const UUID& hallId,
                      const TimeSlot& timeSlot, const std::string& status,
                      const std::string& purpose, const std::string& createdAt);
};

#endif // BOOKINGDTO_HPP
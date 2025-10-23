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
    
    BookingResponseDTO(const Booking& booking);
};

#endif // BOOKINGDTO_HPP
#ifndef BOOKINGDTO_HPP
#define BOOKINGDTO_HPP

#include "../models/TimeSlot.hpp"
#include "../models/Booking.hpp"
#include <string>

struct BookingRequestDTO {
    int clientId;
    int hallId;
    TimeSlot timeSlot;
    std::string purpose;
    
    BookingRequestDTO(int clientId, int hallId, const TimeSlot& timeSlot, const std::string& purpose);
    bool validate() const;
};

struct BookingResponseDTO {
    int bookingId;
    int clientId;
    int hallId;
    TimeSlot timeSlot;
    std::string status;
    std::string purpose;
    std::string createdAt;
    
    BookingResponseDTO(const Booking& booking);
};

#endif // BOOKINGDTO_HPP
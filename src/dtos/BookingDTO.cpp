#include "BookingDTO.hpp"
#include <sstream>
#include <iomanip>

BookingRequestDTO::BookingRequestDTO(int clientId, int hallId, const TimeSlot& timeSlot, const std::string& purpose)
    : clientId(clientId), hallId(hallId), timeSlot(timeSlot), purpose(purpose) {}

bool BookingRequestDTO::validate() const {
    return clientId > 0 && 
           hallId > 0 && 
           timeSlot.isValid() && 
           !purpose.empty() && 
           purpose.length() <= 255;
}

BookingResponseDTO::BookingResponseDTO(const Booking& booking) {
    bookingId = booking.getId();
    clientId = booking.getClientId();
    hallId = booking.getHallId();
    timeSlot = booking.getTimeSlot();
    purpose = booking.getPurpose();

    // Convert status enum to string
    switch (booking.getStatus()) {
        case BookingStatus::PENDING:
            status = "PENDING";
            break;
        case BookingStatus::CONFIRMED:
            status = "CONFIRMED";
            break;
        case BookingStatus::CANCELLED:
            status = "CANCELLED";
            break;
        case BookingStatus::COMPLETED:
            status = "COMPLETED";
            break;
        default:
            status = "UNKNOWN";
            break;
    }

    // Format created at time
    auto time_t = std::chrono::system_clock::to_time_t(booking.getCreatedAt());
    std::tm tm = *std::localtime(&time_t);
    std::ostringstream oss;
    oss << std::put_time(&tm, "%Y-%m-%d %H:%M:%S");
    createdAt = oss.str();
}
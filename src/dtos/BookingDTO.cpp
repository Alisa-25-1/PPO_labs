#include "BookingDTO.hpp"
#include <sstream>
#include <iomanip>

BookingRequestDTO::BookingRequestDTO(const UUID& clientId, const UUID& hallId, 
                                   const TimeSlot& timeSlot, const std::string& purpose)
    : clientId(clientId), hallId(hallId), timeSlot(timeSlot), purpose(purpose) {}

bool BookingRequestDTO::validate() const {
    // Базовая проверка purpose
    if (purpose.empty() || purpose.length() < 3 || purpose.length() > 500) {
        return false;
    }
    
    // Проверка на наличие только пробелов
    bool hasNonSpace = false;
    for (char c : purpose) {
        if (!std::isspace(static_cast<unsigned char>(c))) {
            hasNonSpace = true;
            break;
        }
    }
    if (!hasNonSpace) {
        return false;
    }
    
    // Проверка опасных символов в purpose
    const std::string forbiddenChars = "<>&\"';=()[]{}|\\`~!@#$%^*+?/:\n\r\t";
    for (char c : purpose) {
        if (static_cast<unsigned char>(c) < 32 && c != ' ' && c != '\t' && c != '\n' && c != '\r') {
            return false;
        }
        if (forbiddenChars.find(c) != std::string::npos) {
            return false;
        }
    }
    
    return !clientId.isNull() && clientId.isValid() &&
           !hallId.isNull() && hallId.isValid() && 
           timeSlot.isValid();
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

BookingResponseDTO::BookingResponseDTO(const UUID& bookingId, const UUID& clientId, 
                                     const UUID& hallId, const TimeSlot& timeSlot,
                                     const std::string& status, const std::string& purpose,
                                     const std::string& createdAt)
    : bookingId(bookingId), clientId(clientId), hallId(hallId), timeSlot(timeSlot),
      status(status), purpose(purpose), createdAt(createdAt) {}
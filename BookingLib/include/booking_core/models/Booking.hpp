#ifndef BOOKING_HPP
#define BOOKING_HPP

#include "TimeSlot.hpp"
#include "../types/uuid.hpp"
#include "../types/enums.hpp"
#include <string>
#include <chrono>

class Booking {
private:
    UUID id_;
    UUID clientId_;
    UUID hallId_;
    TimeSlot timeSlot_;
    BookingStatus status_;
    std::string purpose_;
    std::chrono::system_clock::time_point createdAt_;

public:
    Booking();
    Booking(const UUID& id, const UUID& clientId, const UUID& hallId, 
            const TimeSlot& timeSlot, const std::string& purpose);
    
    // Геттеры
    UUID getId() const;
    UUID getClientId() const;
    UUID getHallId() const;
    TimeSlot getTimeSlot() const;
    BookingStatus getStatus() const;
    std::string getPurpose() const;
    std::chrono::system_clock::time_point getCreatedAt() const;
    
    // Бизнес-логика
    bool isActive() const;
    bool isCancelled() const;
    bool isCompleted() const;
    void confirm();
    void cancel();
    void complete();
    bool overlapsWith(const Booking& other) const;
    bool isValid() const;
    
    // Валидация
    static bool isValidPurpose(const std::string& purpose);
    static bool isFutureBooking(const TimeSlot& timeSlot);
    static bool isValidDuration(int durationMinutes);
    static bool isValidStatusTransition(BookingStatus from, BookingStatus to);

    // Операторы сравнения
    bool operator==(const Booking& other) const;
    bool operator!=(const Booking& other) const;
};

#endif // BOOKING_HPP
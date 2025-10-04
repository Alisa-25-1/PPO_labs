#ifndef BOOKING_HPP
#define BOOKING_HPP

#include "TimeSlot.hpp"
#include <string>

enum class BookingStatus {
    PENDING,
    CONFIRMED,
    CANCELLED,
    COMPLETED
};

class Booking {
private:
    int id_;
    int clientId_;
    int hallId_;
    TimeSlot timeSlot_;
    BookingStatus status_;
    std::string purpose_;
    std::chrono::system_clock::time_point createdAt_;

public:
    Booking();
    Booking(int id, int clientId, int hallId, const TimeSlot& timeSlot, const std::string& purpose);
    
    int getId() const;
    int getClientId() const;
    int getHallId() const;
    TimeSlot getTimeSlot() const;
    BookingStatus getStatus() const;
    std::string getPurpose() const;
    std::chrono::system_clock::time_point getCreatedAt() const;
    
    bool isActive() const;
    bool isCancelled() const;
    bool isCompleted() const;
    void confirm();
    void cancel();
    void complete();
    bool overlapsWith(const Booking& other) const;
    bool isValid() const;
    
    static bool isValidPurpose(const std::string& purpose);
    static bool isFutureBooking(const TimeSlot& timeSlot);

    // Операторы сравнения
    bool operator==(const Booking& other) const;
    bool operator!=(const Booking& other) const;
};

#endif // BOOKING_HPP
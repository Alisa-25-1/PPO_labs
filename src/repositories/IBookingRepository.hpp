#pragma once
#include "../models/Booking.hpp"
#include <memory>
#include <optional>
#include <vector>

class IBookingRepository {
public:
    virtual ~IBookingRepository() = default;
    
    virtual std::optional<Booking> findById(int id) = 0;
    virtual std::vector<Booking> findByClientId(int clientId) = 0;
    virtual std::vector<Booking> findByHallId(int hallId) = 0;
    virtual std::vector<Booking> findConflictingBookings(int hallId, const TimeSlot& timeSlot) = 0;
    virtual bool save(const Booking& booking) = 0;
    virtual bool update(const Booking& booking) = 0;
    virtual bool remove(int id) = 0;
    virtual bool exists(int id) = 0;
};
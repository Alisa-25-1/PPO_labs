#pragma once
#include "../types/uuid.hpp"
#include "../models/Booking.hpp"
#include <memory>
#include <optional>
#include <vector>

class IBookingRepository {
public:
    virtual ~IBookingRepository() = default;
    
    virtual std::optional<Booking> findById(const UUID&  id) = 0;
    virtual std::vector<Booking> findByClientId(const UUID&  clientId) = 0;
    virtual std::vector<Booking> findByHallId(const UUID&  hallId) = 0;
    virtual std::vector<Booking> findConflictingBookings(const UUID&  hallId, const TimeSlot& timeSlot) = 0;
    virtual bool save(const Booking& booking) = 0;
    virtual bool update(const Booking& booking) = 0;
    virtual bool remove(const UUID& id) = 0;
    virtual bool exists(const UUID&  id) = 0;
};
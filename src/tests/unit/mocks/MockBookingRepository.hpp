#pragma once
#include "../../../repositories/IBookingRepository.hpp"
#include <gmock/gmock.h>

class MockBookingRepository : public IBookingRepository {
public:
    MOCK_METHOD(std::optional<Booking>, findById, (const UUID& id), (override));
    MOCK_METHOD(std::vector<Booking>, findByClientId, (const UUID& clientId), (override));
    MOCK_METHOD(std::vector<Booking>, findByHallId, (const UUID& hallId), (override));
    MOCK_METHOD(std::vector<Booking>, findConflictingBookings, (const UUID& hallId, const TimeSlot& timeSlot), (override));
    MOCK_METHOD(bool, save, (const Booking& booking), (override));
    MOCK_METHOD(bool, update, (const Booking& booking), (override));
    MOCK_METHOD(bool, remove, (const UUID& id), (override));
    MOCK_METHOD(bool, exists, (const UUID& id), (override));
};
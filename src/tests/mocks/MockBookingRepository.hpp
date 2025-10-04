#pragma once
#include "../../repositories/IBookingRepository.hpp"
#include <gmock/gmock.h>

class MockBookingRepository : public IBookingRepository {
public:
    MOCK_METHOD(std::optional<Booking>, findById, (int id), (override));
    MOCK_METHOD(std::vector<Booking>, findByClientId, (int clientId), (override));
    MOCK_METHOD(std::vector<Booking>, findByHallId, (int hallId), (override));
    MOCK_METHOD(std::vector<Booking>, findConflictingBookings, (int hallId, const TimeSlot& timeSlot), (override));
    MOCK_METHOD(bool, save, (const Booking& booking), (override));
    MOCK_METHOD(bool, update, (const Booking& booking), (override));
    MOCK_METHOD(bool, remove, (int id), (override));
    MOCK_METHOD(bool, exists, (int id), (override));
};
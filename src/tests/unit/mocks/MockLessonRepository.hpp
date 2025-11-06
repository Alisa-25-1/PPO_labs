#pragma once
#include <gmock/gmock.h>
#include "../../../repositories/ILessonRepository.hpp"

class MockLessonRepository : public ILessonRepository {
public:
    MOCK_METHOD(std::optional<Lesson>, findById, (const UUID& id), (override));
    MOCK_METHOD(std::vector<Lesson>, findByTrainerId, (const UUID& trainerId), (override));
    MOCK_METHOD(std::vector<Lesson>, findByHallId, (const UUID& hallId), (override)); 
    MOCK_METHOD(std::vector<Lesson>, findConflictingLessons, (const UUID& hallId, const TimeSlot& timeSlot), (override));
    MOCK_METHOD(std::vector<Lesson>, findUpcomingLessons, (int days), (override)); 
    MOCK_METHOD(bool, save, (const Lesson& lesson), (override));
    MOCK_METHOD(bool, update, (const Lesson& lesson), (override));
    MOCK_METHOD(bool, remove, (const UUID& id), (override));
    MOCK_METHOD(bool, exists, (const UUID& id), (override));
};
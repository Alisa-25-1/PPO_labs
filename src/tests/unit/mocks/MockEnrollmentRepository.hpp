#pragma once
#include "../../../repositories/IEnrollmentRepository.hpp"
#include <gmock/gmock.h>

class MockEnrollmentRepository : public IEnrollmentRepository {
public:
    MOCK_METHOD(std::optional<Enrollment>, findById, (const UUID& id), (override));
    MOCK_METHOD(std::vector<Enrollment>, findByClientId, (const UUID& clientId), (override));
    MOCK_METHOD(std::vector<Enrollment>, findByLessonId, (const UUID& lessonId), (override));
    MOCK_METHOD(std::optional<Enrollment>, findByClientAndLesson, (const UUID& clientId, const UUID& lessonId), (override));
    MOCK_METHOD(int, countByLessonId, (const UUID& lessonId), (override));
    MOCK_METHOD(bool, save, (const Enrollment& enrollment), (override));
    MOCK_METHOD(bool, update, (const Enrollment& enrollment), (override));
    MOCK_METHOD(bool, remove, (const UUID& id), (override));
    MOCK_METHOD(bool, exists, (const UUID& id), (override));
};
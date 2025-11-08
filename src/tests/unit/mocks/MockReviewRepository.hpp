#pragma once
#include "../../../repositories/IReviewRepository.hpp"
#include <gmock/gmock.h>

class MockReviewRepository : public IReviewRepository {
public:
    MOCK_METHOD(std::optional<Review>, findById, (const UUID& id), (override));
    MOCK_METHOD(std::vector<Review>, findByClientId, (const UUID& clientId), (override));
    MOCK_METHOD(std::vector<Review>, findByLessonId, (const UUID& lessonId), (override));
    MOCK_METHOD(std::optional<Review>, findByClientAndLesson, (const UUID& clientId, const UUID& lessonId), (override));
    MOCK_METHOD(std::vector<Review>, findPendingModeration, (), (override));
    MOCK_METHOD(double, getAverageRatingForTrainer, (const UUID& trainerId), (override));
    MOCK_METHOD(std::vector<Review>, findAll, (), (override));
    MOCK_METHOD(bool, save, (const Review& review), (override));
    MOCK_METHOD(bool, update, (const Review& review), (override));
    MOCK_METHOD(bool, remove, (const UUID& id), (override));
    MOCK_METHOD(bool, exists, (const UUID& id), (override));
};
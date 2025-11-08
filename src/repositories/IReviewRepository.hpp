#pragma once
#include "../types/uuid.hpp"
#include "../models/Review.hpp"
#include <memory>
#include <optional>
#include <vector>

class IReviewRepository {
public:
    virtual ~IReviewRepository() = default;
    
    virtual std::optional<Review> findById(const UUID& id) = 0;
    virtual std::vector<Review> findByClientId(const UUID& clientId) = 0;
    virtual std::vector<Review> findByLessonId(const UUID& lessonId) = 0;
    virtual std::optional<Review> findByClientAndLesson(const UUID& clientId, const UUID& lessonId) = 0;
    virtual std::vector<Review> findPendingModeration() = 0;
    virtual double getAverageRatingForTrainer(const UUID& trainerId) = 0;
    virtual std::vector<Review> findAll() = 0;
    virtual bool save(const Review& review) = 0;
    virtual bool update(const Review& review) = 0;
    virtual bool remove(const UUID& id) = 0;
    virtual bool exists(const UUID& id) = 0;
};
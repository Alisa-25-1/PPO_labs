#ifndef MONGODB_REVIEW_REPOSITORY_HPP
#define MONGODB_REVIEW_REPOSITORY_HPP

#include "../IReviewRepository.hpp"
#include "../../data/exceptions/DataAccessException.hpp"
#include <mongocxx/client.hpp>
#include <mongocxx/collection.hpp>
#include <bsoncxx/builder/stream/document.hpp>
#include <bsoncxx/json.hpp>
#include <iostream>

// Предварительное объявление
class MongoDBRepositoryFactory;

class MongoDBReviewRepository : public IReviewRepository {
private:
    std::shared_ptr<MongoDBRepositoryFactory> factory_;
    mongocxx::collection getCollection() const;

public:
    explicit MongoDBReviewRepository(std::shared_ptr<MongoDBRepositoryFactory> factory);
    
    std::optional<Review> findById(const UUID& id) override;
    std::vector<Review> findByClientId(const UUID& clientId) override;
    std::vector<Review> findByLessonId(const UUID& lessonId) override;
    std::optional<Review> findByClientAndLesson(const UUID& clientId, const UUID& lessonId) override;
    std::vector<Review> findPendingModeration() override;
    std::vector<Review> findAll() override;
    double getAverageRatingForTrainer(const UUID& trainerId) override;
    bool save(const Review& review) override;
    bool update(const Review& review) override;
    bool remove(const UUID& id) override;
    bool exists(const UUID& id) override;

private:
    Review mapDocumentToReview(const bsoncxx::document::view& doc) const;
    bsoncxx::document::value mapReviewToDocument(const Review& review) const;
    void validateReview(const Review& review) const;
    
    // Вспомогательные методы для преобразования enum
    std::string reviewStatusToString(ReviewStatus status) const;
    ReviewStatus stringToReviewStatus(const std::string& status) const;
};

#endif // MONGODB_REVIEW_REPOSITORY_HPP
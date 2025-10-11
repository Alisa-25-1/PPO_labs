#ifndef POSTGRESQL_REVIEW_REPOSITORY_HPP
#define POSTGRESQL_REVIEW_REPOSITORY_HPP

#include "../IReviewRepository.hpp"
#include "../../data/DatabaseConnection.hpp"
#include "../../data/exceptions/DataAccessException.hpp"
#include <memory>

class PostgreSQLReviewRepository : public IReviewRepository {
public:
    explicit PostgreSQLReviewRepository(std::shared_ptr<DatabaseConnection> dbConnection);
    
    std::optional<Review> findById(const UUID& id) override;
    std::vector<Review> findByClientId(const UUID& clientId) override;
    std::vector<Review> findByLessonId(const UUID& lessonId) override;
    std::optional<Review> findByClientAndLesson(const UUID& clientId, const UUID& lessonId) override;
    std::vector<Review> findPendingModeration() override;
    double getAverageRatingForTrainer(const UUID& trainerId) override;
    bool save(const Review& review) override;
    bool update(const Review& review) override;
    bool remove(const UUID& id) override;
    bool exists(const UUID& id) override;

private:
    std::shared_ptr<DatabaseConnection> dbConnection_;
    
    Review mapResultToReview(const pqxx::row& row) const;
    void validateReview(const Review& review) const;
    std::string reviewStatusToString(ReviewStatus status) const;
    ReviewStatus stringToReviewStatus(const std::string& status) const;
};

#endif // POSTGRESQL_REVIEW_REPOSITORY_HPP
#include "PostgreSQLReviewRepository.hpp"
#include "../../data/SqlQueryBuilder.hpp"
#include "../../data/DateTimeUtils.hpp"
#include "../../data/QueryFactory.hpp"

PostgreSQLReviewRepository::PostgreSQLReviewRepository(
    std::shared_ptr<DatabaseConnection> dbConnection)
    : dbConnection_(std::move(dbConnection)) {}

std::optional<Review> PostgreSQLReviewRepository::findById(const UUID& id) {
    try {
        auto work = dbConnection_->beginTransaction();
        
        SqlQueryBuilder queryBuilder;
        std::string query = queryBuilder
            .select({"id", "client_id", "lesson_id", "rating", "comment", "publication_date", "status"})
            .from("reviews")
            .where("id = $1")
            .build();
        
        auto result = work.exec_params(query, id.toString());
        
        if (result.empty()) {
            return std::nullopt;
        }
        
        auto review = mapResultToReview(result[0]);
        dbConnection_->commitTransaction(work);
        return review;
        
    } catch (const std::exception& e) {
        throw QueryException(std::string("Failed to find review by ID: ") + e.what());
    }
}

std::vector<Review> PostgreSQLReviewRepository::findByClientId(const UUID& clientId) {
    try {
        auto work = dbConnection_->beginTransaction();
        
        SqlQueryBuilder queryBuilder;
        std::string query = queryBuilder
            .select({"id", "client_id", "lesson_id", "rating", "comment", "publication_date", "status"})
            .from("reviews")
            .where("client_id = $1")
            .build();
        
        auto result = work.exec_params(query, clientId.toString());
        
        std::vector<Review> reviews;
        for (const auto& row : result) {
            reviews.push_back(mapResultToReview(row));
        }
        
        dbConnection_->commitTransaction(work);
        return reviews;
        
    } catch (const std::exception& e) {
        throw QueryException(std::string("Failed to find reviews by client ID: ") + e.what());
    }
}

std::vector<Review> PostgreSQLReviewRepository::findByLessonId(const UUID& lessonId) {
    try {
        auto work = dbConnection_->beginTransaction();
        
        SqlQueryBuilder queryBuilder;
        std::string query = queryBuilder
            .select({"id", "client_id", "lesson_id", "rating", "comment", "publication_date", "status"})
            .from("reviews")
            .where("lesson_id = $1")
            .build();
        
        auto result = work.exec_params(query, lessonId.toString());
        
        std::vector<Review> reviews;
        for (const auto& row : result) {
            reviews.push_back(mapResultToReview(row));
        }
        
        dbConnection_->commitTransaction(work);
        return reviews;
        
    } catch (const std::exception& e) {
        throw QueryException(std::string("Failed to find reviews by lesson ID: ") + e.what());
    }
}

std::optional<Review> PostgreSQLReviewRepository::findByClientAndLesson(
    const UUID& clientId, const UUID& lessonId) {
    
    try {
        auto work = dbConnection_->beginTransaction();
        
        SqlQueryBuilder queryBuilder;
        std::string query = queryBuilder
            .select({"id", "client_id", "lesson_id", "rating", "comment", "publication_date", "status"})
            .from("reviews")
            .where("client_id = $1 AND lesson_id = $2")
            .build();
        
        auto result = work.exec_params(query, clientId.toString(), lessonId.toString());
        
        if (result.empty()) {
            return std::nullopt;
        }
        
        auto review = mapResultToReview(result[0]);
        dbConnection_->commitTransaction(work);
        return review;
        
    } catch (const std::exception& e) {
        throw QueryException(std::string("Failed to find review by client and lesson: ") + e.what());
    }
}

std::vector<Review> PostgreSQLReviewRepository::findPendingModeration() {
    try {
        auto work = dbConnection_->beginTransaction();
        SqlQueryBuilder queryBuilder;
        std::string query = queryBuilder
            .select({"id", "client_id", "lesson_id", "rating", "comment", "publication_date", "status"})
            .from("reviews")
            .where("status = 'PENDING_MODERATION'")
            .build();
        
        auto result = work.exec(query);
        
        std::vector<Review> reviews;
        for (const auto& row : result) {
            reviews.push_back(mapResultToReview(row));
        }
        
        dbConnection_->commitTransaction(work);
        return reviews;
        
    } catch (const std::exception& e) {
        throw QueryException(std::string("Failed to find pending reviews: ") + e.what());
    }
}

std::vector<Review> PostgreSQLReviewRepository::findAll() {
    try {
        auto work = dbConnection_->beginTransaction();
        
        SqlQueryBuilder queryBuilder;
        std::string query = queryBuilder
            .select({"id", "client_id", "lesson_id", "rating", "comment", "publication_date", "status"})
            .from("reviews")
            .orderBy("publication_date", false)
            .build();
        
        auto result = work.exec(query);
        
        std::vector<Review> reviews;
        for (const auto& row : result) {
            reviews.push_back(mapResultToReview(row));
        }
        
        dbConnection_->commitTransaction(work);
        return reviews;
        
    } catch (const std::exception& e) {
        throw QueryException(std::string("Failed to find all reviews: ") + e.what());
    }
}

double PostgreSQLReviewRepository::getAverageRatingForTrainer(const UUID& trainerId) {
    try {
        auto work = dbConnection_->beginTransaction();
        
        std::string query = QueryFactory::createGetAverageRatingForTrainerQuery();
        auto result = work.exec_params(query, trainerId.toString());
        
        if (result.empty() || result[0]["avg_rating"].is_null()) {
            return 0.0;
        }
        
        double avgRating = result[0]["avg_rating"].as<double>();
        dbConnection_->commitTransaction(work);
        return avgRating;
        
    } catch (const std::exception& e) {
        throw QueryException(std::string("Failed to get average rating for trainer: ") + e.what());
    }
}

bool PostgreSQLReviewRepository::save(const Review& review) {
    validateReview(review);
    
    try {
        auto work = dbConnection_->beginTransaction();
        
        std::map<std::string, std::string> values = {
            {"id", "$1"},
            {"client_id", "$2"},
            {"lesson_id", "$3"},
            {"rating", "$4"},
            {"comment", "$5"},
            {"publication_date", "$6"},
            {"status", "$7"}
        };
        
        SqlQueryBuilder queryBuilder;
        std::string query = queryBuilder
            .insertInto("reviews")
            .values(values)
            .build();
        
        work.exec_params(
            query,
            review.getId().toString(),
            review.getClientId().toString(),
            review.getLessonId().toString(),
            std::to_string(review.getRating()),
            review.getComment(),
            DateTimeUtils::formatTimeForPostgres(review.getPublicationDate()),
            reviewStatusToString(review.getStatus())
        );
        
        dbConnection_->commitTransaction(work);
        return true;
        
    } catch (const std::exception& e) {
        throw QueryException(std::string("Failed to save review: ") + e.what());
    }
}

bool PostgreSQLReviewRepository::update(const Review& review) {
    validateReview(review);
    
    try {
        auto work = dbConnection_->beginTransaction();
        
        std::map<std::string, std::string> values = {
            {"client_id", "$2"},
            {"lesson_id", "$3"},
            {"rating", "$4"},
            {"comment", "$5"},
            {"status", "$6"}
        };
        
        SqlQueryBuilder queryBuilder;
        std::string query = queryBuilder
            .update("reviews")
            .set(values)
            .where("id = $1")
            .build();
        
        auto result = work.exec_params(
            query,
            review.getId().toString(),
            review.getClientId().toString(),
            review.getLessonId().toString(),
            std::to_string(review.getRating()),
            review.getComment(),
            reviewStatusToString(review.getStatus())
        );
        
        dbConnection_->commitTransaction(work);
        return result.affected_rows() > 0;
        
    } catch (const std::exception& e) {
        throw QueryException(std::string("Failed to update review: ") + e.what());
    }
}

bool PostgreSQLReviewRepository::remove(const UUID& id) {
    try {
        auto work = dbConnection_->beginTransaction();
        
        SqlQueryBuilder queryBuilder;
        std::string query = queryBuilder
            .deleteFrom("reviews")
            .where("id = $1")
            .build();
            
        auto result = work.exec_params(query, id.toString());
        
        dbConnection_->commitTransaction(work);
        return result.affected_rows() > 0;
        
    } catch (const std::exception& e) {
        throw QueryException(std::string("Failed to remove review: ") + e.what());
    }
}

bool PostgreSQLReviewRepository::exists(const UUID& id) {
    try {
        auto work = dbConnection_->beginTransaction();
        
        SqlQueryBuilder queryBuilder;
        std::string query = queryBuilder
            .select({"1"})
            .from("reviews")
            .where("id = $1")
            .build();
            
        auto result = work.exec_params(query, id.toString());
        
        dbConnection_->commitTransaction(work);
        return !result.empty();
        
    } catch (const std::exception& e) {
        throw QueryException(std::string("Failed to check review existence: ") + e.what());
    }
}

Review PostgreSQLReviewRepository::mapResultToReview(const pqxx::row& row) const {
    UUID id = UUID::fromString(row["id"].c_str());
    UUID clientId = UUID::fromString(row["client_id"].c_str());
    UUID lessonId = UUID::fromString(row["lesson_id"].c_str());
    int rating = row["rating"].as<int>();
    std::string comment = row["comment"].c_str();
    ReviewStatus status = stringToReviewStatus(row["status"].c_str());
    
    // Создаем Review с помощью конструктора
    Review review(id, clientId, lessonId, rating, comment);
    
    // Восстанавливаем статус
    switch (status) {
        case ReviewStatus::APPROVED:
            review.approve();
            break;
        case ReviewStatus::REJECTED:
            review.reject();
            break;
        default:
            break; // PENDING_MODERATION по умолчанию
    }
    
    return review;
}

void PostgreSQLReviewRepository::validateReview(const Review& review) const {
    if (!review.isValid()) {
        throw DataAccessException("Invalid review data");
    }
}

std::string PostgreSQLReviewRepository::reviewStatusToString(ReviewStatus status) const {
    switch (status) {
        case ReviewStatus::PENDING_MODERATION: return "PENDING_MODERATION";
        case ReviewStatus::APPROVED: return "APPROVED";
        case ReviewStatus::REJECTED: return "REJECTED";
        default: return "PENDING_MODERATION";
    }
}

ReviewStatus PostgreSQLReviewRepository::stringToReviewStatus(const std::string& status) const {
    if (status == "APPROVED") return ReviewStatus::APPROVED;
    if (status == "REJECTED") return ReviewStatus::REJECTED;
    return ReviewStatus::PENDING_MODERATION;
}
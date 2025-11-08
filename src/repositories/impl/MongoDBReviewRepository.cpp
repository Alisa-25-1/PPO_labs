#include "MongoDBReviewRepository.hpp"
#include "../../data/MongoDBRepositoryFactory.hpp"
#include "../../data/DateTimeUtils.hpp"
#include <bsoncxx/builder/basic/document.hpp>
#include <bsoncxx/builder/basic/array.hpp>
#include <bsoncxx/builder/basic/kvp.hpp>
#include <iostream>

MongoDBReviewRepository::MongoDBReviewRepository(std::shared_ptr<MongoDBRepositoryFactory> factory)
    : factory_(std::move(factory)) {}

mongocxx::collection MongoDBReviewRepository::getCollection() const {
    return factory_->getDatabase().collection("reviews");
}

std::optional<Review> MongoDBReviewRepository::findById(const UUID& id) {
    try {
        auto collection = getCollection();
        auto filter = bsoncxx::builder::stream::document{}
            << "id" << id.toString()
            << bsoncxx::builder::stream::finalize;
        
        auto result = collection.find_one(filter.view());
        
        if (!result) {
            std::cout << "❌ Отзыв не найден в MongoDB: " << id.toString() << std::endl;
            return std::nullopt;
        }
        
        std::cout << "✅ Отзыв найден в MongoDB: " << id.toString() << std::endl;
        return mapDocumentToReview(result->view());
        
    } catch (const std::exception& e) {
        std::cerr << "❌ MongoDB Error in findById: " << e.what() << std::endl;
        throw DataAccessException(std::string("Failed to find review by ID: ") + e.what());
    }
}

std::vector<Review> MongoDBReviewRepository::findByClientId(const UUID& clientId) {
    std::vector<Review> reviews;
    
    try {
        std::cout << "🔍 Поиск отзывов клиента в MongoDB: " << clientId.toString() << std::endl;
        
        auto collection = getCollection();
        auto filter = bsoncxx::builder::stream::document{}
            << "clientId" << clientId.toString()
            << bsoncxx::builder::stream::finalize;
        
        auto cursor = collection.find(filter.view());
        
        int count = 0;
        for (auto&& doc : cursor) {
            try {
                auto review = mapDocumentToReview(doc);
                reviews.push_back(review);
                count++;
                std::cout << "✅ Найден отзыв клиента: " << review.getId().toString() << std::endl;
            } catch (const std::exception& e) {
                std::cerr << "❌ Ошибка создания отзыва из MongoDB документа: " << e.what() << std::endl;
                continue;
            }
        }
        
        std::cout << "📊 Найдено отзывов клиента в MongoDB: " << count << std::endl;
        return reviews;
        
    } catch (const std::exception& e) {
        std::cerr << "❌ MongoDB Error in findByClientId: " << e.what() << std::endl;
        throw DataAccessException(std::string("Failed to find reviews by client ID: ") + e.what());
    }
}

std::vector<Review> MongoDBReviewRepository::findByLessonId(const UUID& lessonId) {
    std::vector<Review> reviews;
    
    try {
        std::cout << "🔍 Поиск отзывов на занятие в MongoDB: " << lessonId.toString() << std::endl;
        
        auto collection = getCollection();
        auto filter = bsoncxx::builder::stream::document{}
            << "lessonId" << lessonId.toString()
            << bsoncxx::builder::stream::finalize;
        
        auto cursor = collection.find(filter.view());
        
        int count = 0;
        for (auto&& doc : cursor) {
            try {
                auto review = mapDocumentToReview(doc);
                reviews.push_back(review);
                count++;
                std::cout << "✅ Найден отзыв на занятие: " << review.getId().toString() << std::endl;
            } catch (const std::exception& e) {
                std::cerr << "❌ Ошибка создания отзыва из MongoDB документа: " << e.what() << std::endl;
                continue;
            }
        }
        
        std::cout << "📊 Найдено отзывов на занятие в MongoDB: " << count << std::endl;
        return reviews;
        
    } catch (const std::exception& e) {
        std::cerr << "❌ MongoDB Error in findByLessonId: " << e.what() << std::endl;
        throw DataAccessException(std::string("Failed to find reviews by lesson ID: ") + e.what());
    }
}

std::optional<Review> MongoDBReviewRepository::findByClientAndLesson(
    const UUID& clientId, const UUID& lessonId) {
    
    try {
        auto collection = getCollection();
        auto filter = bsoncxx::builder::stream::document{}
            << "clientId" << clientId.toString()
            << "lessonId" << lessonId.toString()
            << bsoncxx::builder::stream::finalize;
        
        auto result = collection.find_one(filter.view());
        
        if (!result) {
            std::cout << "❌ Отзыв клиента на занятие не найден в MongoDB" << std::endl;
            return std::nullopt;
        }
        
        std::cout << "✅ Отзыв клиента на занятие найдена в MongoDB" << std::endl;
        return mapDocumentToReview(result->view());
        
    } catch (const std::exception& e) {
        std::cerr << "❌ MongoDB Error in findByClientAndLesson: " << e.what() << std::endl;
        throw DataAccessException(std::string("Failed to find review by client and lesson: ") + e.what());
    }
}

std::vector<Review> MongoDBReviewRepository::findPendingModeration() {
    std::vector<Review> reviews;
    
    try {
        std::cout << "🔍 Поиск отзывов на модерации в MongoDB" << std::endl;
        
        auto collection = getCollection();
        auto filter = bsoncxx::builder::stream::document{}
            << "status" << "PENDING_MODERATION"
            << bsoncxx::builder::stream::finalize;
        
        auto cursor = collection.find(filter.view());
        
        int count = 0;
        for (auto&& doc : cursor) {
            try {
                auto review = mapDocumentToReview(doc);
                reviews.push_back(review);
                count++;
            } catch (const std::exception& e) {
                std::cerr << "❌ Ошибка при маппинге отзыва из MongoDB: " << e.what() << std::endl;
                continue;
            }
        }
        
        std::cout << "✅ Успешно загружено отзывов на модерации из MongoDB: " << count << std::endl;
        return reviews;
        
    } catch (const std::exception& e) {
        std::cerr << "❌ MongoDB Error in findPendingModeration: " << e.what() << std::endl;
        throw DataAccessException(std::string("Failed to find pending reviews: ") + e.what());
    }
}

std::vector<Review> MongoDBReviewRepository::findAll() {
    std::vector<Review> reviews;
    
    try {
        std::cout << "🔍 Получение всех отзывов из MongoDB" << std::endl;
        
        auto collection = getCollection();
        auto cursor = collection.find({});
        
        int count = 0;
        for (auto&& doc : cursor) {
            try {
                auto review = mapDocumentToReview(doc);
                reviews.push_back(review);
                count++;
            } catch (const std::exception& e) {
                std::cerr << "❌ Ошибка при маппинге отзыва из MongoDB: " << e.what() << std::endl;
                continue;
            }
        }
        
        std::cout << "✅ Успешно загружено отзывов из MongoDB: " << count << std::endl;
        return reviews;
        
    } catch (const std::exception& e) {
        std::cerr << "❌ MongoDB Error in findAll: " << e.what() << std::endl;
        throw DataAccessException(std::string("Failed to find all reviews: ") + e.what());
    }
}

double MongoDBReviewRepository::getAverageRatingForTrainer(const UUID& trainerId) {
    try {
        auto collection = getCollection();
        
        // Для MongoDB нам нужно агрегировать отзывы через занятия тренера
        // Сначала получим все занятия тренера, затем отзывы к этим занятиям
        auto pipeline = bsoncxx::builder::basic::array{};
        
        // Этап 1: Получаем все занятия тренера (нужен доступ к коллекции lessons)
        // Для простоты сделаем два запроса
        auto lessonsCollection = factory_->getDatabase().collection("lessons");
        auto lessonsFilter = bsoncxx::builder::stream::document{}
            << "trainerId" << trainerId.toString()
            << bsoncxx::builder::stream::finalize;
        
        std::vector<UUID> lessonIds;
        auto lessonsCursor = lessonsCollection.find(lessonsFilter.view());
        for (auto&& lessonDoc : lessonsCursor) {
            try {
                UUID lessonId = UUID::fromString(lessonDoc["id"].get_string().value.to_string());
                lessonIds.push_back(lessonId);
            } catch (const std::exception& e) {
                std::cerr << "❌ Ошибка получения ID занятия: " << e.what() << std::endl;
                continue;
            }
        }
        
        if (lessonIds.empty()) {
            return 0.0;
        }
        
        // Этап 2: Получаем средний рейтинг по всем отзывам этих занятий
        auto reviewFilter = bsoncxx::builder::stream::document{}
            << "lessonId" << bsoncxx::builder::stream::open_document
                << "$in" << [&] {
                    auto array_builder = bsoncxx::builder::basic::array{};
                    for (const auto& lessonId : lessonIds) {
                        array_builder.append(lessonId.toString());
                    }
                    return array_builder;
                }()
            << bsoncxx::builder::stream::close_document
            << "status" << "APPROVED"  // Только одобренные отзывы
            << bsoncxx::builder::stream::finalize;
        
        auto reviewsCursor = collection.find(reviewFilter.view());
        
        double totalRating = 0.0;
        int reviewCount = 0;
        
        for (auto&& reviewDoc : reviewsCursor) {
            try {
                int rating = reviewDoc["rating"].get_int32();
                totalRating += rating;
                reviewCount++;
            } catch (const std::exception& e) {
                std::cerr << "❌ Ошибка получения рейтинга отзыва: " << e.what() << std::endl;
                continue;
            }
        }
        
        if (reviewCount == 0) {
            return 0.0;
        }
        
        double averageRating = totalRating / reviewCount;
        std::cout << "📊 Средний рейтинг тренера в MongoDB: " << averageRating 
                  << " (на основе " << reviewCount << " отзывов)" << std::endl;
        
        return averageRating;
        
    } catch (const std::exception& e) {
        std::cerr << "❌ MongoDB Error in getAverageRatingForTrainer: " << e.what() << std::endl;
        throw DataAccessException(std::string("Failed to get average rating for trainer: ") + e.what());
    }
}

bool MongoDBReviewRepository::save(const Review& review) {
    validateReview(review);
    
    try {
        auto collection = getCollection();
        auto document = mapReviewToDocument(review);
        
        auto result = collection.insert_one(document.view());
        
        if (result && result->result().inserted_count() > 0) {
            std::cout << "✅ Отзыв успешно сохранен в MongoDB: " << review.getId().toString() << std::endl;
            return true;
        }
        
        std::cerr << "❌ Не удалось сохранить отзыв в MongoDB" << std::endl;
        return false;
        
    } catch (const std::exception& e) {
        std::cerr << "❌ MongoDB Error in save: " << e.what() << std::endl;
        throw DataAccessException(std::string("Failed to save review: ") + e.what());
    }
}

bool MongoDBReviewRepository::update(const Review& review) {
    validateReview(review);
    
    try {
        auto collection = getCollection();
        auto filter = bsoncxx::builder::stream::document{}
            << "id" << review.getId().toString()
            << bsoncxx::builder::stream::finalize;
        
        auto update_doc = bsoncxx::builder::stream::document{}
            << "$set" << bsoncxx::builder::stream::open_document
                << "clientId" << review.getClientId().toString()
                << "lessonId" << review.getLessonId().toString()
                << "rating" << review.getRating()
                << "comment" << review.getComment()
                << "publicationDate" << DateTimeUtils::formatTimeForMongoDB(review.getPublicationDate())
                << "status" << reviewStatusToString(review.getStatus())
            << bsoncxx::builder::stream::close_document
            << bsoncxx::builder::stream::finalize;
        
        auto result = collection.update_one(filter.view(), update_doc.view());
        
        if (result && result->modified_count() > 0) {
            std::cout << "✅ Отзыв успешно обновлен в MongoDB: " << review.getId().toString() << std::endl;
            return true;
        }
        
        std::cout << "⚠️  Отзыв не найден для обновления в MongoDB: " << review.getId().toString() << std::endl;
        return false;
        
    } catch (const std::exception& e) {
        std::cerr << "❌ MongoDB Error in update: " << e.what() << std::endl;
        throw DataAccessException(std::string("Failed to update review: ") + e.what());
    }
}

bool MongoDBReviewRepository::remove(const UUID& id) {
    try {
        auto collection = getCollection();
        auto filter = bsoncxx::builder::stream::document{}
            << "id" << id.toString()
            << bsoncxx::builder::stream::finalize;
        
        auto result = collection.delete_one(filter.view());
        
        if (result && result->deleted_count() > 0) {
            std::cout << "✅ Отзыв успешно удален из MongoDB: " << id.toString() << std::endl;
            return true;
        }
        
        std::cout << "⚠️  Отзыв не найден для удаления в MongoDB: " << id.toString() << std::endl;
        return false;
        
    } catch (const std::exception& e) {
        std::cerr << "❌ MongoDB Error in remove: " << e.what() << std::endl;
        throw DataAccessException(std::string("Failed to remove review: ") + e.what());
    }
}

bool MongoDBReviewRepository::exists(const UUID& id) {
    try {
        auto collection = getCollection();
        auto filter = bsoncxx::builder::stream::document{}
            << "id" << id.toString()
            << bsoncxx::builder::stream::finalize;
        
        auto result = collection.count_documents(filter.view());
        return result > 0;
        
    } catch (const std::exception& e) {
        std::cerr << "❌ MongoDB Error in exists: " << e.what() << std::endl;
        throw DataAccessException(std::string("Failed to check review existence: ") + e.what());
    }
}

Review MongoDBReviewRepository::mapDocumentToReview(const bsoncxx::document::view& doc) const {
    try {
        UUID id = UUID::fromString(doc["id"].get_string().value.to_string());
        UUID clientId = UUID::fromString(doc["clientId"].get_string().value.to_string());
        UUID lessonId = UUID::fromString(doc["lessonId"].get_string().value.to_string());
        int rating = doc["rating"].get_int32();
        std::string comment = doc["comment"].get_string().value.to_string();
        ReviewStatus status = stringToReviewStatus(doc["status"].get_string().value.to_string());
        
        // Создаем отзыв
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
        
        // Проверяем валидность
        if (!review.isValid()) {
            std::string error = "Invalid review data from MongoDB: ";
            error += "id=" + id.toString();
            error += ", clientId=" + clientId.toString();
            error += ", lessonId=" + lessonId.toString();
            error += ", rating=" + std::to_string(rating);
            error += ", status=" + reviewStatusToString(status);
            throw std::invalid_argument(error);
        }
        
        return review;
        
    } catch (const std::exception& e) {
        std::cerr << "❌ Критическая ошибка маппинга Review из MongoDB: " << e.what() << std::endl;
        throw DataAccessException("Failed to map MongoDB document to Review");
    }
}

bsoncxx::document::value MongoDBReviewRepository::mapReviewToDocument(const Review& review) const {
    return bsoncxx::builder::stream::document{}
        << "id" << review.getId().toString()
        << "clientId" << review.getClientId().toString()
        << "lessonId" << review.getLessonId().toString()
        << "rating" << review.getRating()
        << "comment" << review.getComment()
        << "publicationDate" << DateTimeUtils::formatTimeForMongoDB(review.getPublicationDate())
        << "status" << reviewStatusToString(review.getStatus())
        << bsoncxx::builder::stream::finalize;
}

void MongoDBReviewRepository::validateReview(const Review& review) const {
    if (!review.isValid()) {
        throw DataAccessException("Invalid review data for MongoDB");
    }
}

std::string MongoDBReviewRepository::reviewStatusToString(ReviewStatus status) const {
    switch (status) {
        case ReviewStatus::PENDING_MODERATION: return "PENDING_MODERATION";
        case ReviewStatus::APPROVED: return "APPROVED";
        case ReviewStatus::REJECTED: return "REJECTED";
        default: return "PENDING_MODERATION";
    }
}

ReviewStatus MongoDBReviewRepository::stringToReviewStatus(const std::string& status) const {
    if (status == "APPROVED") return ReviewStatus::APPROVED;
    if (status == "REJECTED") return ReviewStatus::REJECTED;
    return ReviewStatus::PENDING_MODERATION;
}
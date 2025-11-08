#ifndef MONGODB_LESSON_REPOSITORY_HPP
#define MONGODB_LESSON_REPOSITORY_HPP

#include "../ILessonRepository.hpp"
#include "../../data/exceptions/DataAccessException.hpp"
#include <mongocxx/client.hpp>
#include <mongocxx/collection.hpp>
#include <bsoncxx/builder/basic/document.hpp>
#include <bsoncxx/json.hpp>
#include <iostream>

// Предварительное объявление
class MongoDBRepositoryFactory;

class MongoDBLessonRepository : public ILessonRepository {
private:
    std::shared_ptr<MongoDBRepositoryFactory> factory_;
    mongocxx::collection getCollection() const;

public:
    explicit MongoDBLessonRepository(std::shared_ptr<MongoDBRepositoryFactory> factory);
    
    std::optional<Lesson> findById(const UUID& id) override;
    std::vector<Lesson> findByTrainerId(const UUID& trainerId) override;
    std::vector<Lesson> findByHallId(const UUID& hallId) override;
    std::vector<Lesson> findConflictingLessons(const UUID& hallId, const TimeSlot& timeSlot) override;
    std::vector<Lesson> findUpcomingLessons(int days = 7) override;
    std::vector<Lesson> findAll() override;
    bool save(const Lesson& lesson) override;
    bool update(const Lesson& lesson) override;
    bool remove(const UUID& id) override;
    bool exists(const UUID& id) override;

private:
    Lesson mapDocumentToLesson(const bsoncxx::document::view& doc) const;
    bsoncxx::document::value mapLessonToDocument(const Lesson& lesson) const;
    void validateLesson(const Lesson& lesson) const;
    
    // Вспомогательные методы для преобразования enum
    std::string lessonTypeToString(LessonType type) const;
    LessonType stringToLessonType(const std::string& type) const;
    std::string difficultyLevelToString(DifficultyLevel level) const;
    DifficultyLevel stringToDifficultyLevel(const std::string& level) const;
    std::string lessonStatusToString(LessonStatus status) const;
    LessonStatus stringToLessonStatus(const std::string& status) const;
};

#endif // MONGODB_LESSON_REPOSITORY_HPP
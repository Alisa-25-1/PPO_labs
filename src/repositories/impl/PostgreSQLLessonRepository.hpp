#ifndef POSTGRESQL_LESSON_REPOSITORY_HPP
#define POSTGRESQL_LESSON_REPOSITORY_HPP

#include "../ILessonRepository.hpp"
#include "../../data/DatabaseConnection.hpp"
#include "../../data/exceptions/DataAccessException.hpp"
#include <memory>

class PostgreSQLLessonRepository : public ILessonRepository {
public:
    explicit PostgreSQLLessonRepository(std::shared_ptr<DatabaseConnection> dbConnection);
    
    std::optional<Lesson> findById(const UUID& id) override;
    std::vector<Lesson> findByTrainerId(const UUID& trainerId) override;
    std::vector<Lesson> findByHallId(const UUID& hallId) override;
    std::vector<Lesson> findConflictingLessons(const UUID& hallId, const TimeSlot& timeSlot) override;
    std::vector<Lesson> findUpcomingLessons(int days = 7) override;
    bool save(const Lesson& lesson) override;
    bool update(const Lesson& lesson) override;
    bool remove(const UUID& id) override;
    bool exists(const UUID& id) override;

private:
    std::shared_ptr<DatabaseConnection> dbConnection_;
    
    Lesson mapResultToLesson(const pqxx::row& row) const;
    void validateLesson(const Lesson& lesson) const;
    std::string lessonTypeToString(LessonType type) const;
    LessonType stringToLessonType(const std::string& type) const;
    std::string difficultyLevelToString(DifficultyLevel level) const;
    DifficultyLevel stringToDifficultyLevel(const std::string& level) const;
    std::string lessonStatusToString(LessonStatus status) const;
    LessonStatus stringToLessonStatus(const std::string& status) const;
};

#endif // POSTGRESQL_LESSON_REPOSITORY_HPP
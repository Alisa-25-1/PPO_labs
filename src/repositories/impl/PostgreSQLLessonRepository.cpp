#include "PostgreSQLLessonRepository.hpp"
#include <pqxx/pqxx>
#include "../../data/DateTimeUtils.hpp"
#include "../../data/QueryFactory.hpp"

PostgreSQLLessonRepository::PostgreSQLLessonRepository(
    std::shared_ptr<DatabaseConnection> dbConnection)
    : dbConnection_(std::move(dbConnection)) {}

std::optional<Lesson> PostgreSQLLessonRepository::findById(const UUID& id) {
    try {
        auto work = dbConnection_->beginTransaction();
        
        SqlQueryBuilder queryBuilder;
        std::string query = queryBuilder
            .select({
                "id", "type", "name", "description", "start_time", "duration_minutes",
                "difficulty", "max_participants", "current_participants", "price", "status",
                "trainer_id", "hall_id"
            })
            .from("lessons")
            .where("id = $1")
            .build();
        
        auto result = work.exec_params(query, id.toString());
        
        if (result.empty()) {
            return std::nullopt;
        }
        
        auto lesson = mapResultToLesson(result[0]);
        dbConnection_->commitTransaction(work);
        return lesson;
        
    } catch (const std::exception& e) {
        throw QueryException(std::string("Failed to find lesson by ID: ") + e.what());
    }
}

std::vector<Lesson> PostgreSQLLessonRepository::findByTrainerId(const UUID& trainerId) {
    try {
        auto work = dbConnection_->beginTransaction();
        
        SqlQueryBuilder queryBuilder;
        std::string query = queryBuilder
            .select({
                "id", "type", "name", "description", "start_time", "duration_minutes",
                "difficulty", "max_participants", "current_participants", "price", "status",
                "trainer_id", "hall_id"
            })
            .from("lessons")
            .where("trainer_id = $1")
            .build();
        
        auto result = work.exec_params(query, trainerId.toString());
        
        std::vector<Lesson> lessons;
        for (const auto& row : result) {
            lessons.push_back(mapResultToLesson(row));
        }
        
        dbConnection_->commitTransaction(work);
        return lessons;
        
    } catch (const std::exception& e) {
        throw QueryException(std::string("Failed to find lessons by trainer ID: ") + e.what());
    }
}

std::vector<Lesson> PostgreSQLLessonRepository::findByHallId(const UUID& hallId) {
    try {
        auto work = dbConnection_->beginTransaction();
        
        SqlQueryBuilder queryBuilder;
        std::string query = queryBuilder
            .select({
                "id", "type", "name", "description", "start_time", "duration_minutes",
                "difficulty", "max_participants", "current_participants", "price", "status",
                "trainer_id", "hall_id"
            })
            .from("lessons")
            .where("hall_id = $1")
            .build();
        
        auto result = work.exec_params(query, hallId.toString());
        
        std::vector<Lesson> lessons;
        for (const auto& row : result) {
            lessons.push_back(mapResultToLesson(row));
        }
        
        dbConnection_->commitTransaction(work);
        return lessons;
        
    } catch (const std::exception& e) {
        throw QueryException(std::string("Failed to find lessons by hall ID: ") + e.what());
    }
}

std::vector<Lesson> PostgreSQLLessonRepository::findConflictingLessons(
    const UUID& hallId, const TimeSlot& timeSlot) {
    
    try {
        auto work = dbConnection_->beginTransaction();
        
        std::string query = QueryFactory::createFindConflictingLessonsQuery();
        auto startTimeStr = DateTimeUtils::formatTimeForPostgres(timeSlot.getStartTime());
        auto result = work.exec_params(
            query, 
            hallId.toString(),
            startTimeStr,
            timeSlot.getDurationMinutes()
        );
        
        std::vector<Lesson> lessons;
        for (const auto& row : result) {
            lessons.push_back(mapResultToLesson(row));
        }

        dbConnection_->commitTransaction(work);
        return lessons;
        
    } catch (const std::exception& e) {
        throw QueryException(std::string("Failed to find conflicting lessons: ") + e.what());
    }
}

std::vector<Lesson> PostgreSQLLessonRepository::findUpcomingLessons(int days) {
    try {
        auto work = dbConnection_->beginTransaction();
        
        std::string query = QueryFactory::createFindUpcomingLessonsQuery();
        auto result = work.exec_params(query, days);
        
        std::vector<Lesson> lessons;
        for (const auto& row : result) {
            lessons.push_back(mapResultToLesson(row));
        }
        
        dbConnection_->commitTransaction(work);
        return lessons;
        
    } catch (const std::exception& e) {
        throw QueryException(std::string("Failed to find upcoming lessons: ") + e.what());
    }
}

bool PostgreSQLLessonRepository::save(const Lesson& lesson) {
    validateLesson(lesson);
    
    try {
        auto work = dbConnection_->beginTransaction();
        
        std::map<std::string, std::string> values = {
            {"id", "$1"},
            {"type", "$2"},
            {"name", "$3"},
            {"description", "$4"},
            {"start_time", "$5"},
            {"duration_minutes", "$6"},
            {"difficulty", "$7"},
            {"max_participants", "$8"},
            {"current_participants", "$9"},
            {"price", "$10"},
            {"status", "$11"},
            {"trainer_id", "$12"},
            {"hall_id", "$13"}
        };
        
        SqlQueryBuilder queryBuilder;
        std::string query = queryBuilder
            .insertInto("lessons")
            .values(values)
            .build();
        
        work.exec_params(
            query,
            lesson.getId().toString(),
            lessonTypeToString(lesson.getType()),
            lesson.getName(),
            lesson.getDescription(),
            DateTimeUtils::formatTimeForPostgres(lesson.getStartTime()),
            lesson.getDurationMinutes(),
            difficultyLevelToString(lesson.getDifficulty()),
            lesson.getMaxParticipants(),
            lesson.getCurrentParticipants(),
            lesson.getPrice(),
            lessonStatusToString(lesson.getStatus()),
            lesson.getTrainerId().toString(),
            lesson.getHallId().toString()
        );
        
        dbConnection_->commitTransaction(work);
        return true;
        
    } catch (const std::exception& e) {
        throw QueryException(std::string("Failed to save lesson: ") + e.what());
    }
}

bool PostgreSQLLessonRepository::update(const Lesson& lesson) {
    validateLesson(lesson);
    
    try {
        auto work = dbConnection_->beginTransaction();
        
        std::map<std::string, std::string> values = {
            {"type", "$2"},
            {"name", "$3"},
            {"description", "$4"},
            {"start_time", "$5"},
            {"duration_minutes", "$6"},
            {"difficulty", "$7"},
            {"max_participants", "$8"},
            {"current_participants", "$9"},
            {"price", "$10"},
            {"status", "$11"},
            {"trainer_id", "$12"},
            {"hall_id", "$13"}
        };
        
        SqlQueryBuilder queryBuilder;
        std::string query = queryBuilder
            .update("lessons")
            .set(values)
            .where("id = $1")
            .build();
        
        auto result = work.exec_params(
            query,
            lesson.getId().toString(),
            lessonTypeToString(lesson.getType()),
            lesson.getName(),
            lesson.getDescription(),
            DateTimeUtils::formatTimeForPostgres(lesson.getStartTime()),
            lesson.getDurationMinutes(),
            difficultyLevelToString(lesson.getDifficulty()),
            lesson.getMaxParticipants(),
            lesson.getCurrentParticipants(),
            lesson.getPrice(),
            lessonStatusToString(lesson.getStatus()),
            lesson.getTrainerId().toString(),
            lesson.getHallId().toString()
        );
        
        dbConnection_->commitTransaction(work);
        return result.affected_rows() > 0;
        
    } catch (const std::exception& e) {
        throw QueryException(std::string("Failed to update lesson: ") + e.what());
    }
}

bool PostgreSQLLessonRepository::remove(const UUID& id) {
    try {
        auto work = dbConnection_->beginTransaction();
        
        SqlQueryBuilder queryBuilder;
        std::string query = queryBuilder
            .deleteFrom("lessons")
            .where("id = $1")
            .build();
            
        auto result = work.exec_params(query, id.toString());
        
        dbConnection_->commitTransaction(work);
        return result.affected_rows() > 0;
        
    } catch (const std::exception& e) {
        throw QueryException(std::string("Failed to remove lesson: ") + e.what());
    }
}

bool PostgreSQLLessonRepository::exists(const UUID& id) {
    try {
        auto work = dbConnection_->beginTransaction();
        
        SqlQueryBuilder queryBuilder;
        std::string query = queryBuilder
            .select({"1"})
            .from("lessons")
            .where("id = $1")
            .build();
            
        auto result = work.exec_params(query, id.toString());
        
        dbConnection_->commitTransaction(work);
        return !result.empty();
        
    } catch (const std::exception& e) {
        throw QueryException(std::string("Failed to check lesson existence: ") + e.what());
    }
}

Lesson PostgreSQLLessonRepository::mapResultToLesson(const pqxx::row& row) const {
    UUID id = UUID::fromString(row["id"].c_str());
    
    LessonType type = stringToLessonType(row["type"].c_str());
    std::string name = row["name"].c_str();
    std::string description = row["description"].c_str();
    auto startTime = DateTimeUtils::parseTimeFromPostgres(row["start_time"].c_str());
    int durationMinutes = row["duration_minutes"].as<int>();
    DifficultyLevel difficulty = stringToDifficultyLevel(row["difficulty"].c_str());
    int maxParticipants = row["max_participants"].as<int>();
    int currentParticipants = row["current_participants"].as<int>();
    double price = row["price"].as<double>();
    LessonStatus status = stringToLessonStatus(row["status"].c_str());
    UUID trainerId = UUID::fromString(row["trainer_id"].c_str());
    UUID hallId = UUID::fromString(row["hall_id"].c_str());
    
    Lesson lesson(id, type, name, startTime, durationMinutes, difficulty, maxParticipants, price, trainerId, hallId);
    lesson.setDescription(description);
    lesson.setStatus(status);
    // Устанавливаем текущее количество участников
    for (int i = 0; i < currentParticipants; i++) {
        lesson.addParticipant();
    }
    
    return lesson;
}

void PostgreSQLLessonRepository::validateLesson(const Lesson& lesson) const {
    if (!lesson.isValid()) {
        throw DataAccessException("Invalid lesson data");
    }
}

// Вспомогательные функции для преобразования enum в строку и обратно
std::string PostgreSQLLessonRepository::lessonTypeToString(LessonType type) const {
    switch (type) {
        case LessonType::OPEN_CLASS: return "OPEN_CLASS";
        case LessonType::SPECIAL_COURSE: return "SPECIAL_COURSE";
        case LessonType::INDIVIDUAL: return "INDIVIDUAL";
        case LessonType::MASTERCLASS: return "MASTERCLASS";
        default: return "OPEN_CLASS";
    }
}

LessonType PostgreSQLLessonRepository::stringToLessonType(const std::string& type) const {
    if (type == "SPECIAL_COURSE") return LessonType::SPECIAL_COURSE;
    if (type == "INDIVIDUAL") return LessonType::INDIVIDUAL;
    if (type == "MASTERCLASS") return LessonType::MASTERCLASS;
    return LessonType::OPEN_CLASS;
}

std::string PostgreSQLLessonRepository::difficultyLevelToString(DifficultyLevel level) const {
    switch (level) {
        case DifficultyLevel::BEGINNER: return "BEGINNER";
        case DifficultyLevel::INTERMEDIATE: return "INTERMEDIATE";
        case DifficultyLevel::ADVANCED: return "ADVANCED";
        case DifficultyLevel::ALL_LEVELS: return "ALL_LEVELS";
        default: return "BEGINNER";
    }
}

DifficultyLevel PostgreSQLLessonRepository::stringToDifficultyLevel(const std::string& level) const {
    if (level == "INTERMEDIATE") return DifficultyLevel::INTERMEDIATE;
    if (level == "ADVANCED") return DifficultyLevel::ADVANCED;
    if (level == "ALL_LEVELS") return DifficultyLevel::ALL_LEVELS;
    return DifficultyLevel::BEGINNER;
}

std::string PostgreSQLLessonRepository::lessonStatusToString(LessonStatus status) const {
    switch (status) {
        case LessonStatus::SCHEDULED: return "SCHEDULED";
        case LessonStatus::ONGOING: return "ONGOING";
        case LessonStatus::COMPLETED: return "COMPLETED";
        case LessonStatus::CANCELLED: return "CANCELLED";
        default: return "SCHEDULED";
    }
}

LessonStatus PostgreSQLLessonRepository::stringToLessonStatus(const std::string& status) const {
    if (status == "ONGOING") return LessonStatus::ONGOING;
    if (status == "COMPLETED") return LessonStatus::COMPLETED;
    if (status == "CANCELLED") return LessonStatus::CANCELLED;
    return LessonStatus::SCHEDULED;
}
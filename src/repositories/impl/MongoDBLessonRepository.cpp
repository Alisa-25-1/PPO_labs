#include "MongoDBLessonRepository.hpp"
#include "../../data/MongoDBRepositoryFactory.hpp"
#include "../../data/DateTimeUtils.hpp"
#include <bsoncxx/builder/basic/document.hpp>
#include <bsoncxx/builder/basic/array.hpp>
#include <bsoncxx/builder/basic/kvp.hpp>
#include <iostream>

using bsoncxx::builder::basic::kvp;
using bsoncxx::builder::basic::make_document;
using bsoncxx::builder::basic::make_array;

MongoDBLessonRepository::MongoDBLessonRepository(std::shared_ptr<MongoDBRepositoryFactory> factory)
    : factory_(std::move(factory)) {}

mongocxx::collection MongoDBLessonRepository::getCollection() const {
    return factory_->getDatabase().collection("lessons");
}

std::optional<Lesson> MongoDBLessonRepository::findById(const UUID& id) {
    try {
        auto collection = getCollection();
        auto filter = make_document(kvp("id", id.toString()));
        
        auto result = collection.find_one(filter.view());
        
        if (!result) {
            std::cout << "❌ Урок не найден в MongoDB: " << id.toString() << std::endl;
            return std::nullopt;
        }
        
        std::cout << "✅ Урок найден в MongoDB: " << id.toString() << std::endl;
        return mapDocumentToLesson(result->view());
        
    } catch (const std::exception& e) {
        std::cerr << "❌ MongoDB Error in findById: " << e.what() << std::endl;
        throw DataAccessException(std::string("Failed to find lesson by ID: ") + e.what());
    }
}

std::vector<Lesson> MongoDBLessonRepository::findByTrainerId(const UUID& trainerId) {
    std::vector<Lesson> lessons;
    
    try {
        std::cout << "🔍 Поиск уроков тренера в MongoDB: " << trainerId.toString() << std::endl;
        
        auto collection = getCollection();
        auto filter = make_document(kvp("trainerId", trainerId.toString()));
        
        auto cursor = collection.find(filter.view());
        
        int count = 0;
        for (auto&& doc : cursor) {
            try {
                auto lesson = mapDocumentToLesson(doc);
                lessons.push_back(lesson);
                count++;
                std::cout << "✅ Найден урок тренера: " << lesson.getName() << std::endl;
            } catch (const std::exception& e) {
                std::cerr << "❌ Ошибка создания урока из MongoDB документа: " << e.what() << std::endl;
                continue;
            }
        }
        
        std::cout << "📊 Найдено уроков тренера в MongoDB: " << count << std::endl;
        return lessons;
        
    } catch (const std::exception& e) {
        std::cerr << "❌ MongoDB Error in findByTrainerId: " << e.what() << std::endl;
        throw DataAccessException(std::string("Failed to find lessons by trainer ID: ") + e.what());
    }
}

std::vector<Lesson> MongoDBLessonRepository::findByHallId(const UUID& hallId) {
    std::vector<Lesson> lessons;
    
    try {
        std::cout << "🔍 Поиск уроков в зале в MongoDB: " << hallId.toString() << std::endl;
        
        auto collection = getCollection();
        auto filter = make_document(kvp("hallId", hallId.toString()));
        
        auto cursor = collection.find(filter.view());
        
        int count = 0;
        for (auto&& doc : cursor) {
            try {
                auto lesson = mapDocumentToLesson(doc);
                lessons.push_back(lesson);
                count++;
                std::cout << "✅ Найден урок в зале: " << lesson.getName() << std::endl;
            } catch (const std::exception& e) {
                std::cerr << "❌ Ошибка создания урока из MongoDB документа: " << e.what() << std::endl;
                continue;
            }
        }
        
        std::cout << "📊 Найдено уроков в зале в MongoDB: " << count << std::endl;
        return lessons;
        
    } catch (const std::exception& e) {
        std::cerr << "❌ MongoDB Error in findByHallId: " << e.what() << std::endl;
        throw DataAccessException(std::string("Failed to find lessons by hall ID: ") + e.what());
    }
}

std::vector<Lesson> MongoDBLessonRepository::findConflictingLessons(const UUID& hallId, const TimeSlot& timeSlot) {
    std::vector<Lesson> lessons;
    
    try {
        std::cout << "🔍 Поиск конфликтующих уроков в MongoDB для зала: " << hallId.toString() << std::endl;
        
        auto collection = getCollection();
        
        auto startTime = timeSlot.getStartTime();
        auto endTime = timeSlot.getEndTime();
        
        // MongoDB query для поиска конфликтующих уроков
        auto filter = make_document(
            kvp("hallId", hallId.toString()),
            kvp("status", make_document(kvp("$in", make_array("SCHEDULED", "ONGOING")))),
            kvp("$or", make_array(
                make_document(
                    kvp("startTime", make_document(kvp("$lt", DateTimeUtils::formatTimeForMongoDB(endTime)))),
                    kvp("endTime", make_document(kvp("$gt", DateTimeUtils::formatTimeForMongoDB(startTime))))
                )
            ))
        );
        
        auto cursor = collection.find(filter.view());
        
        int count = 0;
        for (auto&& doc : cursor) {
            try {
                auto lesson = mapDocumentToLesson(doc);
                lessons.push_back(lesson);
                count++;
                std::cout << "⚠️  Найден конфликтующий урок: " << lesson.getName() << std::endl;
            } catch (const std::exception& e) {
                std::cerr << "❌ Ошибка создания конфликтующего урока из MongoDB: " << e.what() << std::endl;
                continue;
            }
        }
        
        std::cout << "📊 Найдено конфликтующих уроков в MongoDB: " << count << std::endl;
        return lessons;
        
    } catch (const std::exception& e) {
        std::cerr << "❌ MongoDB Error in findConflictingLessons: " << e.what() << std::endl;
        throw DataAccessException(std::string("Failed to find conflicting lessons: ") + e.what());
    }
}

std::vector<Lesson> MongoDBLessonRepository::findUpcomingLessons(int days) {
    std::vector<Lesson> lessons;
    
    try {
        std::cout << "🔍 Поиск предстоящих уроков в MongoDB (дней: " << days << ")" << std::endl;
        
        auto collection = getCollection();
        
        auto now = std::chrono::system_clock::now();
        auto futureDate = now + std::chrono::hours(24 * days);
        
        // Уроки, которые начнутся в течение указанного количества дней
        auto filter = make_document(
            kvp("startTime", make_document(
                kvp("$gte", DateTimeUtils::formatTimeForMongoDB(now)),
                kvp("$lte", DateTimeUtils::formatTimeForMongoDB(futureDate))
            )),
            kvp("status", make_document(kvp("$in", make_array("SCHEDULED", "ONGOING"))))
        );
        
        auto cursor = collection.find(filter.view());
        
        int count = 0;
        for (auto&& doc : cursor) {
            try {
                auto lesson = mapDocumentToLesson(doc);
                lessons.push_back(lesson);
                count++;
            } catch (const std::exception& e) {
                std::cerr << "❌ Ошибка при маппинге урока из MongoDB: " << e.what() << std::endl;
                continue;
            }
        }
        
        std::cout << "✅ Успешно загружено предстоящих уроков из MongoDB: " << count << std::endl;
        return lessons;
        
    } catch (const std::exception& e) {
        std::cerr << "❌ MongoDB Error in findUpcomingLessons: " << e.what() << std::endl;
        throw DataAccessException(std::string("Failed to find upcoming lessons: ") + e.what());
    }
}

std::vector<Lesson> MongoDBLessonRepository::findAll() {
    std::vector<Lesson> lessons;
    
    try {
        std::cout << "🔍 Получение всех уроков из MongoDB" << std::endl;
        
        auto collection = getCollection();
        auto cursor = collection.find({});
        
        int count = 0;
        for (auto&& doc : cursor) {
            try {
                auto lesson = mapDocumentToLesson(doc);
                lessons.push_back(lesson);
                count++;
            } catch (const std::exception& e) {
                std::cerr << "❌ Ошибка при маппинге урока из MongoDB: " << e.what() << std::endl;
                continue;
            }
        }
        
        std::cout << "✅ Успешно загружено уроков из MongoDB: " << count << std::endl;
        return lessons;
        
    } catch (const std::exception& e) {
        std::cerr << "❌ MongoDB Error in findAll: " << e.what() << std::endl;
        throw DataAccessException(std::string("Failed to find all lessons: ") + e.what());
    }
}

bool MongoDBLessonRepository::save(const Lesson& lesson) {
    validateLesson(lesson);
    
    try {
        auto collection = getCollection();
        auto document = mapLessonToDocument(lesson);
        
        auto result = collection.insert_one(document.view());
        
        if (result && result->result().inserted_count() > 0) {
            std::cout << "✅ Урок успешно сохранен в MongoDB: " << lesson.getId().toString() << std::endl;
            return true;
        }
        
        std::cerr << "❌ Не удалось сохранить урок в MongoDB" << std::endl;
        return false;
        
    } catch (const std::exception& e) {
        std::cerr << "❌ MongoDB Error in save: " << e.what() << std::endl;
        throw DataAccessException(std::string("Failed to save lesson: ") + e.what());
    }
}

bool MongoDBLessonRepository::update(const Lesson& lesson) {
    validateLesson(lesson);
    
    try {
        auto collection = getCollection();
        auto filter = make_document(kvp("id", lesson.getId().toString()));
        
        auto update_doc = make_document(
            kvp("$set", make_document(
                kvp("type", lessonTypeToString(lesson.getType())),
                kvp("name", lesson.getName()),
                kvp("description", lesson.getDescription()),
                kvp("startTime", DateTimeUtils::formatTimeForMongoDB(lesson.getStartTime())),
                kvp("endTime", DateTimeUtils::formatTimeForMongoDB(lesson.getTimeSlot().getEndTime())),
                kvp("durationMinutes", lesson.getDurationMinutes()),
                kvp("difficulty", difficultyLevelToString(lesson.getDifficulty())),
                kvp("maxParticipants", lesson.getMaxParticipants()),
                kvp("currentParticipants", lesson.getCurrentParticipants()),
                kvp("price", lesson.getPrice()),
                kvp("status", lessonStatusToString(lesson.getStatus())),
                kvp("trainerId", lesson.getTrainerId().toString()),
                kvp("hallId", lesson.getHallId().toString())
            ))
        );
        
        auto result = collection.update_one(filter.view(), update_doc.view());
        
        if (result && result->modified_count() > 0) {
            std::cout << "✅ Урок успешно обновлен в MongoDB: " << lesson.getId().toString() << std::endl;
            return true;
        }
        
        std::cout << "⚠️  Урок не найден для обновления в MongoDB: " << lesson.getId().toString() << std::endl;
        return false;
        
    } catch (const std::exception& e) {
        std::cerr << "❌ MongoDB Error in update: " << e.what() << std::endl;
        throw DataAccessException(std::string("Failed to update lesson: ") + e.what());
    }
}

bool MongoDBLessonRepository::remove(const UUID& id) {
    try {
        auto collection = getCollection();
        auto filter = make_document(kvp("id", id.toString()));
        
        auto result = collection.delete_one(filter.view());
        
        if (result && result->deleted_count() > 0) {
            std::cout << "✅ Урок успешно удален из MongoDB: " << id.toString() << std::endl;
            return true;
        }
        
        std::cout << "⚠️  Урок не найден для удаления в MongoDB: " << id.toString() << std::endl;
        return false;
        
    } catch (const std::exception& e) {
        std::cerr << "❌ MongoDB Error in remove: " << e.what() << std::endl;
        throw DataAccessException(std::string("Failed to remove lesson: ") + e.what());
    }
}

bool MongoDBLessonRepository::exists(const UUID& id) {
    try {
        auto collection = getCollection();
        auto filter = make_document(kvp("id", id.toString()));
        
        auto result = collection.count_documents(filter.view());
        return result > 0;
        
    } catch (const std::exception& e) {
        std::cerr << "❌ MongoDB Error in exists: " << e.what() << std::endl;
        throw DataAccessException(std::string("Failed to check lesson existence: ") + e.what());
    }
}

Lesson MongoDBLessonRepository::mapDocumentToLesson(const bsoncxx::document::view& doc) const {
    try {
        UUID id = UUID::fromString(doc["id"].get_string().value.to_string());
        
        LessonType type = stringToLessonType(doc["type"].get_string().value.to_string());
        std::string name = doc["name"].get_string().value.to_string();
        std::string description = doc["description"].get_string().value.to_string();
        auto startTime = DateTimeUtils::parseTimeFromMongoDB(doc["startTime"].get_string().value.to_string());
        int durationMinutes = doc["durationMinutes"].get_int32();
        DifficultyLevel difficulty = stringToDifficultyLevel(doc["difficulty"].get_string().value.to_string());
        int maxParticipants = doc["maxParticipants"].get_int32();
        int currentParticipants = doc["currentParticipants"].get_int32();
        double price = doc["price"].get_double().value;
        LessonStatus status = stringToLessonStatus(doc["status"].get_string().value.to_string());
        UUID trainerId = UUID::fromString(doc["trainerId"].get_string().value.to_string());
        UUID hallId = UUID::fromString(doc["hallId"].get_string().value.to_string());
        
        // Создаем урок
        Lesson lesson(id, type, name, startTime, durationMinutes, difficulty, maxParticipants, price, trainerId, hallId);
        lesson.setDescription(description);
        lesson.setStatus(status);
        
        // Устанавливаем текущее количество участников
        for (int i = lesson.getCurrentParticipants(); i < currentParticipants; i++) {
            lesson.addParticipant();
        }
        
        // Проверяем валидность
        if (!lesson.isValid()) {
            std::string error = "Invalid lesson data from MongoDB: ";
            error += "id=" + id.toString();
            error += ", name=" + name;
            error += ", durationMinutes=" + std::to_string(durationMinutes);
            error += ", maxParticipants=" + std::to_string(maxParticipants);
            error += ", trainerId=" + trainerId.toString();
            error += ", hallId=" + hallId.toString();
            throw std::invalid_argument(error);
        }
        
        return lesson;
        
    } catch (const std::exception& e) {
        std::cerr << "❌ Критическая ошибка маппинга Lesson из MongoDB: " << e.what() << std::endl;
        throw DataAccessException("Failed to map MongoDB document to Lesson");
    }
}

bsoncxx::document::value MongoDBLessonRepository::mapLessonToDocument(const Lesson& lesson) const {
    return make_document(
        kvp("id", lesson.getId().toString()),
        kvp("type", lessonTypeToString(lesson.getType())),
        kvp("name", lesson.getName()),
        kvp("description", lesson.getDescription()),
        kvp("startTime", DateTimeUtils::formatTimeForMongoDB(lesson.getStartTime())),
        kvp("endTime", DateTimeUtils::formatTimeForMongoDB(lesson.getTimeSlot().getEndTime())),
        kvp("durationMinutes", lesson.getDurationMinutes()),
        kvp("difficulty", difficultyLevelToString(lesson.getDifficulty())),
        kvp("maxParticipants", lesson.getMaxParticipants()),
        kvp("currentParticipants", lesson.getCurrentParticipants()),
        kvp("price", lesson.getPrice()),
        kvp("status", lessonStatusToString(lesson.getStatus())),
        kvp("trainerId", lesson.getTrainerId().toString()),
        kvp("hallId", lesson.getHallId().toString())
    );
}

void MongoDBLessonRepository::validateLesson(const Lesson& lesson) const {
    if (!lesson.isValid()) {
        throw DataAccessException("Invalid lesson data for MongoDB");
    }
}

// Вспомогательные функции для преобразования enum в строку и обратно
std::string MongoDBLessonRepository::lessonTypeToString(LessonType type) const {
    switch (type) {
        case LessonType::OPEN_CLASS: return "OPEN_CLASS";
        case LessonType::SPECIAL_COURSE: return "SPECIAL_COURSE";
        case LessonType::INDIVIDUAL: return "INDIVIDUAL";
        case LessonType::MASTERCLASS: return "MASTERCLASS";
        default: return "OPEN_CLASS";
    }
}

LessonType MongoDBLessonRepository::stringToLessonType(const std::string& type) const {
    if (type == "SPECIAL_COURSE") return LessonType::SPECIAL_COURSE;
    if (type == "INDIVIDUAL") return LessonType::INDIVIDUAL;
    if (type == "MASTERCLASS") return LessonType::MASTERCLASS;
    return LessonType::OPEN_CLASS;
}

std::string MongoDBLessonRepository::difficultyLevelToString(DifficultyLevel level) const {
    switch (level) {
        case DifficultyLevel::BEGINNER: return "BEGINNER";
        case DifficultyLevel::INTERMEDIATE: return "INTERMEDIATE";
        case DifficultyLevel::ADVANCED: return "ADVANCED";
        case DifficultyLevel::ALL_LEVELS: return "ALL_LEVELS";
        default: return "BEGINNER";
    }
}

DifficultyLevel MongoDBLessonRepository::stringToDifficultyLevel(const std::string& level) const {
    if (level == "INTERMEDIATE") return DifficultyLevel::INTERMEDIATE;
    if (level == "ADVANCED") return DifficultyLevel::ADVANCED;
    if (level == "ALL_LEVELS") return DifficultyLevel::ALL_LEVELS;
    return DifficultyLevel::BEGINNER;
}

std::string MongoDBLessonRepository::lessonStatusToString(LessonStatus status) const {
    switch (status) {
        case LessonStatus::SCHEDULED: return "SCHEDULED";
        case LessonStatus::ONGOING: return "ONGOING";
        case LessonStatus::COMPLETED: return "COMPLETED";
        case LessonStatus::CANCELLED: return "CANCELLED";
        default: return "SCHEDULED";
    }
}

LessonStatus MongoDBLessonRepository::stringToLessonStatus(const std::string& status) const {
    if (status == "ONGOING") return LessonStatus::ONGOING;
    if (status == "COMPLETED") return LessonStatus::COMPLETED;
    if (status == "CANCELLED") return LessonStatus::CANCELLED;
    return LessonStatus::SCHEDULED;
}
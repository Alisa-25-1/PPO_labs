#include "MongoDBEnrollmentRepository.hpp"
#include "../../data/MongoDBRepositoryFactory.hpp"
#include "../../data/DateTimeUtils.hpp"
#include <bsoncxx/builder/basic/document.hpp>
#include <bsoncxx/builder/basic/array.hpp>
#include <bsoncxx/builder/basic/kvp.hpp>
#include <iostream>

using bsoncxx::builder::basic::kvp;
using bsoncxx::builder::basic::make_document;
using bsoncxx::builder::basic::make_array;

MongoDBEnrollmentRepository::MongoDBEnrollmentRepository(std::shared_ptr<MongoDBRepositoryFactory> factory)
    : factory_(std::move(factory)) {}

mongocxx::collection MongoDBEnrollmentRepository::getCollection() const {
    return factory_->getDatabase().collection("enrollments");
}

std::optional<Enrollment> MongoDBEnrollmentRepository::findById(const UUID& id) {
    try {
        auto collection = getCollection();
        auto filter = make_document(kvp("id", id.toString()));
        
        auto result = collection.find_one(filter.view());
        
        if (!result) {
            std::cout << "❌ Запись на занятие не найдена в MongoDB: " << id.toString() << std::endl;
            return std::nullopt;
        }
        
        std::cout << "✅ Запись на занятие найдена в MongoDB: " << id.toString() << std::endl;
        return mapDocumentToEnrollment(result->view());
        
    } catch (const std::exception& e) {
        std::cerr << "❌ MongoDB Error in findById: " << e.what() << std::endl;
        throw DataAccessException(std::string("Failed to find enrollment by ID: ") + e.what());
    }
}

std::vector<Enrollment> MongoDBEnrollmentRepository::findByClientId(const UUID& clientId) {
    std::vector<Enrollment> enrollments;
    
    try {
        std::cout << "🔍 Поиск записей клиента в MongoDB: " << clientId.toString() << std::endl;
        
        auto collection = getCollection();
        auto filter = make_document(kvp("clientId", clientId.toString()));
        
        auto cursor = collection.find(filter.view());
        
        int count = 0;
        for (auto&& doc : cursor) {
            try {
                auto enrollment = mapDocumentToEnrollment(doc);
                enrollments.push_back(enrollment);
                count++;
                std::cout << "✅ Найдена запись клиента: " << enrollment.getId().toString() << std::endl;
            } catch (const std::exception& e) {
                std::cerr << "❌ Ошибка создания записи из MongoDB документа: " << e.what() << std::endl;
                continue;
            }
        }
        
        std::cout << "📊 Найдено записей клиента в MongoDB: " << count << std::endl;
        return enrollments;
        
    } catch (const std::exception& e) {
        std::cerr << "❌ MongoDB Error in findByClientId: " << e.what() << std::endl;
        throw DataAccessException(std::string("Failed to find enrollments by client ID: ") + e.what());
    }
}

std::vector<Enrollment> MongoDBEnrollmentRepository::findByLessonId(const UUID& lessonId) {
    std::vector<Enrollment> enrollments;
    
    try {
        std::cout << "🔍 Поиск записей на занятие в MongoDB: " << lessonId.toString() << std::endl;
        
        auto collection = getCollection();
        auto filter = make_document(kvp("lessonId", lessonId.toString()));
        
        auto cursor = collection.find(filter.view());
        
        int count = 0;
        for (auto&& doc : cursor) {
            try {
                auto enrollment = mapDocumentToEnrollment(doc);
                enrollments.push_back(enrollment);
                count++;
                std::cout << "✅ Найдена запись на занятие: " << enrollment.getId().toString() << std::endl;
            } catch (const std::exception& e) {
                std::cerr << "❌ Ошибка создания записи из MongoDB документа: " << e.what() << std::endl;
                continue;
            }
        }
        
        std::cout << "📊 Найдено записей на занятие в MongoDB: " << count << std::endl;
        return enrollments;
        
    } catch (const std::exception& e) {
        std::cerr << "❌ MongoDB Error in findByLessonId: " << e.what() << std::endl;
        throw DataAccessException(std::string("Failed to find enrollments by lesson ID: ") + e.what());
    }
}

std::optional<Enrollment> MongoDBEnrollmentRepository::findByClientAndLesson(
    const UUID& clientId, const UUID& lessonId) {
    
    try {
        auto collection = getCollection();
        auto filter = make_document(
            kvp("clientId", clientId.toString()),
            kvp("lessonId", lessonId.toString())
        );
        
        auto result = collection.find_one(filter.view());
        
        if (!result) {
            std::cout << "❌ Запись клиента на занятие не найдена в MongoDB" << std::endl;
            return std::nullopt;
        }
        
        std::cout << "✅ Запись клиента на занятие найдена в MongoDB" << std::endl;
        return mapDocumentToEnrollment(result->view());
        
    } catch (const std::exception& e) {
        std::cerr << "❌ MongoDB Error in findByClientAndLesson: " << e.what() << std::endl;
        throw DataAccessException(std::string("Failed to find enrollment by client and lesson: ") + e.what());
    }
}

int MongoDBEnrollmentRepository::countByLessonId(const UUID& lessonId) {
    try {
        auto collection = getCollection();
        auto filter = make_document(
            kvp("lessonId", lessonId.toString()),
            kvp("status", "REGISTERED")  // Считаем только активные записи
        );
        
        auto count = collection.count_documents(filter.view());
        
        std::cout << "📊 Количество записей на занятие в MongoDB: " << count << std::endl;
        return static_cast<int>(count);
        
    } catch (const std::exception& e) {
        std::cerr << "❌ MongoDB Error in countByLessonId: " << e.what() << std::endl;
        throw DataAccessException(std::string("Failed to count enrollments by lesson ID: ") + e.what());
    }
}

std::vector<Enrollment> MongoDBEnrollmentRepository::findAll() {
    std::vector<Enrollment> enrollments;
    
    try {
        std::cout << "🔍 Получение всех записей из MongoDB" << std::endl;
        
        auto collection = getCollection();
        auto cursor = collection.find({});
        
        int count = 0;
        for (auto&& doc : cursor) {
            try {
                auto enrollment = mapDocumentToEnrollment(doc);
                enrollments.push_back(enrollment);
                count++;
            } catch (const std::exception& e) {
                std::cerr << "❌ Ошибка при маппинге записи из MongoDB: " << e.what() << std::endl;
                continue;
            }
        }
        
        std::cout << "✅ Успешно загружено записей из MongoDB: " << count << std::endl;
        return enrollments;
        
    } catch (const std::exception& e) {
        std::cerr << "❌ MongoDB Error in findAll: " << e.what() << std::endl;
        throw DataAccessException(std::string("Failed to find all enrollments: ") + e.what());
    }
}

bool MongoDBEnrollmentRepository::save(const Enrollment& enrollment) {
    validateEnrollment(enrollment);
    
    try {
        auto collection = getCollection();
        auto document = mapEnrollmentToDocument(enrollment);
        
        auto result = collection.insert_one(document.view());
        
        if (result && result->result().inserted_count() > 0) {
            std::cout << "✅ Запись на занятие успешно сохранена в MongoDB: " << enrollment.getId().toString() << std::endl;
            return true;
        }
        
        std::cerr << "❌ Не удалось сохранить запись на занятие в MongoDB" << std::endl;
        return false;
        
    } catch (const std::exception& e) {
        std::cerr << "❌ MongoDB Error in save: " << e.what() << std::endl;
        throw DataAccessException(std::string("Failed to save enrollment: ") + e.what());
    }
}

bool MongoDBEnrollmentRepository::update(const Enrollment& enrollment) {
    validateEnrollment(enrollment);
    
    try {
        auto collection = getCollection();
        auto filter = make_document(kvp("id", enrollment.getId().toString()));
        
        auto update_doc = make_document(
            kvp("$set", make_document(
                kvp("clientId", enrollment.getClientId().toString()),
                kvp("lessonId", enrollment.getLessonId().toString()),
                kvp("status", enrollmentStatusToString(enrollment.getStatus())),
                kvp("enrollmentDate", DateTimeUtils::formatTimeForMongoDB(enrollment.getEnrollmentDate()))
            ))
        );
        
        auto result = collection.update_one(filter.view(), update_doc.view());
        
        if (result && result->modified_count() > 0) {
            std::cout << "✅ Запись на занятие успешно обновлена в MongoDB: " << enrollment.getId().toString() << std::endl;
            return true;
        }
        
        std::cout << "⚠️  Запись на занятие не найдена для обновления в MongoDB: " << enrollment.getId().toString() << std::endl;
        return false;
        
    } catch (const std::exception& e) {
        std::cerr << "❌ MongoDB Error in update: " << e.what() << std::endl;
        throw DataAccessException(std::string("Failed to update enrollment: ") + e.what());
    }
}

bool MongoDBEnrollmentRepository::remove(const UUID& id) {
    try {
        auto collection = getCollection();
        auto filter = make_document(kvp("id", id.toString()));
        
        auto result = collection.delete_one(filter.view());
        
        if (result && result->deleted_count() > 0) {
            std::cout << "✅ Запись на занятие успешно удалена из MongoDB: " << id.toString() << std::endl;
            return true;
        }
        
        std::cout << "⚠️  Запись на занятие не найдена для удаления в MongoDB: " << id.toString() << std::endl;
        return false;
        
    } catch (const std::exception& e) {
        std::cerr << "❌ MongoDB Error in remove: " << e.what() << std::endl;
        throw DataAccessException(std::string("Failed to remove enrollment: ") + e.what());
    }
}

bool MongoDBEnrollmentRepository::exists(const UUID& id) {
    try {
        auto collection = getCollection();
        auto filter = make_document(kvp("id", id.toString()));
        
        auto result = collection.count_documents(filter.view());
        return result > 0;
        
    } catch (const std::exception& e) {
        std::cerr << "❌ MongoDB Error in exists: " << e.what() << std::endl;
        throw DataAccessException(std::string("Failed to check enrollment existence: ") + e.what());
    }
}

Enrollment MongoDBEnrollmentRepository::mapDocumentToEnrollment(const bsoncxx::document::view& doc) const {
    try {
        UUID id = UUID::fromString(doc["id"].get_string().value.to_string());
        UUID clientId = UUID::fromString(doc["clientId"].get_string().value.to_string());
        UUID lessonId = UUID::fromString(doc["lessonId"].get_string().value.to_string());
        EnrollmentStatus status = stringToEnrollmentStatus(doc["status"].get_string().value.to_string());
        
        // Создаем запись
        Enrollment enrollment(id, clientId, lessonId);
        
        // Восстанавливаем статус
        switch (status) {
            case EnrollmentStatus::CANCELLED:
                enrollment.cancel();
                break;
            case EnrollmentStatus::ATTENDED:
                enrollment.markAttended();
                break;
            case EnrollmentStatus::MISSED:
                enrollment.markMissed();
                break;
            default:
                break; // REGISTERED по умолчанию
        }
        
        // Проверяем валидность
        if (!enrollment.isValid()) {
            std::string error = "Invalid enrollment data from MongoDB: ";
            error += "id=" + id.toString();
            error += ", clientId=" + clientId.toString();
            error += ", lessonId=" + lessonId.toString();
            error += ", status=" + enrollmentStatusToString(status);
            throw std::invalid_argument(error);
        }
        
        return enrollment;
        
    } catch (const std::exception& e) {
        std::cerr << "❌ Критическая ошибка маппинга Enrollment из MongoDB: " << e.what() << std::endl;
        throw DataAccessException("Failed to map MongoDB document to Enrollment");
    }
}

bsoncxx::document::value MongoDBEnrollmentRepository::mapEnrollmentToDocument(const Enrollment& enrollment) const {
    return make_document(
        kvp("id", enrollment.getId().toString()),
        kvp("clientId", enrollment.getClientId().toString()),
        kvp("lessonId", enrollment.getLessonId().toString()),
        kvp("status", enrollmentStatusToString(enrollment.getStatus())),
        kvp("enrollmentDate", DateTimeUtils::formatTimeForMongoDB(enrollment.getEnrollmentDate()))
    );
}

void MongoDBEnrollmentRepository::validateEnrollment(const Enrollment& enrollment) const {
    if (!enrollment.isValid()) {
        throw DataAccessException("Invalid enrollment data for MongoDB");
    }
}

std::string MongoDBEnrollmentRepository::enrollmentStatusToString(EnrollmentStatus status) const {
    switch (status) {
        case EnrollmentStatus::REGISTERED: return "REGISTERED";
        case EnrollmentStatus::CANCELLED: return "CANCELLED";
        case EnrollmentStatus::ATTENDED: return "ATTENDED";
        case EnrollmentStatus::MISSED: return "MISSED";
        default: return "REGISTERED";
    }
}

EnrollmentStatus MongoDBEnrollmentRepository::stringToEnrollmentStatus(const std::string& status) const {
    if (status == "CANCELLED") return EnrollmentStatus::CANCELLED;
    if (status == "ATTENDED") return EnrollmentStatus::ATTENDED;
    if (status == "MISSED") return EnrollmentStatus::MISSED;
    return EnrollmentStatus::REGISTERED;
}
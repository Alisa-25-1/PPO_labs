#include "MongoDBAttendanceRepository.hpp"
#include "../../data/MongoDBRepositoryFactory.hpp"
#include "../../data/DateTimeUtils.hpp"
#include <bsoncxx/builder/basic/document.hpp>
#include <bsoncxx/builder/basic/array.hpp>
#include <bsoncxx/builder/basic/kvp.hpp>
#include <mongocxx/pipeline.hpp>
#include <iostream>

MongoDBAttendanceRepository::MongoDBAttendanceRepository(std::shared_ptr<MongoDBRepositoryFactory> factory)
    : factory_(std::move(factory)) {}

mongocxx::collection MongoDBAttendanceRepository::getCollection() const {
    return factory_->getDatabase().collection("attendance");
}

std::optional<Attendance> MongoDBAttendanceRepository::findById(const UUID& id) {
    try {
        auto collection = getCollection();
        auto filter = bsoncxx::builder::stream::document{}
            << "id" << id.toString()
            << bsoncxx::builder::stream::finalize;
        
        auto result = collection.find_one(filter.view());
        
        if (!result) {
            std::cout << "❌ Запись посещаемости не найдена в MongoDB: " << id.toString() << std::endl;
            return std::nullopt;
        }
        
        std::cout << "✅ Запись посещаемости найдена в MongoDB: " << id.toString() << std::endl;
        return mapDocumentToAttendance(result->view());
        
    } catch (const std::exception& e) {
        std::cerr << "❌ MongoDB Error in findById: " << e.what() << std::endl;
        throw DataAccessException(std::string("Failed to find attendance by ID: ") + e.what());
    }
}

std::vector<Attendance> MongoDBAttendanceRepository::findByClientId(const UUID& clientId) {
    std::vector<Attendance> attendances;
    
    try {
        std::cout << "🔍 Поиск посещаемости клиента в MongoDB: " << clientId.toString() << std::endl;
        
        auto collection = getCollection();
        auto filter = bsoncxx::builder::stream::document{}
            << "clientId" << clientId.toString()
            << bsoncxx::builder::stream::finalize;
        
        auto cursor = collection.find(filter.view());
        
        int count = 0;
        for (auto&& doc : cursor) {
            try {
                auto attendance = mapDocumentToAttendance(doc);
                attendances.push_back(attendance);
                count++;
                std::cout << "✅ Найдена запись посещаемости клиента: " << attendance.getId().toString() << std::endl;
            } catch (const std::exception& e) {
                std::cerr << "❌ Ошибка создания записи посещаемости из MongoDB документа: " << e.what() << std::endl;
                continue;
            }
        }
        
        std::cout << "📊 Найдено записей посещаемости клиента в MongoDB: " << count << std::endl;
        return attendances;
        
    } catch (const std::exception& e) {
        std::cerr << "❌ MongoDB Error in findByClientId: " << e.what() << std::endl;
        throw DataAccessException(std::string("Failed to find attendances by client ID: ") + e.what());
    }
}

std::vector<Attendance> MongoDBAttendanceRepository::findByEntityId(const UUID& entityId) {
    std::vector<Attendance> attendances;
    
    try {
        std::cout << "🔍 Поиск посещаемости по entity в MongoDB: " << entityId.toString() << std::endl;
        
        auto collection = getCollection();
        auto filter = bsoncxx::builder::stream::document{}
            << "entityId" << entityId.toString()
            << bsoncxx::builder::stream::finalize;
        
        auto cursor = collection.find(filter.view());
        
        int count = 0;
        for (auto&& doc : cursor) {
            try {
                auto attendance = mapDocumentToAttendance(doc);
                attendances.push_back(attendance);
                count++;
                std::cout << "✅ Найдена запись посещаемости по entity: " << attendance.getId().toString() << std::endl;
            } catch (const std::exception& e) {
                std::cerr << "❌ Ошибка создания записи посещаемости из MongoDB документа: " << e.what() << std::endl;
                continue;
            }
        }
        
        std::cout << "📊 Найдено записей посещаемости по entity в MongoDB: " << count << std::endl;
        return attendances;
        
    } catch (const std::exception& e) {
        std::cerr << "❌ MongoDB Error in findByEntityId: " << e.what() << std::endl;
        throw DataAccessException(std::string("Failed to find attendances by entity ID: ") + e.what());
    }
}

std::vector<Attendance> MongoDBAttendanceRepository::findByClientAndPeriod(
    const UUID& clientId, 
    const std::chrono::system_clock::time_point& start, 
    const std::chrono::system_clock::time_point& end) {
    
    std::vector<Attendance> attendances;
    
    try {
        std::cout << "🔍 Поиск посещаемости клиента за период в MongoDB: " << clientId.toString() << std::endl;
        
        auto collection = getCollection();
        auto filter = bsoncxx::builder::stream::document{}
            << "clientId" << clientId.toString()
            << "scheduledTime" << bsoncxx::builder::stream::open_document
                << "$gte" << DateTimeUtils::formatTimeForMongoDB(start)
                << "$lte" << DateTimeUtils::formatTimeForMongoDB(end)
            << bsoncxx::builder::stream::close_document
            << bsoncxx::builder::stream::finalize;
        
        auto cursor = collection.find(filter.view());
        
        int count = 0;
        for (auto&& doc : cursor) {
            try {
                auto attendance = mapDocumentToAttendance(doc);
                attendances.push_back(attendance);
                count++;
            } catch (const std::exception& e) {
                std::cerr << "❌ Ошибка при маппинге посещаемости из MongoDB: " << e.what() << std::endl;
                continue;
            }
        }
        
        std::cout << "✅ Успешно загружено записей посещаемости за период из MongoDB: " << count << std::endl;
        return attendances;
        
    } catch (const std::exception& e) {
        std::cerr << "❌ MongoDB Error in findByClientAndPeriod: " << e.what() << std::endl;
        throw DataAccessException(std::string("Failed to find attendances by client and period: ") + e.what());
    }
}

std::vector<Attendance> MongoDBAttendanceRepository::findByTypeAndStatus(
    AttendanceType type, AttendanceStatus status) {
    
    std::vector<Attendance> attendances;
    
    try {
        std::cout << "🔍 Поиск посещаемости по типу и статусу в MongoDB" << std::endl;
        
        auto collection = getCollection();
        auto filter = bsoncxx::builder::stream::document{}
            << "type" << attendanceTypeToString(type)
            << "status" << attendanceStatusToString(status)
            << bsoncxx::builder::stream::finalize;
        
        auto cursor = collection.find(filter.view());
        
        int count = 0;
        for (auto&& doc : cursor) {
            try {
                auto attendance = mapDocumentToAttendance(doc);
                attendances.push_back(attendance);
                count++;
            } catch (const std::exception& e) {
                std::cerr << "❌ Ошибка при маппинге посещаемости из MongoDB: " << e.what() << std::endl;
                continue;
            }
        }
        
        std::cout << "✅ Успешно загружено записей посещаемости по типу и статусу из MongoDB: " << count << std::endl;
        return attendances;
        
    } catch (const std::exception& e) {
        std::cerr << "❌ MongoDB Error in findByTypeAndStatus: " << e.what() << std::endl;
        throw DataAccessException(std::string("Failed to find attendances by type and status: ") + e.what());
    }
}

std::vector<Attendance> MongoDBAttendanceRepository::findAll() {
    std::vector<Attendance> attendances;
    
    try {
        std::cout << "🔍 Получение всех записей посещаемости из MongoDB" << std::endl;
        
        auto collection = getCollection();
        auto cursor = collection.find({});
        
        int count = 0;
        for (auto&& doc : cursor) {
            try {
                auto attendance = mapDocumentToAttendance(doc);
                attendances.push_back(attendance);
                count++;
            } catch (const std::exception& e) {
                std::cerr << "❌ Ошибка при маппинге посещаемости из MongoDB: " << e.what() << std::endl;
                continue;
            }
        }
        
        std::cout << "✅ Успешно загружено записей посещаемости из MongoDB: " << count << std::endl;
        return attendances;
        
    } catch (const std::exception& e) {
        std::cerr << "❌ MongoDB Error in findAll: " << e.what() << std::endl;
        throw DataAccessException(std::string("Failed to find all attendance records: ") + e.what());
    }
}

bool MongoDBAttendanceRepository::save(const Attendance& attendance) {
    validateAttendance(attendance);
    
    try {
        auto collection = getCollection();
        auto document = mapAttendanceToDocument(attendance);
        
        auto result = collection.insert_one(document.view());
        
        if (result && result->result().inserted_count() > 0) {
            std::cout << "✅ Запись посещаемости успешно сохранена в MongoDB: " << attendance.getId().toString() << std::endl;
            return true;
        }
        
        std::cerr << "❌ Не удалось сохранить запись посещаемости в MongoDB" << std::endl;
        return false;
        
    } catch (const std::exception& e) {
        std::cerr << "❌ MongoDB Error in save: " << e.what() << std::endl;
        throw DataAccessException(std::string("Failed to save attendance: ") + e.what());
    }
}

bool MongoDBAttendanceRepository::update(const Attendance& attendance) {
    validateAttendance(attendance);
    
    try {
        auto collection = getCollection();
        auto filter = bsoncxx::builder::stream::document{}
            << "id" << attendance.getId().toString()
            << bsoncxx::builder::stream::finalize;
        
        auto update_doc = bsoncxx::builder::stream::document{}
            << "$set" << bsoncxx::builder::stream::open_document
                << "clientId" << attendance.getClientId().toString()
                << "entityId" << attendance.getEntityId().toString()
                << "type" << attendanceTypeToString(attendance.getType())
                << "status" << attendanceStatusToString(attendance.getStatus())
                << "scheduledTime" << DateTimeUtils::formatTimeForMongoDB(attendance.getScheduledTime())
                << "actualTime" << DateTimeUtils::formatTimeForMongoDB(attendance.getActualTime())
                << "notes" << attendance.getNotes()
                << "amountPaid" << attendance.getAmountPaid()
                << "durationMinutes" << attendance.getDurationMinutes()
            << bsoncxx::builder::stream::close_document
            << bsoncxx::builder::stream::finalize;
        
        auto result = collection.update_one(filter.view(), update_doc.view());
        
        if (result && result->modified_count() > 0) {
            std::cout << "✅ Запись посещаемости успешно обновлена в MongoDB: " << attendance.getId().toString() << std::endl;
            return true;
        }
        
        std::cout << "⚠️  Запись посещаемости не найдена для обновления в MongoDB: " << attendance.getId().toString() << std::endl;
        return false;
        
    } catch (const std::exception& e) {
        std::cerr << "❌ MongoDB Error in update: " << e.what() << std::endl;
        throw DataAccessException(std::string("Failed to update attendance: ") + e.what());
    }
}

bool MongoDBAttendanceRepository::remove(const UUID& id) {
    try {
        auto collection = getCollection();
        auto filter = bsoncxx::builder::stream::document{}
            << "id" << id.toString()
            << bsoncxx::builder::stream::finalize;
        
        auto result = collection.delete_one(filter.view());
        
        if (result && result->deleted_count() > 0) {
            std::cout << "✅ Запись посещаемости успешно удалена из MongoDB: " << id.toString() << std::endl;
            return true;
        }
        
        std::cout << "⚠️  Запись посещаемости не найдена для удаления в MongoDB: " << id.toString() << std::endl;
        return false;
        
    } catch (const std::exception& e) {
        std::cerr << "❌ MongoDB Error in remove: " << e.what() << std::endl;
        throw DataAccessException(std::string("Failed to remove attendance: ") + e.what());
    }
}

bool MongoDBAttendanceRepository::exists(const UUID& id) {
    try {
        auto collection = getCollection();
        auto filter = bsoncxx::builder::stream::document{}
            << "id" << id.toString()
            << bsoncxx::builder::stream::finalize;
        
        auto result = collection.count_documents(filter.view());
        return result > 0;
        
    } catch (const std::exception& e) {
        std::cerr << "❌ MongoDB Error in exists: " << e.what() << std::endl;
        throw DataAccessException(std::string("Failed to check attendance existence: ") + e.what());
    }
}

int MongoDBAttendanceRepository::countByClientAndStatus(const UUID& clientId, AttendanceStatus status) {
    try {
        auto collection = getCollection();
        auto filter = bsoncxx::builder::stream::document{}
            << "clientId" << clientId.toString()
            << "status" << attendanceStatusToString(status)
            << bsoncxx::builder::stream::finalize;
        
        auto count = collection.count_documents(filter.view());
        
        std::cout << "📊 Количество записей посещаемости клиента со статусом в MongoDB: " << count << std::endl;
        return static_cast<int>(count);
        
    } catch (const std::exception& e) {
        std::cerr << "❌ MongoDB Error in countByClientAndStatus: " << e.what() << std::endl;
        throw DataAccessException(std::string("Failed to count attendances by client and status: ") + e.what());
    }
}

int MongoDBAttendanceRepository::countByTypeAndStatus(AttendanceType type, AttendanceStatus status) {
    try {
        auto collection = getCollection();
        auto filter = bsoncxx::builder::stream::document{}
            << "type" << attendanceTypeToString(type)
            << "status" << attendanceStatusToString(status)
            << bsoncxx::builder::stream::finalize;
        
        auto count = collection.count_documents(filter.view());
        
        std::cout << "📊 Количество записей посещаемости по типу и статусу в MongoDB: " << count << std::endl;
        return static_cast<int>(count);
        
    } catch (const std::exception& e) {
        std::cerr << "❌ MongoDB Error in countByTypeAndStatus: " << e.what() << std::endl;
        throw DataAccessException(std::string("Failed to count attendances by type and status: ") + e.what());
    }
}

std::vector<std::pair<UUID, int>> MongoDBAttendanceRepository::getTopClientsByVisits(int limit) {
    std::vector<std::pair<UUID, int>> topClients;
    
    try {
        std::cout << "🔍 Получение топ клиентов по посещениям в MongoDB" << std::endl;
        
        auto collection = getCollection();
        
        // Создаем pipeline с использованием mongocxx::pipeline
        mongocxx::pipeline pipeline;
        
        // Этап 1: Фильтруем только посещенные записи
        pipeline.match(bsoncxx::builder::stream::document{}
            << "status" << "VISITED"
            << bsoncxx::builder::stream::finalize);
        
        // Этап 2: Группируем по clientId и считаем количество
        pipeline.group(bsoncxx::builder::stream::document{}
            << "_id" << "$clientId"
            << "visitCount" << bsoncxx::builder::stream::open_document
                << "$sum" << 1
            << bsoncxx::builder::stream::close_document
            << bsoncxx::builder::stream::finalize);
        
        // Этап 3: Сортируем по количеству посещений (по убыванию)
        pipeline.sort(bsoncxx::builder::stream::document{}
            << "visitCount" << -1
            << bsoncxx::builder::stream::finalize);
        
        // Этап 4: Ограничиваем количество результатов
        pipeline.limit(limit);
        
        // Выполняем агрегацию
        auto cursor = collection.aggregate(pipeline);
        
        for (auto&& doc : cursor) {
            try {
                UUID clientId = UUID::fromString(doc["_id"].get_string().value.to_string());
                int visitCount = doc["visitCount"].get_int32();
                topClients.emplace_back(clientId, visitCount);
                std::cout << "👤 Клиент " << clientId.toString() << " - " << visitCount << " посещений" << std::endl;
            } catch (const std::exception& e) {
                std::cerr << "❌ Ошибка обработки результата агрегации: " << e.what() << std::endl;
                continue;
            }
        }
        
        std::cout << "✅ Успешно получено топ клиентов: " << topClients.size() << std::endl;
        return topClients;
        
    } catch (const std::exception& e) {
        std::cerr << "❌ MongoDB Error in getTopClientsByVisits: " << e.what() << std::endl;
        throw DataAccessException(std::string("Failed to get top clients by visits: ") + e.what());
    }
}

Attendance MongoDBAttendanceRepository::mapDocumentToAttendance(const bsoncxx::document::view& doc) const {
    try {
        UUID id = UUID::fromString(doc["id"].get_string().value.to_string());
        UUID clientId = UUID::fromString(doc["clientId"].get_string().value.to_string());
        UUID entityId = UUID::fromString(doc["entityId"].get_string().value.to_string());
        
        AttendanceType type = stringToAttendanceType(doc["type"].get_string().value.to_string());
        AttendanceStatus status = stringToAttendanceStatus(doc["status"].get_string().value.to_string());
        
        auto scheduledTime = DateTimeUtils::parseTimeFromMongoDB(doc["scheduledTime"].get_string().value.to_string());
        auto actualTime = DateTimeUtils::parseTimeFromMongoDB(doc["actualTime"].get_string().value.to_string());
        
        std::string notes = doc["notes"].get_string().value.to_string();
        double amountPaid = doc["amountPaid"].get_double().value;
        int durationMinutes = doc["durationMinutes"].get_int32();
        
        // Создаем запись посещаемости
        Attendance attendance(id, clientId, entityId, type, scheduledTime);
        
        // Восстанавливаем статус
        switch (status) {
            case AttendanceStatus::VISITED:
                attendance.markVisited(notes);
                break;
            case AttendanceStatus::CANCELLED:
                attendance.markCancelled(notes);
                break;
            case AttendanceStatus::NO_SHOW:
                attendance.markNoShow(notes);
                break;
            default:
                // SCHEDULED - оставляем как есть
                break;
        }
        
        // Устанавливаем дополнительные поля
        attendance.setAmountPaid(amountPaid);
        attendance.setDurationMinutes(durationMinutes);
        
        // Проверяем валидность
        if (!attendance.isValid()) {
            std::string error = "Invalid attendance data from MongoDB: ";
            error += "id=" + id.toString();
            error += ", clientId=" + clientId.toString();
            error += ", entityId=" + entityId.toString();
            error += ", type=" + attendanceTypeToString(type);
            error += ", status=" + attendanceStatusToString(status);
            throw std::invalid_argument(error);
        }
        
        return attendance;
        
    } catch (const std::exception& e) {
        std::cerr << "❌ Критическая ошибка маппинга Attendance из MongoDB: " << e.what() << std::endl;
        throw DataAccessException("Failed to map MongoDB document to Attendance");
    }
}

bsoncxx::document::value MongoDBAttendanceRepository::mapAttendanceToDocument(const Attendance& attendance) const {
    return bsoncxx::builder::stream::document{}
        << "id" << attendance.getId().toString()
        << "clientId" << attendance.getClientId().toString()
        << "entityId" << attendance.getEntityId().toString()
        << "type" << attendanceTypeToString(attendance.getType())
        << "status" << attendanceStatusToString(attendance.getStatus())
        << "scheduledTime" << DateTimeUtils::formatTimeForMongoDB(attendance.getScheduledTime())
        << "actualTime" << DateTimeUtils::formatTimeForMongoDB(attendance.getActualTime())
        << "notes" << attendance.getNotes()
        << "amountPaid" << attendance.getAmountPaid()
        << "durationMinutes" << attendance.getDurationMinutes()
        << bsoncxx::builder::stream::finalize;
}

void MongoDBAttendanceRepository::validateAttendance(const Attendance& attendance) const {
    if (!attendance.isValid()) {
        throw DataAccessException("Invalid attendance data for MongoDB");
    }
}

std::string MongoDBAttendanceRepository::attendanceTypeToString(AttendanceType type) const {
    switch (type) {
        case AttendanceType::LESSON: return "LESSON";
        case AttendanceType::BOOKING: return "BOOKING";
        default: return "LESSON";
    }
}

AttendanceType MongoDBAttendanceRepository::stringToAttendanceType(const std::string& type) const {
    if (type == "BOOKING") return AttendanceType::BOOKING;
    return AttendanceType::LESSON;
}

std::string MongoDBAttendanceRepository::attendanceStatusToString(AttendanceStatus status) const {
    switch (status) {
        case AttendanceStatus::SCHEDULED: return "SCHEDULED";
        case AttendanceStatus::VISITED: return "VISITED";
        case AttendanceStatus::CANCELLED: return "CANCELLED";
        case AttendanceStatus::NO_SHOW: return "NO_SHOW";
        default: return "SCHEDULED";
    }
}

AttendanceStatus MongoDBAttendanceRepository::stringToAttendanceStatus(const std::string& status) const {
    if (status == "VISITED") return AttendanceStatus::VISITED;
    if (status == "CANCELLED") return AttendanceStatus::CANCELLED;
    if (status == "NO_SHOW") return AttendanceStatus::NO_SHOW;
    return AttendanceStatus::SCHEDULED;
}
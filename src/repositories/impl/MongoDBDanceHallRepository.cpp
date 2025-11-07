#include "MongoDBDanceHallRepository.hpp"
#include "../../data/DateTimeUtils.hpp"
#include "../../data/MongoDBRepositoryFactory.hpp"
#include <iostream>

MongoDBDanceHallRepository::MongoDBDanceHallRepository(std::shared_ptr<MongoDBRepositoryFactory> factory)
    : factory_(std::move(factory)) {}

mongocxx::collection MongoDBDanceHallRepository::getCollection() const {
    return factory_->getDatabase().collection("dance_halls");
}

std::optional<DanceHall> MongoDBDanceHallRepository::findById(const UUID& id) {
    try {
        auto collection = getCollection();
        auto filter = bsoncxx::builder::stream::document{}
            << "id" << id.toString()
            << bsoncxx::builder::stream::finalize;
        
        auto result = collection.find_one(filter.view());
        
        if (!result) {
            std::cout << "❌ Зал не найден в MongoDB: " << id.toString() << std::endl;
            return std::nullopt;
        }
        
        std::cout << "✅ Зал найден в MongoDB: " << id.toString() << std::endl;
        return mapDocumentToDanceHall(result->view());
        
    } catch (const std::exception& e) {
        std::cerr << "❌ MongoDB Error in findById: " << e.what() << std::endl;
        throw DataAccessException(std::string("Failed to find hall by ID: ") + e.what());
    }
}

std::vector<DanceHall> MongoDBDanceHallRepository::findByBranchId(const UUID& branchId) {
    std::vector<DanceHall> halls;
    
    try {
        std::cout << "🔍 Поиск залов для филиала в MongoDB: " << branchId.toString() << std::endl;
        
        auto collection = getCollection();
        auto filter = bsoncxx::builder::stream::document{}
            << "branchId" << branchId.toString()
            << bsoncxx::builder::stream::finalize;
        
        auto cursor = collection.find(filter.view());
        
        int count = 0;
        for (auto&& doc : cursor) {
            try {
                auto hall = mapDocumentToDanceHall(doc);
                halls.push_back(hall);
                count++;
                std::cout << "✅ Успешно создан зал из MongoDB: " << hall.getName() 
                          << " (ID: " << hall.getId().toString() << ")" << std::endl;
            } catch (const std::exception& e) {
                std::cerr << "❌ Ошибка создания зала из MongoDB документа: " << e.what() << std::endl;
                continue;
            }
        }
        
        std::cout << "📊 Найдено залов в MongoDB: " << count << std::endl;
        return halls;
        
    } catch (const std::exception& e) {
        std::cerr << "❌ MongoDB Error in findByBranchId: " << e.what() << std::endl;
        throw DataAccessException(std::string("Failed to find halls by branch ID: ") + e.what());
    }
}

bool MongoDBDanceHallRepository::exists(const UUID& id) {
    try {
        auto collection = getCollection();
        auto filter = bsoncxx::builder::stream::document{}
            << "id" << id.toString()
            << bsoncxx::builder::stream::finalize;
        
        auto result = collection.count_documents(filter.view());
        return result > 0;
        
    } catch (const std::exception& e) {
        std::cerr << "❌ MongoDB Error in exists: " << e.what() << std::endl;
        throw DataAccessException(std::string("Failed to check hall existence: ") + e.what());
    }
}

std::vector<DanceHall> MongoDBDanceHallRepository::findAll() {
    std::vector<DanceHall> halls;
    
    try {
        std::cout << "🔍 Получение всех залов из MongoDB" << std::endl;
        
        auto collection = getCollection();
        auto cursor = collection.find({});
        
        int count = 0;
        for (auto&& doc : cursor) {
            try {
                auto hall = mapDocumentToDanceHall(doc);
                halls.push_back(hall);
                count++;
            } catch (const std::exception& e) {
                std::cerr << "❌ Ошибка при маппинге зала из MongoDB: " << e.what() << std::endl;
                continue;
            }
        }
        
        std::cout << "✅ Успешно загружено залов из MongoDB: " << count << std::endl;
        return halls;
        
    } catch (const std::exception& e) {
        std::cerr << "❌ MongoDB Error in findAll: " << e.what() << std::endl;
        throw DataAccessException(std::string("Failed to find all halls: ") + e.what());
    }
}

bool MongoDBDanceHallRepository::save(const DanceHall& hall) {
    validateDanceHall(hall);
    
    try {
        auto collection = getCollection();
        auto document = mapDanceHallToDocument(hall);
        
        auto result = collection.insert_one(document.view());
        
        if (result && result->result().inserted_count() > 0) {
            std::cout << "✅ Зал успешно сохранен в MongoDB: " << hall.getId().toString() << std::endl;
            return true;
        }
        
        std::cerr << "❌ Не удалось сохранить зал в MongoDB" << std::endl;
        return false;
        
    } catch (const std::exception& e) {
        std::cerr << "❌ MongoDB Error in save: " << e.what() << std::endl;
        throw DataAccessException(std::string("Failed to save hall: ") + e.what());
    }
}

bool MongoDBDanceHallRepository::update(const DanceHall& hall) {
    validateDanceHall(hall);
    
    try {
        auto collection = getCollection();
        auto filter = bsoncxx::builder::stream::document{}
            << "id" << hall.getId().toString()
            << bsoncxx::builder::stream::finalize;
        
        auto update_doc = bsoncxx::builder::stream::document{}
            << "$set" << bsoncxx::builder::stream::open_document
                << "name" << hall.getName()
                << "description" << hall.getDescription()
                << "capacity" << hall.getCapacity()
                << "floorType" << hall.getFloorType()
                << "equipment" << hall.getEquipment()
                << "branchId" << hall.getBranchId().toString()
            << bsoncxx::builder::stream::close_document
            << bsoncxx::builder::stream::finalize;
        
        auto result = collection.update_one(filter.view(), update_doc.view());
        
        if (result && result->modified_count() > 0) {
            std::cout << "✅ Зал успешно обновлен в MongoDB: " << hall.getId().toString() << std::endl;
            return true;
        }
        
        std::cout << "⚠️  Зал не найден для обновления в MongoDB: " << hall.getId().toString() << std::endl;
        return false;
        
    } catch (const std::exception& e) {
        std::cerr << "❌ MongoDB Error in update: " << e.what() << std::endl;
        throw DataAccessException(std::string("Failed to update hall: ") + e.what());
    }
}

bool MongoDBDanceHallRepository::remove(const UUID& id) {
    try {
        auto collection = getCollection();
        auto filter = bsoncxx::builder::stream::document{}
            << "id" << id.toString()
            << bsoncxx::builder::stream::finalize;
        
        auto result = collection.delete_one(filter.view());
        
        if (result && result->deleted_count() > 0) {
            std::cout << "✅ Зал успешно удален из MongoDB: " << id.toString() << std::endl;
            return true;
        }
        
        std::cout << "⚠️  Зал не найден для удаления в MongoDB: " << id.toString() << std::endl;
        return false;
        
    } catch (const std::exception& e) {
        std::cerr << "❌ MongoDB Error in remove: " << e.what() << std::endl;
        throw DataAccessException(std::string("Failed to remove hall: ") + e.what());
    }
}

DanceHall MongoDBDanceHallRepository::mapDocumentToDanceHall(const bsoncxx::document::view& doc) const {
    try {
        UUID id = UUID::fromString(doc["id"].get_string().value.to_string());
        std::string name = doc["name"].get_string().value.to_string();
        std::string description = doc["description"].get_string().value.to_string();
        int capacity = doc["capacity"].get_int32();
        std::string floorType = doc["floorType"].get_string().value.to_string();
        std::string equipment = doc["equipment"].get_string().value.to_string();
        UUID branchId = UUID::fromString(doc["branchId"].get_string().value.to_string());
        
        // Корректируем проблемные данные перед созданием объекта
        if (name.empty()) {
            std::cerr << "⚠️  Пустое название зала в MongoDB, устанавливаем значение по умолчанию" << std::endl;
            name = "Зал " + id.toString().substr(0, 8);
        }
        
        if (capacity <= 0) {
            std::cerr << "⚠️  Неверная вместимость в MongoDB: " << capacity << ", устанавливаем 10" << std::endl;
            capacity = 10;
        }
        
        if (floorType.empty()) {
            std::cerr << "⚠️  Пустой тип покрытия в MongoDB, устанавливаем 'стандартное'" << std::endl;
            floorType = "стандартное";
        }
        
        if (equipment.empty()) {
            equipment = "стандартное оборудование";
        }
        
        // Создаем зал с исправленными данными
        DanceHall hall(id, name, capacity, branchId);
        hall.setDescription(description);
        hall.setFloorType(floorType);
        hall.setEquipment(equipment);
        
        // Проверяем валидность
        if (!hall.isValid()) {
            std::string error = "Invalid dance hall data from MongoDB after correction: ";
            error += "id=" + id.toString();
            error += ", name=" + name;
            error += ", capacity=" + std::to_string(capacity);
            error += ", floorType=" + floorType;
            error += ", branchId=" + branchId.toString();
            throw std::invalid_argument(error);
        }
        
        return hall;
        
    } catch (const std::exception& e) {
        std::cerr << "❌ Критическая ошибка маппинга DanceHall из MongoDB: " << e.what() << std::endl;
        throw DataAccessException("Failed to map MongoDB document to DanceHall");
    }
}

bsoncxx::document::value MongoDBDanceHallRepository::mapDanceHallToDocument(const DanceHall& hall) const {
    return bsoncxx::builder::stream::document{}
        << "id" << hall.getId().toString()
        << "name" << hall.getName()
        << "description" << hall.getDescription()
        << "capacity" << hall.getCapacity()
        << "floorType" << hall.getFloorType()
        << "equipment" << hall.getEquipment()
        << "branchId" << hall.getBranchId().toString()
        << bsoncxx::builder::stream::finalize;
}

void MongoDBDanceHallRepository::validateDanceHall(const DanceHall& hall) const {
    if (!hall.isValid()) {
        throw DataAccessException("Invalid hall data for MongoDB");
    }
}
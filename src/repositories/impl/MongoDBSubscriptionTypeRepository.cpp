#include "MongoDBSubscriptionTypeRepository.hpp"
#include "../../data/MongoDBRepositoryFactory.hpp"
#include "../../data/DateTimeUtils.hpp"
#include <bsoncxx/builder/basic/document.hpp>
#include <bsoncxx/builder/basic/array.hpp>
#include <bsoncxx/builder/basic/kvp.hpp>
#include <iostream>

MongoDBSubscriptionTypeRepository::MongoDBSubscriptionTypeRepository(std::shared_ptr<MongoDBRepositoryFactory> factory)
    : factory_(std::move(factory)) {}

mongocxx::collection MongoDBSubscriptionTypeRepository::getCollection() const {
    return factory_->getDatabase().collection("subscription_types");
}

std::optional<SubscriptionType> MongoDBSubscriptionTypeRepository::findById(const UUID& id) {
    try {
        auto collection = getCollection();
        auto filter = bsoncxx::builder::stream::document{}
            << "id" << id.toString()
            << bsoncxx::builder::stream::finalize;
        
        auto result = collection.find_one(filter.view());
        
        if (!result) {
            std::cout << "❌ Тип абонемента не найден в MongoDB: " << id.toString() << std::endl;
            return std::nullopt;
        }
        
        std::cout << "✅ Тип абонемента найден в MongoDB: " << id.toString() << std::endl;
        return mapDocumentToSubscriptionType(result->view());
        
    } catch (const std::exception& e) {
        std::cerr << "❌ MongoDB Error in findById: " << e.what() << std::endl;
        throw DataAccessException(std::string("Failed to find subscription type by ID: ") + e.what());
    }
}

std::vector<SubscriptionType> MongoDBSubscriptionTypeRepository::findAllActive() {
    std::vector<SubscriptionType> subscriptionTypes;
    
    try {
        std::cout << "🔍 Поиск активных типов абонементов в MongoDB" << std::endl;
        
        auto collection = getCollection();
        
        // Активные типы: безлимитные или с количеством посещений > 0
        auto filter = bsoncxx::builder::stream::document{}
            << "$or" << bsoncxx::builder::stream::open_array
                << bsoncxx::builder::stream::open_document
                    << "unlimited" << true
                << bsoncxx::builder::stream::close_document
                << bsoncxx::builder::stream::open_document
                    << "visitCount" << bsoncxx::builder::stream::open_document
                        << "$gt" << 0
                    << bsoncxx::builder::stream::close_document
                << bsoncxx::builder::stream::close_document
            << bsoncxx::builder::stream::close_array
            << bsoncxx::builder::stream::finalize;
        
        auto cursor = collection.find(filter.view());
        
        int count = 0;
        for (auto&& doc : cursor) {
            try {
                auto subscriptionType = mapDocumentToSubscriptionType(doc);
                subscriptionTypes.push_back(subscriptionType);
                count++;
                std::cout << "✅ Найден активный тип абонемента: " << subscriptionType.getName() << std::endl;
            } catch (const std::exception& e) {
                std::cerr << "❌ Ошибка создания типа абонемента из MongoDB документа: " << e.what() << std::endl;
                continue;
            }
        }
        
        std::cout << "📊 Найдено активных типов абонементов в MongoDB: " << count << std::endl;
        return subscriptionTypes;
        
    } catch (const std::exception& e) {
        std::cerr << "❌ MongoDB Error in findAllActive: " << e.what() << std::endl;
        throw DataAccessException(std::string("Failed to find active subscription types: ") + e.what());
    }
}

std::vector<SubscriptionType> MongoDBSubscriptionTypeRepository::findAll() {
    std::vector<SubscriptionType> subscriptionTypes;
    
    try {
        std::cout << "🔍 Получение всех типов абонементов из MongoDB" << std::endl;
        
        auto collection = getCollection();
        auto cursor = collection.find({});
        
        int count = 0;
        for (auto&& doc : cursor) {
            try {
                auto subscriptionType = mapDocumentToSubscriptionType(doc);
                subscriptionTypes.push_back(subscriptionType);
                count++;
            } catch (const std::exception& e) {
                std::cerr << "❌ Ошибка при маппинге типа абонемента из MongoDB: " << e.what() << std::endl;
                continue;
            }
        }
        
        std::cout << "✅ Успешно загружено типов абонементов из MongoDB: " << count << std::endl;
        return subscriptionTypes;
        
    } catch (const std::exception& e) {
        std::cerr << "❌ MongoDB Error in findAll: " << e.what() << std::endl;
        throw DataAccessException(std::string("Failed to find all subscription types: ") + e.what());
    }
}

bool MongoDBSubscriptionTypeRepository::save(const SubscriptionType& subscriptionType) {
    validateSubscriptionType(subscriptionType);
    
    try {
        auto collection = getCollection();
        auto document = mapSubscriptionTypeToDocument(subscriptionType);
        
        auto result = collection.insert_one(document.view());
        
        if (result && result->result().inserted_count() > 0) {
            std::cout << "✅ Тип абонемента успешно сохранен в MongoDB: " << subscriptionType.getId().toString() << std::endl;
            return true;
        }
        
        std::cerr << "❌ Не удалось сохранить тип абонемента в MongoDB" << std::endl;
        return false;
        
    } catch (const std::exception& e) {
        std::cerr << "❌ MongoDB Error in save: " << e.what() << std::endl;
        throw DataAccessException(std::string("Failed to save subscription type: ") + e.what());
    }
}

bool MongoDBSubscriptionTypeRepository::update(const SubscriptionType& subscriptionType) {
    validateSubscriptionType(subscriptionType);
    
    try {
        auto collection = getCollection();
        auto filter = bsoncxx::builder::stream::document{}
            << "id" << subscriptionType.getId().toString()
            << bsoncxx::builder::stream::finalize;
        
        auto update_doc = bsoncxx::builder::stream::document{}
            << "$set" << bsoncxx::builder::stream::open_document
                << "name" << subscriptionType.getName()
                << "description" << subscriptionType.getDescription()
                << "validityDays" << subscriptionType.getValidityDays()
                << "visitCount" << subscriptionType.getVisitCount()
                << "unlimited" << subscriptionType.isUnlimited()
                << "price" << subscriptionType.getPrice()
            << bsoncxx::builder::stream::close_document
            << bsoncxx::builder::stream::finalize;
        
        auto result = collection.update_one(filter.view(), update_doc.view());
        
        if (result && result->modified_count() > 0) {
            std::cout << "✅ Тип абонемента успешно обновлен в MongoDB: " << subscriptionType.getId().toString() << std::endl;
            return true;
        }
        
        std::cout << "⚠️  Тип абонемента не найден для обновления в MongoDB: " << subscriptionType.getId().toString() << std::endl;
        return false;
        
    } catch (const std::exception& e) {
        std::cerr << "❌ MongoDB Error in update: " << e.what() << std::endl;
        throw DataAccessException(std::string("Failed to update subscription type: ") + e.what());
    }
}

bool MongoDBSubscriptionTypeRepository::remove(const UUID& id) {
    try {
        auto collection = getCollection();
        auto filter = bsoncxx::builder::stream::document{}
            << "id" << id.toString()
            << bsoncxx::builder::stream::finalize;
        
        auto result = collection.delete_one(filter.view());
        
        if (result && result->deleted_count() > 0) {
            std::cout << "✅ Тип абонемента успешно удален из MongoDB: " << id.toString() << std::endl;
            return true;
        }
        
        std::cout << "⚠️  Тип абонемента не найден для удаления в MongoDB: " << id.toString() << std::endl;
        return false;
        
    } catch (const std::exception& e) {
        std::cerr << "❌ MongoDB Error in remove: " << e.what() << std::endl;
        throw DataAccessException(std::string("Failed to remove subscription type: ") + e.what());
    }
}

bool MongoDBSubscriptionTypeRepository::exists(const UUID& id) {
    try {
        auto collection = getCollection();
        auto filter = bsoncxx::builder::stream::document{}
            << "id" << id.toString()
            << bsoncxx::builder::stream::finalize;
        
        auto result = collection.count_documents(filter.view());
        return result > 0;
        
    } catch (const std::exception& e) {
        std::cerr << "❌ MongoDB Error in exists: " << e.what() << std::endl;
        throw DataAccessException(std::string("Failed to check subscription type existence: ") + e.what());
    }
}

SubscriptionType MongoDBSubscriptionTypeRepository::mapDocumentToSubscriptionType(const bsoncxx::document::view& doc) const {
    try {
        UUID id = UUID::fromString(doc["id"].get_string().value.to_string());
        std::string name = doc["name"].get_string().value.to_string();
        std::string description = doc["description"].get_string().value.to_string();
        int validityDays = doc["validityDays"].get_int32();
        int visitCount = doc["visitCount"].get_int32();
        bool unlimited = doc["unlimited"].get_bool().value;
        double price = doc["price"].get_double().value;
        
        // Создаем тип абонемента
        SubscriptionType subscriptionType(id, name, validityDays, visitCount, unlimited, price);
        subscriptionType.setDescription(description);
        
        // Проверяем валидность
        if (!subscriptionType.isValid()) {
            std::string error = "Invalid subscription type data from MongoDB: ";
            error += "id=" + id.toString();
            error += ", name=" + name;
            error += ", validityDays=" + std::to_string(validityDays);
            error += ", visitCount=" + std::to_string(visitCount);
            error += ", unlimited=" + std::to_string(unlimited);
            error += ", price=" + std::to_string(price);
            throw std::invalid_argument(error);
        }
        
        return subscriptionType;
        
    } catch (const std::exception& e) {
        std::cerr << "❌ Критическая ошибка маппинга SubscriptionType из MongoDB: " << e.what() << std::endl;
        throw DataAccessException("Failed to map MongoDB document to SubscriptionType");
    }
}

bsoncxx::document::value MongoDBSubscriptionTypeRepository::mapSubscriptionTypeToDocument(const SubscriptionType& subscriptionType) const {
    return bsoncxx::builder::stream::document{}
        << "id" << subscriptionType.getId().toString()
        << "name" << subscriptionType.getName()
        << "description" << subscriptionType.getDescription()
        << "validityDays" << subscriptionType.getValidityDays()
        << "visitCount" << subscriptionType.getVisitCount()
        << "unlimited" << subscriptionType.isUnlimited()
        << "price" << subscriptionType.getPrice()
        << bsoncxx::builder::stream::finalize;
}

void MongoDBSubscriptionTypeRepository::validateSubscriptionType(const SubscriptionType& subscriptionType) const {
    if (!subscriptionType.isValid()) {
        throw DataAccessException("Invalid subscription type data for MongoDB");
    }
}
#include "MongoDBSubscriptionRepository.hpp"
#include "../../data/MongoDBRepositoryFactory.hpp"
#include "../../data/DateTimeUtils.hpp"
#include <bsoncxx/builder/basic/document.hpp>
#include <bsoncxx/builder/basic/array.hpp>
#include <bsoncxx/builder/basic/kvp.hpp>
#include <iostream>

MongoDBSubscriptionRepository::MongoDBSubscriptionRepository(std::shared_ptr<MongoDBRepositoryFactory> factory)
    : factory_(std::move(factory)) {}

mongocxx::collection MongoDBSubscriptionRepository::getCollection() const {
    return factory_->getDatabase().collection("subscriptions");
}

std::optional<Subscription> MongoDBSubscriptionRepository::findById(const UUID& id) {
    try {
        auto collection = getCollection();
        auto filter = bsoncxx::builder::stream::document{}
            << "id" << id.toString()
            << bsoncxx::builder::stream::finalize;
        
        auto result = collection.find_one(filter.view());
        
        if (!result) {
            std::cout << "❌ Подписка не найдена в MongoDB: " << id.toString() << std::endl;
            return std::nullopt;
        }
        
        std::cout << "✅ Подписка найдена в MongoDB: " << id.toString() << std::endl;
        return mapDocumentToSubscription(result->view());
        
    } catch (const std::exception& e) {
        std::cerr << "❌ MongoDB Error in findById: " << e.what() << std::endl;
        throw DataAccessException(std::string("Failed to find subscription by ID: ") + e.what());
    }
}

std::vector<Subscription> MongoDBSubscriptionRepository::findByClientId(const UUID& clientId) {
    std::vector<Subscription> subscriptions;
    
    try {
        std::cout << "🔍 Поиск подписок клиента в MongoDB: " << clientId.toString() << std::endl;
        
        auto collection = getCollection();
        auto filter = bsoncxx::builder::stream::document{}
            << "clientId" << clientId.toString()
            << bsoncxx::builder::stream::finalize;
        
        auto cursor = collection.find(filter.view());
        
        int count = 0;
        for (auto&& doc : cursor) {
            try {
                auto subscription = mapDocumentToSubscription(doc);
                subscriptions.push_back(subscription);
                count++;
                std::cout << "✅ Найдена подписка: " << subscription.getId().toString() << std::endl;
            } catch (const std::exception& e) {
                std::cerr << "❌ Ошибка создания подписки из MongoDB документа: " << e.what() << std::endl;
                continue;
            }
        }
        
        std::cout << "📊 Найдено подписок в MongoDB: " << count << std::endl;
        return subscriptions;
        
    } catch (const std::exception& e) {
        std::cerr << "❌ MongoDB Error in findByClientId: " << e.what() << std::endl;
        throw DataAccessException(std::string("Failed to find subscriptions by client ID: ") + e.what());
    }
}

std::vector<Subscription> MongoDBSubscriptionRepository::findActiveSubscriptions() {
    std::vector<Subscription> subscriptions;
    
    try {
        std::cout << "🔍 Поиск активных подписок в MongoDB" << std::endl;
        
        auto collection = getCollection();
        
        // Активные подписки: статус ACTIVE и дата окончания в будущем
        auto now = std::chrono::system_clock::now();
        auto filter = bsoncxx::builder::stream::document{}
            << "status" << "ACTIVE"
            << "endDate" << bsoncxx::builder::stream::open_document
                << "$gte" << DateTimeUtils::formatTimeForMongoDB(now)
            << bsoncxx::builder::stream::close_document
            << bsoncxx::builder::stream::finalize;
        
        auto cursor = collection.find(filter.view());
        
        int count = 0;
        for (auto&& doc : cursor) {
            try {
                auto subscription = mapDocumentToSubscription(doc);
                subscriptions.push_back(subscription);
                count++;
            } catch (const std::exception& e) {
                std::cerr << "❌ Ошибка при маппинге подписки из MongoDB: " << e.what() << std::endl;
                continue;
            }
        }
        
        std::cout << "✅ Успешно загружено активных подписок из MongoDB: " << count << std::endl;
        return subscriptions;
        
    } catch (const std::exception& e) {
        std::cerr << "❌ MongoDB Error in findActiveSubscriptions: " << e.what() << std::endl;
        throw DataAccessException(std::string("Failed to find active subscriptions: ") + e.what());
    }
}

std::vector<Subscription> MongoDBSubscriptionRepository::findExpiringSubscriptions(int days) {
    std::vector<Subscription> subscriptions;
    
    try {
        std::cout << "🔍 Поиск истекающих подписок в MongoDB (дней: " << days << ")" << std::endl;
        
        auto collection = getCollection();
        
        auto now = std::chrono::system_clock::now();
        auto expirationThreshold = now + std::chrono::hours(24 * days);
        
        // Подписки, которые истекают в течение указанного количества дней
        auto filter = bsoncxx::builder::stream::document{}
            << "status" << "ACTIVE"
            << "endDate" << bsoncxx::builder::stream::open_document
                << "$gte" << DateTimeUtils::formatTimeForMongoDB(now)
                << "$lte" << DateTimeUtils::formatTimeForMongoDB(expirationThreshold)
            << bsoncxx::builder::stream::close_document
            << bsoncxx::builder::stream::finalize;
        
        auto cursor = collection.find(filter.view());
        
        int count = 0;
        for (auto&& doc : cursor) {
            try {
                auto subscription = mapDocumentToSubscription(doc);
                subscriptions.push_back(subscription);
                count++;
            } catch (const std::exception& e) {
                std::cerr << "❌ Ошибка при маппинге истекающей подписки из MongoDB: " << e.what() << std::endl;
                continue;
            }
        }
        
        std::cout << "✅ Найдено истекающих подписок в MongoDB: " << count << std::endl;
        return subscriptions;
        
    } catch (const std::exception& e) {
        std::cerr << "❌ MongoDB Error in findExpiringSubscriptions: " << e.what() << std::endl;
        throw DataAccessException(std::string("Failed to find expiring subscriptions: ") + e.what());
    }
}

bool MongoDBSubscriptionRepository::save(const Subscription& subscription) {
    validateSubscription(subscription);
    
    try {
        auto collection = getCollection();
        auto document = mapSubscriptionToDocument(subscription);
        
        auto result = collection.insert_one(document.view());
        
        if (result && result->result().inserted_count() > 0) {
            std::cout << "✅ Подписка успешно сохранена в MongoDB: " << subscription.getId().toString() << std::endl;
            return true;
        }
        
        std::cerr << "❌ Не удалось сохранить подписку в MongoDB" << std::endl;
        return false;
        
    } catch (const std::exception& e) {
        std::cerr << "❌ MongoDB Error in save: " << e.what() << std::endl;
        throw DataAccessException(std::string("Failed to save subscription: ") + e.what());
    }
}

bool MongoDBSubscriptionRepository::update(const Subscription& subscription) {
    validateSubscription(subscription);
    
    try {
        auto collection = getCollection();
        auto filter = bsoncxx::builder::stream::document{}
            << "id" << subscription.getId().toString()
            << bsoncxx::builder::stream::finalize;
        
        auto update_doc = bsoncxx::builder::stream::document{}
            << "$set" << bsoncxx::builder::stream::open_document
                << "clientId" << subscription.getClientId().toString()
                << "subscriptionTypeId" << subscription.getSubscriptionTypeId().toString()
                << "startDate" << DateTimeUtils::formatTimeForMongoDB(subscription.getStartDate())
                << "endDate" << DateTimeUtils::formatTimeForMongoDB(subscription.getEndDate())
                << "remainingVisits" << subscription.getRemainingVisits()
                << "status" << subscriptionStatusToString(subscription.getStatus())
                << "purchaseDate" << DateTimeUtils::formatTimeForMongoDB(subscription.getPurchaseDate())
            << bsoncxx::builder::stream::close_document
            << bsoncxx::builder::stream::finalize;
        
        auto result = collection.update_one(filter.view(), update_doc.view());
        
        if (result && result->modified_count() > 0) {
            std::cout << "✅ Подписка успешно обновлена в MongoDB: " << subscription.getId().toString() << std::endl;
            return true;
        }
        
        std::cout << "⚠️  Подписка не найдена для обновления в MongoDB: " << subscription.getId().toString() << std::endl;
        return false;
        
    } catch (const std::exception& e) {
        std::cerr << "❌ MongoDB Error in update: " << e.what() << std::endl;
        throw DataAccessException(std::string("Failed to update subscription: ") + e.what());
    }
}

bool MongoDBSubscriptionRepository::remove(const UUID& id) {
    try {
        auto collection = getCollection();
        auto filter = bsoncxx::builder::stream::document{}
            << "id" << id.toString()
            << bsoncxx::builder::stream::finalize;
        
        auto result = collection.delete_one(filter.view());
        
        if (result && result->deleted_count() > 0) {
            std::cout << "✅ Подписка успешно удалена из MongoDB: " << id.toString() << std::endl;
            return true;
        }
        
        std::cout << "⚠️  Подписка не найдена для удаления в MongoDB: " << id.toString() << std::endl;
        return false;
        
    } catch (const std::exception& e) {
        std::cerr << "❌ MongoDB Error in remove: " << e.what() << std::endl;
        throw DataAccessException(std::string("Failed to remove subscription: ") + e.what());
    }
}

bool MongoDBSubscriptionRepository::exists(const UUID& id) {
    try {
        auto collection = getCollection();
        auto filter = bsoncxx::builder::stream::document{}
            << "id" << id.toString()
            << bsoncxx::builder::stream::finalize;
        
        auto result = collection.count_documents(filter.view());
        return result > 0;
        
    } catch (const std::exception& e) {
        std::cerr << "❌ MongoDB Error in exists: " << e.what() << std::endl;
        throw DataAccessException(std::string("Failed to check subscription existence: ") + e.what());
    }
}

Subscription MongoDBSubscriptionRepository::mapDocumentToSubscription(const bsoncxx::document::view& doc) const {
    try {
        UUID id = UUID::fromString(doc["id"].get_string().value.to_string());
        UUID clientId = UUID::fromString(doc["clientId"].get_string().value.to_string());
        UUID subscriptionTypeId = UUID::fromString(doc["subscriptionTypeId"].get_string().value.to_string());
        
        auto startDate = DateTimeUtils::parseTimeFromMongoDB(doc["startDate"].get_string().value.to_string());
        auto endDate = DateTimeUtils::parseTimeFromMongoDB(doc["endDate"].get_string().value.to_string());
        int remainingVisits = doc["remainingVisits"].get_int32();
        
        // Создаем подписку
        Subscription subscription(id, clientId, subscriptionTypeId, startDate, endDate, remainingVisits);
        
        // Восстанавливаем статус
        std::string statusStr = doc["status"].get_string().value.to_string();
        SubscriptionStatus status = stringToSubscriptionStatus(statusStr);
        
        switch (status) {
            case SubscriptionStatus::SUSPENDED:
                subscription.suspend();
                break;
            case SubscriptionStatus::CANCELLED:
                subscription.cancel();
                break;
            case SubscriptionStatus::EXPIRED:
                // Для EXPIRED статуса просто оставляем подписку как есть
                break;
            default:
                break; // ACTIVE по умолчанию
        }
        
        // Устанавливаем дату покупки если она есть
        if (doc["purchaseDate"]) {
            auto purchaseDate = DateTimeUtils::parseTimeFromMongoDB(doc["purchaseDate"].get_string().value.to_string());
            // В модели нет сеттера для purchaseDate, поэтому оставляем как есть
        }
        
        // Проверяем валидность
        if (!subscription.isValid()) {
            std::string error = "Invalid subscription data from MongoDB: ";
            error += "id=" + id.toString();
            error += ", clientId=" + clientId.toString();
            error += ", subscriptionTypeId=" + subscriptionTypeId.toString();
            error += ", remainingVisits=" + std::to_string(remainingVisits);
            error += ", status=" + statusStr;
            throw std::invalid_argument(error);
        }
        
        return subscription;
        
    } catch (const std::exception& e) {
        std::cerr << "❌ Критическая ошибка маппинга Subscription из MongoDB: " << e.what() << std::endl;
        throw DataAccessException("Failed to map MongoDB document to Subscription");
    }
}

bsoncxx::document::value MongoDBSubscriptionRepository::mapSubscriptionToDocument(const Subscription& subscription) const {
    return bsoncxx::builder::stream::document{}
        << "id" << subscription.getId().toString()
        << "clientId" << subscription.getClientId().toString()
        << "subscriptionTypeId" << subscription.getSubscriptionTypeId().toString()
        << "startDate" << DateTimeUtils::formatTimeForMongoDB(subscription.getStartDate())
        << "endDate" << DateTimeUtils::formatTimeForMongoDB(subscription.getEndDate())
        << "remainingVisits" << subscription.getRemainingVisits()
        << "status" << subscriptionStatusToString(subscription.getStatus())
        << "purchaseDate" << DateTimeUtils::formatTimeForMongoDB(subscription.getPurchaseDate())
        << bsoncxx::builder::stream::finalize;
}

void MongoDBSubscriptionRepository::validateSubscription(const Subscription& subscription) const {
    if (!subscription.isValid()) {
        throw DataAccessException("Invalid subscription data for MongoDB");
    }
}

std::string MongoDBSubscriptionRepository::subscriptionStatusToString(SubscriptionStatus status) const {
    switch (status) {
        case SubscriptionStatus::ACTIVE: return "ACTIVE";
        case SubscriptionStatus::SUSPENDED: return "SUSPENDED";
        case SubscriptionStatus::EXPIRED: return "EXPIRED";
        case SubscriptionStatus::CANCELLED: return "CANCELLED";
        default: return "ACTIVE";
    }
}

SubscriptionStatus MongoDBSubscriptionRepository::stringToSubscriptionStatus(const std::string& status) const {
    if (status == "SUSPENDED") return SubscriptionStatus::SUSPENDED;
    if (status == "EXPIRED") return SubscriptionStatus::EXPIRED;
    if (status == "CANCELLED") return SubscriptionStatus::CANCELLED;
    return SubscriptionStatus::ACTIVE;
}
#include "PostgreSQLSubscriptionRepository.hpp"
#include "../../data/SqlQueryBuilder.hpp"
#include "../../data/DateTimeUtils.hpp"
#include "../../data/QueryFactory.hpp"

PostgreSQLSubscriptionRepository::PostgreSQLSubscriptionRepository(
    std::shared_ptr<DatabaseConnection> dbConnection)
    : dbConnection_(std::move(dbConnection)) {}

std::optional<Subscription> PostgreSQLSubscriptionRepository::findById(const UUID& id) {
    try {
        auto work = dbConnection_->beginTransaction();
        
        SqlQueryBuilder queryBuilder;
        std::string query = queryBuilder
            .select({"id", "client_id", "subscription_type_id", "start_date", "end_date", 
                     "remaining_visits", "status", "purchase_date"})
            .from("subscriptions")
            .where("id = $1")
            .build();
        
        auto result = work.exec_params(query, id.toString());
        
        if (result.empty()) {
            return std::nullopt;
        }
        
        auto subscription = mapResultToSubscription(result[0]);
        dbConnection_->commitTransaction(work);
        return subscription;
        
    } catch (const std::exception& e) {
        throw QueryException(std::string("Failed to find subscription by ID: ") + e.what());
    }
}

std::vector<Subscription> PostgreSQLSubscriptionRepository::findByClientId(const UUID& clientId) {
    try {
        auto work = dbConnection_->beginTransaction();
        
        SqlQueryBuilder queryBuilder;
        std::string query = queryBuilder
            .select({"id", "client_id", "subscription_type_id", "start_date", "end_date", 
                     "remaining_visits", "status", "purchase_date"})
            .from("subscriptions")
            .where("client_id = $1")
            .build();
        
        auto result = work.exec_params(query, clientId.toString());
        
        std::vector<Subscription> subscriptions;
        for (const auto& row : result) {
            subscriptions.push_back(mapResultToSubscription(row));
        }
        
        dbConnection_->commitTransaction(work);
        return subscriptions;
        
    } catch (const std::exception& e) {
        throw QueryException(std::string("Failed to find subscriptions by client ID: ") + e.what());
    }
}

std::vector<Subscription> PostgreSQLSubscriptionRepository::findActiveSubscriptions() {
    try {
        auto work = dbConnection_->beginTransaction();
        
        SqlQueryBuilder queryBuilder;
        std::string query = queryBuilder
            .select({"id", "client_id", "subscription_type_id", "start_date", "end_date", 
                     "remaining_visits", "status", "purchase_date"})
            .from("subscriptions")
            .where("status = 'ACTIVE'")
            .build();
        
        auto result = work.exec(query);
        
        std::vector<Subscription> subscriptions;
        for (const auto& row : result) {
            subscriptions.push_back(mapResultToSubscription(row));
        }
        
        dbConnection_->commitTransaction(work);
        return subscriptions;
        
    } catch (const std::exception& e) {
        throw QueryException(std::string("Failed to find active subscriptions: ") + e.what());
    }
}

std::vector<Subscription> PostgreSQLSubscriptionRepository::findExpiringSubscriptions(int days) {
    try {
        auto work = dbConnection_->beginTransaction();
        
        std::string query = QueryFactory::createFindExpiringSubscriptionsQuery();
        auto result = work.exec_params(query, days);
        
        std::vector<Subscription> subscriptions;
        for (const auto& row : result) {
            subscriptions.push_back(mapResultToSubscription(row));
        }
        
        dbConnection_->commitTransaction(work);
        return subscriptions;
        
    } catch (const std::exception& e) {
        throw QueryException(std::string("Failed to find expiring subscriptions: ") + e.what());
    }
}

bool PostgreSQLSubscriptionRepository::save(const Subscription& subscription) {
    validateSubscription(subscription);
    
    try {
        auto work = dbConnection_->beginTransaction();
        
        std::map<std::string, std::string> values = {
            {"id", "$1"},
            {"client_id", "$2"},
            {"subscription_type_id", "$3"},
            {"start_date", "$4"},
            {"end_date", "$5"},
            {"remaining_visits", "$6"},
            {"status", "$7"},
            {"purchase_date", "$8"}
        };
        
        SqlQueryBuilder queryBuilder;
        std::string query = queryBuilder
            .insertInto("subscriptions")
            .values(values)
            .build();
        
        work.exec_params(
            query,
            subscription.getId().toString(),
            subscription.getClientId().toString(),
            subscription.getSubscriptionTypeId().toString(),
            DateTimeUtils::formatTimeForPostgres(subscription.getStartDate()),
            DateTimeUtils::formatTimeForPostgres(subscription.getEndDate()),
            std::to_string(subscription.getRemainingVisits()),
            subscriptionStatusToString(subscription.getStatus()),
            DateTimeUtils::formatTimeForPostgres(subscription.getPurchaseDate())
        );
        
        dbConnection_->commitTransaction(work);
        return true;
        
    } catch (const std::exception& e) {
        throw QueryException(std::string("Failed to save subscription: ") + e.what());
    }
}

bool PostgreSQLSubscriptionRepository::update(const Subscription& subscription) {
    validateSubscription(subscription);
    
    try {
        auto work = dbConnection_->beginTransaction();
        
        std::map<std::string, std::string> values = {
            {"client_id", "$2"},
            {"subscription_type_id", "$3"},
            {"start_date", "$4"},
            {"end_date", "$5"},
            {"remaining_visits", "$6"},
            {"status", "$7"}
        };
        
        SqlQueryBuilder queryBuilder;
        std::string query = queryBuilder
            .update("subscriptions")
            .set(values)
            .where("id = $1")
            .build();
        
        auto result = work.exec_params(
            query,
            subscription.getId().toString(),
            subscription.getClientId().toString(),
            subscription.getSubscriptionTypeId().toString(),
            DateTimeUtils::formatTimeForPostgres(subscription.getStartDate()),
            DateTimeUtils::formatTimeForPostgres(subscription.getEndDate()),
            std::to_string(subscription.getRemainingVisits()),
            subscriptionStatusToString(subscription.getStatus())
        );
        
        dbConnection_->commitTransaction(work);
        return result.affected_rows() > 0;
        
    } catch (const std::exception& e) {
        throw QueryException(std::string("Failed to update subscription: ") + e.what());
    }
}

bool PostgreSQLSubscriptionRepository::remove(const UUID& id) {
    try {
        auto work = dbConnection_->beginTransaction();
        
        SqlQueryBuilder queryBuilder;
        std::string query = queryBuilder
            .deleteFrom("subscriptions")
            .where("id = $1")
            .build();
            
        auto result = work.exec_params(query, id.toString());
        
        dbConnection_->commitTransaction(work);
        return result.affected_rows() > 0;
        
    } catch (const std::exception& e) {
        throw QueryException(std::string("Failed to remove subscription: ") + e.what());
    }
}

bool PostgreSQLSubscriptionRepository::exists(const UUID& id) {
    try {
        auto work = dbConnection_->beginTransaction();
        
        SqlQueryBuilder queryBuilder;
        std::string query = queryBuilder
            .select({"1"})
            .from("subscriptions")
            .where("id = $1")
            .build();
            
        auto result = work.exec_params(query, id.toString());
        
        dbConnection_->commitTransaction(work);
        return !result.empty();
        
    } catch (const std::exception& e) {
        throw QueryException(std::string("Failed to check subscription existence: ") + e.what());
    }
}

Subscription PostgreSQLSubscriptionRepository::mapResultToSubscription(const pqxx::row& row) const {
    UUID id = UUID::fromString(row["id"].c_str());
    UUID clientId = UUID::fromString(row["client_id"].c_str());
    UUID subscriptionTypeId = UUID::fromString(row["subscription_type_id"].c_str());
    
    auto startDate = DateTimeUtils::parseTimeFromPostgres(row["start_date"].c_str());
    auto endDate = DateTimeUtils::parseTimeFromPostgres(row["end_date"].c_str());
    int remainingVisits = row["remaining_visits"].as<int>();
    
    Subscription subscription(id, clientId, subscriptionTypeId, startDate, endDate, remainingVisits);
    
    // Восстанавливаем статус
    SubscriptionStatus status = stringToSubscriptionStatus(row["status"].c_str());
    switch (status) {
        case SubscriptionStatus::SUSPENDED:
            subscription.suspend();
            break;
        case SubscriptionStatus::CANCELLED:
            subscription.cancel();
            break;
        case SubscriptionStatus::EXPIRED:
            // Для EXPIRED статуса просто оставляем подписку как есть,
            // так как expire() метода нет в модели Subscription
            // Статус будет установлен автоматически при проверке дат
            break;
        default:
            break; // ACTIVE по умолчанию
    }
    
    return subscription;
}

void PostgreSQLSubscriptionRepository::validateSubscription(const Subscription& subscription) const {
    if (!subscription.isValid()) {
        throw DataAccessException("Invalid subscription data");
    }
}

std::string PostgreSQLSubscriptionRepository::subscriptionStatusToString(SubscriptionStatus status) const {
    switch (status) {
        case SubscriptionStatus::ACTIVE: return "ACTIVE";
        case SubscriptionStatus::SUSPENDED: return "SUSPENDED";
        case SubscriptionStatus::EXPIRED: return "EXPIRED";
        case SubscriptionStatus::CANCELLED: return "CANCELLED";
        default: return "ACTIVE";
    }
}

SubscriptionStatus PostgreSQLSubscriptionRepository::stringToSubscriptionStatus(const std::string& status) const {
    if (status == "SUSPENDED") return SubscriptionStatus::SUSPENDED;
    if (status == "EXPIRED") return SubscriptionStatus::EXPIRED;
    if (status == "CANCELLED") return SubscriptionStatus::CANCELLED;
    return SubscriptionStatus::ACTIVE;
}
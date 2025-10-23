#ifndef SUBSCRIPTION_HPP
#define SUBSCRIPTION_HPP

#include "../types/uuid.hpp"
#include "../types/enums.hpp"
#include <chrono>

class Subscription {
private:
    UUID id_;
    UUID clientId_;
    UUID subscriptionTypeId_;
    std::chrono::system_clock::time_point startDate_;
    std::chrono::system_clock::time_point endDate_;
    int remainingVisits_;
    SubscriptionStatus status_;
    std::chrono::system_clock::time_point purchaseDate_;

public:
    Subscription();
    Subscription(const UUID& id, const UUID& clientId, const UUID& subscriptionTypeId,
                 const std::chrono::system_clock::time_point& startDate,
                 const std::chrono::system_clock::time_point& endDate,
                 int remainingVisits);
    
    // Геттеры
    UUID getId() const;
    UUID getClientId() const;
    UUID getSubscriptionTypeId() const;
    std::chrono::system_clock::time_point getStartDate() const;
    std::chrono::system_clock::time_point getEndDate() const;
    int getRemainingVisits() const;
    SubscriptionStatus getStatus() const;
    std::chrono::system_clock::time_point getPurchaseDate() const;
    
    // Бизнес-логика
    bool useVisit();
    bool canUseVisit() const;
    bool isActive() const;
    bool isExpired() const;
    void suspend();
    void activate();
    void cancel();
    bool isValid() const;
    
    // Валидация
    static bool isValidDuration(const std::chrono::system_clock::time_point& startDate,
                               const std::chrono::system_clock::time_point& endDate);
    static bool isValidRemainingVisits(int visits);
    
    // Операторы сравнения
    bool operator==(const Subscription& other) const;
    bool operator!=(const Subscription& other) const;
};

#endif // SUBSCRIPTION_HPP
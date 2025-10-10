#include "Subscription.hpp"
#include <chrono>
#include <stdexcept>

Subscription::Subscription() 
    : id_(UUID()), clientId_(UUID()), subscriptionTypeId_(UUID()),
      startDate_(std::chrono::system_clock::now()),
      endDate_(std::chrono::system_clock::now() + std::chrono::hours(24 * 30)), // 30 дней по умолчанию
      remainingVisits_(0), status_(SubscriptionStatus::ACTIVE),
      purchaseDate_(std::chrono::system_clock::now()) {}

Subscription::Subscription(const UUID& id, const UUID& clientId, const UUID& subscriptionTypeId,
                           const std::chrono::system_clock::time_point& startDate,
                           const std::chrono::system_clock::time_point& endDate,
                           int remainingVisits)
    : id_(id), clientId_(clientId), subscriptionTypeId_(subscriptionTypeId),
      startDate_(startDate), endDate_(endDate), remainingVisits_(remainingVisits),
      status_(SubscriptionStatus::ACTIVE),
      purchaseDate_(std::chrono::system_clock::now()) {
    
    if (!isValid()) {
        throw std::invalid_argument("Invalid subscription data");
    }
}

UUID Subscription::getId() const { return id_; }
UUID Subscription::getClientId() const { return clientId_; }
UUID Subscription::getSubscriptionTypeId() const { return subscriptionTypeId_; }
std::chrono::system_clock::time_point Subscription::getStartDate() const { return startDate_; }
std::chrono::system_clock::time_point Subscription::getEndDate() const { return endDate_; }
int Subscription::getRemainingVisits() const { return remainingVisits_; }
SubscriptionStatus Subscription::getStatus() const { return status_; }
std::chrono::system_clock::time_point Subscription::getPurchaseDate() const { return purchaseDate_; }

bool Subscription::useVisit() {
    if (!canUseVisit()) {
        return false;
    }
    
    // Для безлимитных подписок не уменьшаем количество посещений
    if (remainingVisits_ > 0) {
        remainingVisits_--;
        
        // Если визиты закончились, помечаем как истекший
        if (remainingVisits_ <= 0) {
            status_ = SubscriptionStatus::EXPIRED;
        }
    }
    // Для remainingVisits_ == -1 (безлимит) ничего не делаем
    
    return true;
}

bool Subscription::canUseVisit() const {
    return isActive() && (remainingVisits_ == -1 || remainingVisits_ > 0);
}

bool Subscription::isActive() const {
    auto now = std::chrono::system_clock::now();
    return status_ == SubscriptionStatus::ACTIVE && 
           now >= startDate_ && now <= endDate_ &&
           (remainingVisits_ == -1 || remainingVisits_ > 0);
}

bool Subscription::isExpired() const {
    auto now = std::chrono::system_clock::now();
    return now > endDate_ || (remainingVisits_ == 0);
}

void Subscription::suspend() {
    if (status_ != SubscriptionStatus::ACTIVE) {
        throw std::logic_error("Only active subscriptions can be suspended");
    }
    status_ = SubscriptionStatus::SUSPENDED;
}

void Subscription::activate() {
    if (status_ != SubscriptionStatus::SUSPENDED) {
        throw std::logic_error("Only suspended subscriptions can be activated");
    }
    status_ = SubscriptionStatus::ACTIVE;
}

void Subscription::cancel() {
    status_ = SubscriptionStatus::CANCELLED;
}

bool Subscription::isValid() const {
    return !id_.isNull() && id_.isValid() &&
           !clientId_.isNull() && clientId_.isValid() &&
           !subscriptionTypeId_.isNull() && subscriptionTypeId_.isValid() &&
           isValidDuration(startDate_, endDate_) &&
           isValidRemainingVisits(remainingVisits_) &&
           purchaseDate_ <= std::chrono::system_clock::now();
}

bool Subscription::isValidDuration(const std::chrono::system_clock::time_point& startDate,
                                  const std::chrono::system_clock::time_point& endDate) {
    return startDate < endDate;
}

bool Subscription::isValidRemainingVisits(int visits) {
    return visits >= -1; // -1 означает безлимит
}

bool Subscription::operator==(const Subscription& other) const {
    return id_ == other.id_ && 
           clientId_ == other.clientId_ && 
           subscriptionTypeId_ == other.subscriptionTypeId_ && 
           startDate_ == other.startDate_ && 
           endDate_ == other.endDate_;
}

bool Subscription::operator!=(const Subscription& other) const {
    return !(*this == other);
}
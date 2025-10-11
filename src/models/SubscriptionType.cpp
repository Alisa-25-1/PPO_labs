#include "SubscriptionType.hpp"
#include <regex>
#include <stdexcept>

SubscriptionType::SubscriptionType() 
    : id_(UUID()), name_(""), description_(""), validityDays_(30),
      visitCount_(0), unlimited_(false), price_(0.0) {}

SubscriptionType::SubscriptionType(const UUID& id, const std::string& name, int validityDays, 
                                   int visitCount, bool unlimited, double price)
    : id_(id), name_(name), description_(""), validityDays_(validityDays),
      visitCount_(visitCount), unlimited_(unlimited), price_(price) {
    
    if (!isValid()) {
        throw std::invalid_argument("Invalid subscription type data");
    }
}

UUID SubscriptionType::getId() const { return id_; }
std::string SubscriptionType::getName() const { return name_; }
std::string SubscriptionType::getDescription() const { return description_; }
int SubscriptionType::getValidityDays() const { return validityDays_; }
int SubscriptionType::getVisitCount() const { return visitCount_; }
bool SubscriptionType::isUnlimited() const { return unlimited_; }  // Одна реализация
double SubscriptionType::getPrice() const { return price_; }

void SubscriptionType::setDescription(const std::string& description) {
    if (!isValidDescription(description)) {
        throw std::invalid_argument("Invalid description");
    }
    description_ = description;
}

bool SubscriptionType::isValidForNewSubscription() const {
    return isValid() && price_ >= 0;
}

int SubscriptionType::calculateRemainingVisits() const {
    return unlimited_ ? -1 : visitCount_; // -1 означает безлимит
}

bool SubscriptionType::isValid() const {
    return !id_.isNull() && id_.isValid() &&
           isValidName(name_) &&
           isValidDescription(description_) &&
           isValidValidityDays(validityDays_) &&
           isValidVisitCount(visitCount_) &&
           isValidPrice(price_);
}

bool SubscriptionType::isValidName(const std::string& name) {
    return !name.empty() && name.length() <= 100 && name.length() >= 2;
}

bool SubscriptionType::isValidDescription(const std::string& description) {
    return description.length() <= 500;
}

bool SubscriptionType::isValidValidityDays(int days) {
    return days > 0 && days <= 365; // Максимум 1 год
}

bool SubscriptionType::isValidVisitCount(int count) {
    return count >= 0 && count <= 1000;
}

bool SubscriptionType::isValidPrice(double price) {
    return price >= 0.0;
}

bool SubscriptionType::operator==(const SubscriptionType& other) const {
    return id_ == other.id_ && 
           name_ == other.name_ && 
           validityDays_ == other.validityDays_ && 
           visitCount_ == other.visitCount_ && 
           unlimited_ == other.unlimited_ && 
           price_ == other.price_;
}

bool SubscriptionType::operator!=(const SubscriptionType& other) const {
    return !(*this == other);
}
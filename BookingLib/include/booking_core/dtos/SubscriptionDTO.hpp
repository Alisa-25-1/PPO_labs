#ifndef SUBSCRIPTIONDTO_HPP
#define SUBSCRIPTIONDTO_HPP

#include "../types/uuid.hpp"
#include "../models/Subscription.hpp"
#include "../models/SubscriptionType.hpp"
#include <string>
#include <iomanip>

struct SubscriptionRequestDTO {
    UUID clientId;
    UUID subscriptionTypeId;
    
    bool validate() const {
        return !clientId.isNull() && clientId.isValid() &&
               !subscriptionTypeId.isNull() && subscriptionTypeId.isValid();
    }
};

struct SubscriptionResponseDTO {
    UUID subscriptionId;
    UUID clientId;
    UUID subscriptionTypeId;
    std::string startDate;
    std::string endDate;
    int remainingVisits;
    std::string status;
    std::string purchaseDate;
    
    SubscriptionResponseDTO(const Subscription& subscription) {
        subscriptionId = subscription.getId();
        clientId = subscription.getClientId();
        subscriptionTypeId = subscription.getSubscriptionTypeId();
        
        // Format dates
        auto time_t_start = std::chrono::system_clock::to_time_t(subscription.getStartDate());
        std::tm tm_start = *std::localtime(&time_t_start);
        std::ostringstream oss_start;
        oss_start << std::put_time(&tm_start, "%Y-%m-%d");
        startDate = oss_start.str();
        
        auto time_t_end = std::chrono::system_clock::to_time_t(subscription.getEndDate());
        std::tm tm_end = *std::localtime(&time_t_end);
        std::ostringstream oss_end;
        oss_end << std::put_time(&tm_end, "%Y-%m-%d");
        endDate = oss_end.str();
        
        remainingVisits = subscription.getRemainingVisits();
        
        switch (subscription.getStatus()) {
            case SubscriptionStatus::ACTIVE:
                status = "ACTIVE";
                break;
            case SubscriptionStatus::SUSPENDED:
                status = "SUSPENDED";
                break;
            case SubscriptionStatus::EXPIRED:
                status = "EXPIRED";
                break;
            case SubscriptionStatus::CANCELLED:
                status = "CANCELLED";
                break;
            default:
                status = "UNKNOWN";
                break;
        }
        
        auto time_t_purchase = std::chrono::system_clock::to_time_t(subscription.getPurchaseDate());
        std::tm tm_purchase = *std::localtime(&time_t_purchase);
        std::ostringstream oss_purchase;
        oss_purchase << std::put_time(&tm_purchase, "%Y-%m-%d %H:%M:%S");
        purchaseDate = oss_purchase.str();
    }
};

struct SubscriptionTypeResponseDTO {
    UUID subscriptionTypeId;
    std::string name;
    std::string description;
    int validityDays;
    int visitCount;
    bool unlimited;
    double price;
    
    SubscriptionTypeResponseDTO(const SubscriptionType& subscriptionType) {
        subscriptionTypeId = subscriptionType.getId();
        name = subscriptionType.getName();
        description = subscriptionType.getDescription();
        validityDays = subscriptionType.getValidityDays();
        visitCount = subscriptionType.getVisitCount();
        unlimited = subscriptionType.isUnlimited();
        price = subscriptionType.getPrice();
    }
};

#endif // SUBSCRIPTIONDTO_HPP
#pragma once

#include "../../services/SubscriptionService.hpp"
#include "../../dtos/SubscriptionDTO.hpp"
#include <memory>
#include <vector>

class SubscriptionController {
private:
    std::shared_ptr<SubscriptionService> subscriptionService_;

public:
    SubscriptionController(std::shared_ptr<SubscriptionService> subscriptionService);
    
    // Основные методы для работы с абонементами
    SubscriptionResponseDTO purchaseSubscription(const SubscriptionRequestDTO& request);
    SubscriptionResponseDTO cancelSubscription(const UUID& subscriptionId, const UUID& clientId);
    std::vector<SubscriptionResponseDTO> getClientSubscriptions(const UUID& clientId);
    std::vector<SubscriptionTypeResponseDTO> getAvailableSubscriptionTypes();
    
    // Вспомогательные методы
    bool canClientUseSubscription(const UUID& clientId) const;
    int getClientRemainingVisits(const UUID& clientId) const;
    
private:
    void validateClientOwnership(const UUID& subscriptionId, const UUID& clientId) const;
};
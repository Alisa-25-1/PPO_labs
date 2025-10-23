#pragma once
#include "../repositories/ISubscriptionRepository.hpp"
#include "../repositories/ISubscriptionTypeRepository.hpp"
#include "../repositories/IClientRepository.hpp"
#include "../dtos/SubscriptionDTO.hpp"
#include "../types/uuid.hpp"
#include "exceptions/ValidationException.hpp"
#include <memory>
#include <vector>

class SubscriptionService {
private:
    std::shared_ptr<ISubscriptionRepository> subscriptionRepository_;
    std::shared_ptr<ISubscriptionTypeRepository> subscriptionTypeRepository_;
    std::shared_ptr<IClientRepository> clientRepository_;

    void validateSubscriptionRequest(const SubscriptionRequestDTO& request) const;
    void validateClient(const UUID& clientId) const;
    bool hasActiveSubscription(const UUID& clientId) const;

public:
    SubscriptionService(
        std::shared_ptr<ISubscriptionRepository> subscriptionRepo,
        std::shared_ptr<ISubscriptionTypeRepository> subscriptionTypeRepo,
        std::shared_ptr<IClientRepository> clientRepo
    );

    SubscriptionResponseDTO purchaseSubscription(const SubscriptionRequestDTO& request);
    SubscriptionResponseDTO renewSubscription(const UUID& subscriptionId);
    SubscriptionResponseDTO cancelSubscription(const UUID& subscriptionId);
    std::vector<SubscriptionResponseDTO> getClientSubscriptions(const UUID& clientId);
    std::vector<SubscriptionTypeResponseDTO> getAvailableSubscriptionTypes();
    bool canUseSubscription(const UUID& clientId) const;
    int getRemainingVisits(const UUID& clientId) const;
};
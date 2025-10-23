#include "SubscriptionService.hpp"
#include <algorithm>

SubscriptionService::SubscriptionService(
    std::shared_ptr<ISubscriptionRepository> subscriptionRepo,
    std::shared_ptr<ISubscriptionTypeRepository> subscriptionTypeRepo,
    std::shared_ptr<IClientRepository> clientRepo
) : subscriptionRepository_(std::move(subscriptionRepo)),
    subscriptionTypeRepository_(std::move(subscriptionTypeRepo)),
    clientRepository_(std::move(clientRepo)) {}

void SubscriptionService::validateSubscriptionRequest(const SubscriptionRequestDTO& request) const {
    if (!request.validate()) {
        throw ValidationException("Invalid subscription request data");
    }
    
    validateClient(request.clientId);
    
    auto subscriptionType = subscriptionTypeRepository_->findById(request.subscriptionTypeId);
    if (!subscriptionType) {
        throw ValidationException("Subscription type not found");
    }
    
    if (!subscriptionType->isValidForNewSubscription()) {
        throw BusinessRuleException("Subscription type is not available for purchase");
    }
}

void SubscriptionService::validateClient(const UUID& clientId) const {
    auto client = clientRepository_->findById(clientId);
    if (!client) {
        throw ValidationException("Client not found");
    }
    
    if (!client->isActive()) {
        throw BusinessRuleException("Client account is not active");
    }
}

bool SubscriptionService::hasActiveSubscription(const UUID& clientId) const {
    auto subscriptions = subscriptionRepository_->findByClientId(clientId);
    
    return std::any_of(subscriptions.begin(), subscriptions.end(),
        [](const Subscription& subscription) {
            return subscription.isActive();
        });
}

SubscriptionResponseDTO SubscriptionService::purchaseSubscription(const SubscriptionRequestDTO& request) {
    validateSubscriptionRequest(request);
    
    // Проверяем, что у клиента нет активной подписки
    if (hasActiveSubscription(request.clientId)) {
        throw BusinessRuleException("Client already has an active subscription");
    }
    
    auto subscriptionType = subscriptionTypeRepository_->findById(request.subscriptionTypeId);
    if (!subscriptionType) {
        throw std::runtime_error("Subscription type not found");
    }
    
    // Создаем подписку
    UUID newId = UUID::generate();
    auto startDate = std::chrono::system_clock::now();
    auto endDate = startDate + std::chrono::hours(24 * subscriptionType->getValidityDays());
    
    // Для безлимитных подписок устанавливаем -1, иначе количество посещений
    int remainingVisits = subscriptionType->isUnlimited() ? -1 : subscriptionType->getVisitCount();
    
    Subscription subscription(newId, request.clientId, request.subscriptionTypeId,
                             startDate, endDate, remainingVisits);
    
    if (!subscriptionRepository_->save(subscription)) {
        throw std::runtime_error("Failed to save subscription");
    }
    
    return SubscriptionResponseDTO(subscription);
}

SubscriptionResponseDTO SubscriptionService::renewSubscription(const UUID& subscriptionId) {
    auto subscription = subscriptionRepository_->findById(subscriptionId);
    if (!subscription) {
        throw std::runtime_error("Subscription not found");
    }
    
    if (!subscription->isExpired()) {
        throw BusinessRuleException("Only expired subscriptions can be renewed");
    }
    
    auto subscriptionType = subscriptionTypeRepository_->findById(subscription->getSubscriptionTypeId());
    if (!subscriptionType) {
        throw std::runtime_error("Subscription type not found");
    }
    
    // Обновляем даты и количество посещений
    auto startDate = std::chrono::system_clock::now();
    auto endDate = startDate + std::chrono::hours(24 * subscriptionType->getValidityDays());
    int remainingVisits = subscriptionType->calculateRemainingVisits();
    
    // Создаем новую подписку (или обновляем существующую - зависит от бизнес-логики)
    Subscription renewedSubscription(subscriptionId, subscription->getClientId(), 
                                    subscription->getSubscriptionTypeId(), startDate, 
                                    endDate, remainingVisits);
    
    if (!subscriptionRepository_->update(renewedSubscription)) {
        throw std::runtime_error("Failed to renew subscription");
    }
    
    return SubscriptionResponseDTO(renewedSubscription);
}

SubscriptionResponseDTO SubscriptionService::cancelSubscription(const UUID& subscriptionId) {
    auto subscription = subscriptionRepository_->findById(subscriptionId);
    if (!subscription) {
        throw std::runtime_error("Subscription not found");
    }
    
    if (subscription->isExpired()) {
        throw BusinessRuleException("Cannot cancel expired subscription");
    }
    
    subscription->cancel();
    
    if (!subscriptionRepository_->update(*subscription)) {
        throw std::runtime_error("Failed to cancel subscription");
    }
    
    return SubscriptionResponseDTO(*subscription);
}

std::vector<SubscriptionResponseDTO> SubscriptionService::getClientSubscriptions(const UUID& clientId) {
    validateClient(clientId);
    
    auto subscriptions = subscriptionRepository_->findByClientId(clientId);
    std::vector<SubscriptionResponseDTO> result;
    
    for (const auto& subscription : subscriptions) {
        result.push_back(SubscriptionResponseDTO(subscription));
    }
    
    return result;
}

std::vector<SubscriptionTypeResponseDTO> SubscriptionService::getAvailableSubscriptionTypes() {
    auto subscriptionTypes = subscriptionTypeRepository_->findAllActive();
    std::vector<SubscriptionTypeResponseDTO> result;
    
    for (const auto& subscriptionType : subscriptionTypes) {
        result.push_back(SubscriptionTypeResponseDTO(subscriptionType));
    }
    
    return result;
}

bool SubscriptionService::canUseSubscription(const UUID& clientId) const {
    auto subscriptions = subscriptionRepository_->findByClientId(clientId);
    
    for (const auto& subscription : subscriptions) {
        if (subscription.isActive()) {
            // Получаем тип подписки для проверки безлимитности
            auto subscriptionType = subscriptionTypeRepository_->findById(subscription.getSubscriptionTypeId());
            if (subscriptionType && subscriptionType->isUnlimited()) {
                return true; // Безлимитная подписка всегда может быть использована
            }
            if (subscription.getRemainingVisits() > 0) {
                return true; // Есть остаточные посещения
            }
        }
    }
    
    return false;
}

int SubscriptionService::getRemainingVisits(const UUID& clientId) const {
    auto subscriptions = subscriptionRepository_->findByClientId(clientId);
    int totalRemaining = 0;
    bool hasUnlimited = false;

    for (const auto& subscription : subscriptions) {
        if (subscription.isActive()) {
            // Получаем тип подписки для проверки безлимитности
            auto subscriptionType = subscriptionTypeRepository_->findById(subscription.getSubscriptionTypeId());
            if (subscriptionType && subscriptionType->isUnlimited()) {
                hasUnlimited = true;
                break; // Если нашли безлимитную подписку, можно прервать цикл
            }
            totalRemaining += subscription.getRemainingVisits();
        }
    }

    return hasUnlimited ? -1 : totalRemaining; // -1 означает безлимит
}
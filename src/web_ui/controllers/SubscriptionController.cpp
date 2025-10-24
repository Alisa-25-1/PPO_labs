#include "SubscriptionController.hpp"
#include <iostream>

SubscriptionController::SubscriptionController(std::shared_ptr<SubscriptionService> subscriptionService)
    : subscriptionService_(std::move(subscriptionService)) {
    std::cout << "✅ SubscriptionController создан" << std::endl;
}

SubscriptionResponseDTO SubscriptionController::purchaseSubscription(const SubscriptionRequestDTO& request) {
    try {
        std::cout << "🛒 Покупка абонемента для клиента: " << request.clientId.toString() << std::endl;
        return subscriptionService_->purchaseSubscription(request);
    } catch (const std::exception& e) {
        std::cerr << "❌ Ошибка покупки абонемента: " << e.what() << std::endl;
        throw;
    }
}

SubscriptionResponseDTO SubscriptionController::cancelSubscription(const UUID& subscriptionId, const UUID& clientId) {
    try {
        std::cout << "❌ Отмена абонемента: " << subscriptionId.toString() << " для клиента: " << clientId.toString() << std::endl;
        validateClientOwnership(subscriptionId, clientId);
        return subscriptionService_->cancelSubscription(subscriptionId);
    } catch (const std::exception& e) {
        std::cerr << "❌ Ошибка отмены абонемента: " << e.what() << std::endl;
        throw;
    }
}

std::vector<SubscriptionResponseDTO> SubscriptionController::getClientSubscriptions(const UUID& clientId) {
    try {
        std::cout << "📋 Получение абонементов клиента: " << clientId.toString() << std::endl;
        return subscriptionService_->getClientSubscriptions(clientId);
    } catch (const std::exception& e) {
        std::cerr << "❌ Ошибка получения абонементов: " << e.what() << std::endl;
        return {};
    }
}

std::vector<SubscriptionTypeResponseDTO> SubscriptionController::getAvailableSubscriptionTypes() {
    try {
        std::cout << "🎫 Получение доступных типов абонементов" << std::endl;
        return subscriptionService_->getAvailableSubscriptionTypes();
    } catch (const std::exception& e) {
        std::cerr << "❌ Ошибка получения типов абонементов: " << e.what() << std::endl;
        return {};
    }
}

bool SubscriptionController::canClientUseSubscription(const UUID& clientId) const {
    try {
        return subscriptionService_->canUseSubscription(clientId);
    } catch (const std::exception& e) {
        std::cerr << "❌ Ошибка проверки доступности абонемента: " << e.what() << std::endl;
        return false;
    }
}

int SubscriptionController::getClientRemainingVisits(const UUID& clientId) const {
    try {
        return subscriptionService_->getRemainingVisits(clientId);
    } catch (const std::exception& e) {
        std::cerr << "❌ Ошибка получения остатка посещений: " << e.what() << std::endl;
        return 0;
    }
}

void SubscriptionController::validateClientOwnership(const UUID& subscriptionId, const UUID& clientId) const {
    auto subscriptions = subscriptionService_->getClientSubscriptions(clientId);
    bool ownsSubscription = false;
    
    for (const auto& subscription : subscriptions) {
        if (subscription.subscriptionId == subscriptionId) {
            ownsSubscription = true;
            break;
        }
    }
    
    if (!ownsSubscription) {
        throw std::runtime_error("Client does not own this subscription");
    }
}
#pragma once

#include <Wt/WContainerWidget.h>
#include <Wt/WTable.h>
#include <Wt/WPushButton.h>
#include <Wt/WText.h>
#include "../../types/uuid.hpp"
#include "../../dtos/SubscriptionDTO.hpp"

class WebApplication;

class SubscriptionListWidget : public Wt::WContainerWidget {
public:
    SubscriptionListWidget(WebApplication* app);
    void loadSubscriptions();
    
private:
    WebApplication* app_;
    Wt::WTable* subscriptionsTable_;
    Wt::WText* statusText_;

    void setupUI();
    void handleCancelSubscription(const UUID& subscriptionId);
    void cancelSubscription(const UUID& subscriptionId); // ИСПРАВЛЕНИЕ: переименовано
    void updateStatus(const std::string& message, bool isError = false);
    
    // Вспомогательные методы для работы с бизнес-логикой
    std::vector<SubscriptionResponseDTO> getClientSubscriptionsFromService();
    bool cancelSubscriptionThroughService(const UUID& subscriptionId);
    UUID getCurrentClientId();
    std::string formatDate(const std::string& dateStr);
    std::string getStatusDisplayName(const std::string& status);
    std::string getStatusStyleClass(const std::string& status);
};
#pragma once

#include <Wt/WContainerWidget.h>
#include <Wt/WComboBox.h>
#include <Wt/WPushButton.h>
#include <Wt/WText.h>
#include "../../types/uuid.hpp"
#include "../../dtos/SubscriptionDTO.hpp"

class WebApplication;
class SubscriptionView;

class PurchaseSubscriptionWidget : public Wt::WContainerWidget {
public:
    PurchaseSubscriptionWidget(WebApplication* app);
    void loadSubscriptionTypes();

    void setBackToListCallback(std::function<void()> callback);
    
private:
    WebApplication* app_;
    std::function<void()> onBackToList_;
    
    Wt::WComboBox* subscriptionTypeComboBox_;
    Wt::WPushButton* purchaseButton_;
    Wt::WPushButton* backButton_;
    Wt::WText* statusText_;
    Wt::WText* detailsText_;

    void setupUI();
    void handlePurchase();
    void handleBack();
    void updateSubscriptionDetails();
    void updateStatus(const std::string& message, bool isError = false);
    
    // Вспомогательные методы для работы с бизнес-логикой
    std::vector<SubscriptionTypeResponseDTO> getAvailableSubscriptionTypesFromService();
    SubscriptionResponseDTO purchaseSubscriptionThroughService(const SubscriptionRequestDTO& request);
    UUID getCurrentClientId();
};
#pragma once

#include <Wt/WContainerWidget.h>
#include <Wt/WStackedWidget.h>

class WebApplication;
class SubscriptionListWidget;
class PurchaseSubscriptionWidget;

class SubscriptionView : public Wt::WContainerWidget {
public:
    SubscriptionView(WebApplication* app);
    
private:
    WebApplication* app_;
    Wt::WStackedWidget* contentStack_;
    
    // Виджеты для разных состояний
    Wt::WContainerWidget* menuWidget_;
    SubscriptionListWidget* listWidget_;
    PurchaseSubscriptionWidget* purchaseWidget_;

    void setupUI();
    void showSubscriptionMenu();
    void showMySubscriptions();
    void showPurchaseSubscription();
};
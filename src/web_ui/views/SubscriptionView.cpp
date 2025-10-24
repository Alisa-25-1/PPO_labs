#include "SubscriptionView.hpp"
#include "../WebApplication.hpp"
#include "SubscriptionListWidget.hpp"
#include "PurchaseSubscriptionWidget.hpp"
#include <Wt/WPushButton.h>
#include <Wt/WText.h>
#include <iostream>

SubscriptionView::SubscriptionView(WebApplication* app) 
    : app_(app),
      contentStack_(nullptr),
      menuWidget_(nullptr),
      listWidget_(nullptr),
      purchaseWidget_(nullptr) {
    
    std::cout << "🔧 Создание SubscriptionView..." << std::endl;
    setupUI();
    std::cout << "✅ SubscriptionView создан" << std::endl;
}

void SubscriptionView::setupUI() {
    std::cout << "🔧 Настройка UI SubscriptionView..." << std::endl;
    
    // Устанавливаем основной класс для всего view
    setStyleClass("subscription-view");
    
    // Заголовок
    auto header = addNew<Wt::WContainerWidget>();
    header->setStyleClass("subscription-header");
    auto title = header->addNew<Wt::WText>("<h1>🎫 Абонементы</h1>");
    title->setTextFormat(Wt::TextFormat::UnsafeXHTML);
    
    // Навигационное меню
    auto nav = addNew<Wt::WContainerWidget>();
    nav->setStyleClass("subscription-nav");
    
    auto purchaseBtn = nav->addNew<Wt::WPushButton>("🛒 Купить абонемент");
    purchaseBtn->setStyleClass("btn-nav");
    purchaseBtn->clicked().connect(this, &SubscriptionView::showPurchaseSubscription);
    
    auto listBtn = nav->addNew<Wt::WPushButton>("📋 Мои абонементы");
    listBtn->setStyleClass("btn-nav");
    listBtn->clicked().connect(this, &SubscriptionView::showMySubscriptions);
    
    auto backBtn = nav->addNew<Wt::WPushButton>("← Назад в меню");
    backBtn->setStyleClass("btn-nav btn-back");
    backBtn->clicked().connect([this]() {
        app_->showDashboard();
    });
    
    // Основной контент - центрирующий контейнер
    auto contentContainer = addNew<Wt::WContainerWidget>();
    contentContainer->setStyleClass("subscription-content");
    
    // Стек для контента внутри центрирующего контейнера
    contentStack_ = contentContainer->addNew<Wt::WStackedWidget>();
    contentStack_->setStyleClass("subscription-content-stack");
    
    std::cout << "🔧 Создание виджетов SubscriptionView..." << std::endl;
    
    // Создаем виджеты заранее
    menuWidget_ = contentStack_->addNew<Wt::WContainerWidget>();
    listWidget_ = contentStack_->addNew<SubscriptionListWidget>(app_);
    purchaseWidget_ = contentStack_->addNew<PurchaseSubscriptionWidget>(app_);

    // Настраиваем колбэк для возврата к списку
    purchaseWidget_->setBackToListCallback([this]() {
        showMySubscriptions();
    });
    
    std::cout << "🔧 Настройка меню SubscriptionView..." << std::endl;
    
    // Настраиваем меню
    menuWidget_->setStyleClass("subscription-welcome");
    auto welcomeTitle = menuWidget_->addNew<Wt::WText>("<h2>Добро пожаловать в управление абонементами!</h2>");
    welcomeTitle->setTextFormat(Wt::TextFormat::UnsafeXHTML);
    auto welcomeText = menuWidget_->addNew<Wt::WText>("<p>Выберите действие из меню выше</p>");
    welcomeText->setTextFormat(Wt::TextFormat::UnsafeXHTML);
    
    // Показываем меню по умолчанию
    showSubscriptionMenu();
    
    std::cout << "✅ UI SubscriptionView настроен" << std::endl;
}

void SubscriptionView::showSubscriptionMenu() {
    std::cout << "🔄 Показываем меню абонементов" << std::endl;
    if (contentStack_ && menuWidget_) {
        contentStack_->setCurrentWidget(menuWidget_);
    }
}

void SubscriptionView::showMySubscriptions() {
    std::cout << "🔄 Показываем список абонементов" << std::endl;
    if (contentStack_ && listWidget_) {
        listWidget_->loadSubscriptions();
        contentStack_->setCurrentWidget(listWidget_);
    }
}

void SubscriptionView::showPurchaseSubscription() {
    std::cout << "🔄 Показываем покупку абонемента" << std::endl;
    if (contentStack_ && purchaseWidget_) {
        purchaseWidget_->loadSubscriptionTypes();
        contentStack_->setCurrentWidget(purchaseWidget_);
    }
}
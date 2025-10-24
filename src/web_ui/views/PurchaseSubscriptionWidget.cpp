#include "PurchaseSubscriptionWidget.hpp"
#include "../WebApplication.hpp"
#include <Wt/WBreak.h>
#include <iostream>

PurchaseSubscriptionWidget::PurchaseSubscriptionWidget(WebApplication* app) 
    : app_(app),
      subscriptionTypeComboBox_(nullptr),
      purchaseButton_(nullptr),
      backButton_(nullptr),
      statusText_(nullptr),
      detailsText_(nullptr) {
    
    std::cout << "🔧 Создание PurchaseSubscriptionWidget..." << std::endl;
    setupUI();
    std::cout << "✅ PurchaseSubscriptionWidget создан" << std::endl;
}

void PurchaseSubscriptionWidget::setupUI() {
    std::cout << "🔧 Настройка UI PurchaseSubscriptionWidget..." << std::endl;
    
    setStyleClass("purchase-subscription-widget");
    
    auto card = addNew<Wt::WContainerWidget>();
    card->setStyleClass("subscription-card");
    
    // Заголовок
    auto header = card->addNew<Wt::WContainerWidget>();
    header->setStyleClass("subscription-card-header");
    auto headerText = header->addNew<Wt::WText>("<h2>🛒 Купить абонемент</h2>");
    headerText->setTextFormat(Wt::TextFormat::UnsafeXHTML);
    
    // Форма
    auto form = card->addNew<Wt::WContainerWidget>();
    form->setStyleClass("purchase-subscription-form");
    
    // Выбор типа абонемента
    auto typeGroup = form->addNew<Wt::WContainerWidget>();
    typeGroup->setStyleClass("form-group");
    
    auto typeLabel = typeGroup->addNew<Wt::WText>("<label class='form-label'>🎫 Выберите тип абонемента</label>");
    typeLabel->setTextFormat(Wt::TextFormat::UnsafeXHTML);
    
    subscriptionTypeComboBox_ = typeGroup->addNew<Wt::WComboBox>();
    subscriptionTypeComboBox_->setStyleClass("form-input");
    subscriptionTypeComboBox_->changed().connect(this, &PurchaseSubscriptionWidget::updateSubscriptionDetails);
    
    // Детали выбранного абонемента
    auto detailsGroup = form->addNew<Wt::WContainerWidget>();
    detailsGroup->setStyleClass("subscription-details");
    
    detailsText_ = detailsGroup->addNew<Wt::WText>();
    detailsText_->setStyleClass("subscription-details-text");
    
    // Кнопки
    auto buttonGroup = form->addNew<Wt::WContainerWidget>();
    buttonGroup->setStyleClass("purchase-subscription-button-group");
    
    purchaseButton_ = buttonGroup->addNew<Wt::WPushButton>("💳 КУПИТЬ АБОНЕМЕНТ");
    purchaseButton_->setStyleClass("btn-purchase-subscription");
    purchaseButton_->clicked().connect(this, &PurchaseSubscriptionWidget::handlePurchase);
    
    backButton_ = buttonGroup->addNew<Wt::WPushButton>("← Назад к списку");
    backButton_->setStyleClass("btn-back");
    backButton_->clicked().connect(this, &PurchaseSubscriptionWidget::handleBack);
    
    // Статус
    statusText_ = form->addNew<Wt::WText>();
    statusText_->setStyleClass("subscription-status");
    
    // Загружаем доступные типы абонементов
    loadSubscriptionTypes();
    
    std::cout << "✅ UI PurchaseSubscriptionWidget настроен" << std::endl;
}

void PurchaseSubscriptionWidget::loadSubscriptionTypes() {
    try {
        auto subscriptionTypes = getAvailableSubscriptionTypesFromService();
        
        subscriptionTypeComboBox_->clear();
        subscriptionTypeComboBox_->addItem("-- Выберите тип абонемента --");
        
        for (const auto& type : subscriptionTypes) {
            std::string typeInfo = type.name + " - " + std::to_string(static_cast<int>(type.price)) + " руб.";
            subscriptionTypeComboBox_->addItem(typeInfo);
        }
        
        if (subscriptionTypes.empty()) {
            updateStatus("❌ Нет доступных типов абонементов", true);
        } else {
            updateStatus("✅ Загружено типов абонементов: " + std::to_string(subscriptionTypes.size()), false);
        }
        
    } catch (const std::exception& e) {
        std::cerr << "Ошибка загрузки типов абонементов: " << e.what() << std::endl;
        updateStatus("❌ Ошибка загрузки типов абонементов", true);
    }
}

void PurchaseSubscriptionWidget::handlePurchase() {
    try {
        // Проверяем обязательные поля
        if (subscriptionTypeComboBox_->currentIndex() <= 0) {
            updateStatus("❌ Пожалуйста, выберите тип абонемента", true);
            return;
        }
        
        // Получаем выбранный тип абонемента
        int typeIndex = subscriptionTypeComboBox_->currentIndex() - 1;
        auto subscriptionTypes = getAvailableSubscriptionTypesFromService();
        
        if (typeIndex < 0 || typeIndex >= static_cast<int>(subscriptionTypes.size())) {
            updateStatus("❌ Ошибка: неверный выбор типа абонемента", true);
            return;
        }
        
        UUID subscriptionTypeId = subscriptionTypes[typeIndex].subscriptionTypeId;
        
        // Создаем DTO запроса
        SubscriptionRequestDTO request;
        request.clientId = getCurrentClientId();
        request.subscriptionTypeId = subscriptionTypeId;
        
        // Вызываем сервис для покупки абонемента
        auto response = purchaseSubscriptionThroughService(request);
        
        updateStatus("✅ Абонемент успешно приобретен! ID: " + response.subscriptionId.toString(), false);
        
        // Очищаем форму
        subscriptionTypeComboBox_->setCurrentIndex(0);
        detailsText_->setText("");
        
    } catch (const std::exception& e) {
        updateStatus("❌ Ошибка при покупке абонемента: " + std::string(e.what()), true);
    }
}

void PurchaseSubscriptionWidget::handleBack() {
    // Вызываем колбэк для возврата к списку бронирований
    if (onBackToList_) {
        onBackToList_();
    }
}

void PurchaseSubscriptionWidget::setBackToListCallback(std::function<void()> callback) {
    onBackToList_ = std::move(callback);
}

void PurchaseSubscriptionWidget::updateSubscriptionDetails() {
    try {
        if (subscriptionTypeComboBox_->currentIndex() <= 0) {
            detailsText_->setText("");
            return;
        }
        
        int typeIndex = subscriptionTypeComboBox_->currentIndex() - 1;
        auto subscriptionTypes = getAvailableSubscriptionTypesFromService();
        
        if (typeIndex < 0 || typeIndex >= static_cast<int>(subscriptionTypes.size())) {
            detailsText_->setText("");
            return;
        }
        
        const auto& selectedType = subscriptionTypes[typeIndex];
        
        std::string details = "<div class='subscription-details-card'>";
        details += "<h3>" + selectedType.name + "</h3>";
        details += "<p><strong>Описание:</strong> " + selectedType.description + "</p>";
        details += "<p><strong>Срок действия:</strong> " + std::to_string(selectedType.validityDays) + " дней</p>";
        
        if (selectedType.unlimited) {
            details += "<p><strong>Посещения:</strong> Безлимит</p>";
        } else {
            details += "<p><strong>Посещения:</strong> " + std::to_string(selectedType.visitCount) + "</p>";
        }
        
        details += "<p><strong>Цена:</strong> " + std::to_string(static_cast<int>(selectedType.price)) + " руб.</p>";
        details += "</div>";
        
        detailsText_->setText(details);
        detailsText_->setTextFormat(Wt::TextFormat::UnsafeXHTML);
        
    } catch (const std::exception& e) {
        std::cerr << "Ошибка обновления деталей абонемента: " << e.what() << std::endl;
        detailsText_->setText("");
    }
}

void PurchaseSubscriptionWidget::updateStatus(const std::string& message, bool isError) {
    statusText_->setText(message);
    if (isError) {
        statusText_->removeStyleClass("status-success");
        statusText_->addStyleClass("status-error");
    } else {
        statusText_->removeStyleClass("status-error");
        statusText_->addStyleClass("status-success");
    }
}

// Вспомогательные методы
std::vector<SubscriptionTypeResponseDTO> PurchaseSubscriptionWidget::getAvailableSubscriptionTypesFromService() {
    try {
        return app_->getSubscriptionController()->getAvailableSubscriptionTypes();
    } catch (const std::exception& e) {
        std::cerr << "Ошибка получения типов абонементов: " << e.what() << std::endl;
        return {};
    }
}

SubscriptionResponseDTO PurchaseSubscriptionWidget::purchaseSubscriptionThroughService(const SubscriptionRequestDTO& request) {
    try {
        return app_->getSubscriptionController()->purchaseSubscription(request);
    } catch (const std::exception& e) {
        std::cerr << "Ошибка покупки абонемента: " << e.what() << std::endl;
        throw;
    }
}

UUID PurchaseSubscriptionWidget::getCurrentClientId() {
    return app_->getCurrentClientId();
}
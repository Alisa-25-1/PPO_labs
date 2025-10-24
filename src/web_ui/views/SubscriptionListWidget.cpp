#include "SubscriptionListWidget.hpp"
#include "../WebApplication.hpp"
#include <Wt/WTimer.h>
#include <Wt/WMessageBox.h>
#include <iostream>
#include <iomanip>
#include <sstream>

SubscriptionListWidget::SubscriptionListWidget(WebApplication* app) 
    : app_(app),
      subscriptionsTable_(nullptr),
      statusText_(nullptr) {
    
    std::cout << "🔧 Создание SubscriptionListWidget..." << std::endl;
    setupUI();
    std::cout << "✅ SubscriptionListWidget создан" << std::endl;
}

void SubscriptionListWidget::setupUI() {
    std::cout << "🔧 Настройка UI SubscriptionListWidget..." << std::endl;
    
    setStyleClass("subscription-list-widget");
    
    auto card = addNew<Wt::WContainerWidget>();
    card->setStyleClass("subscription-card");
    
    // Заголовок
    auto header = card->addNew<Wt::WContainerWidget>();
    header->setStyleClass("subscription-card-header");
    auto headerText = header->addNew<Wt::WText>("<h2>📋 Мои абонементы</h2>");
    headerText->setTextFormat(Wt::TextFormat::UnsafeXHTML);
    
    // Кнопка обновления
    auto refreshBtn = header->addNew<Wt::WPushButton>("🔄 Обновить");
    refreshBtn->setStyleClass("btn-refresh");
    refreshBtn->clicked().connect([this]() {
        loadSubscriptions();
    });
    
    // Таблица абонементов
    auto content = card->addNew<Wt::WContainerWidget>();
    content->setStyleClass("subscription-list-content");
    
    subscriptionsTable_ = content->addNew<Wt::WTable>();
    subscriptionsTable_->setStyleClass("subscription-table");
    
    // Статус
    statusText_ = content->addNew<Wt::WText>();
    statusText_->setStyleClass("subscription-status");
    
    // Загружаем абонементы
    loadSubscriptions();
    
    std::cout << "✅ UI SubscriptionListWidget настроен" << std::endl;
}

void SubscriptionListWidget::loadSubscriptions() {
    try {
        // Очищаем таблицу
        subscriptionsTable_->clear();
        
        // Заголовки таблицы
        subscriptionsTable_->elementAt(0, 0)->addNew<Wt::WText>("<strong>ID типа</strong>")->setTextFormat(Wt::TextFormat::UnsafeXHTML);
        subscriptionsTable_->elementAt(0, 1)->addNew<Wt::WText>("<strong>Период</strong>")->setTextFormat(Wt::TextFormat::UnsafeXHTML);
        subscriptionsTable_->elementAt(0, 2)->addNew<Wt::WText>("<strong>Осталось посещений</strong>")->setTextFormat(Wt::TextFormat::UnsafeXHTML);
        subscriptionsTable_->elementAt(0, 3)->addNew<Wt::WText>("<strong>Статус</strong>")->setTextFormat(Wt::TextFormat::UnsafeXHTML);
        subscriptionsTable_->elementAt(0, 4)->addNew<Wt::WText>("<strong>Действия</strong>")->setTextFormat(Wt::TextFormat::UnsafeXHTML);
        
        // Получаем абонементы текущего пользователя
        auto subscriptions = getClientSubscriptionsFromService();
        
        if (subscriptions.empty()) {
            auto noSubscriptionsRow = subscriptionsTable_->elementAt(1, 0);
            noSubscriptionsRow->setColumnSpan(5);
            noSubscriptionsRow->addNew<Wt::WText>("<div style='text-align: center; padding: 2rem; color: #6c757d;'>У вас пока нет абонементов</div>")->setTextFormat(Wt::TextFormat::UnsafeXHTML);
            return;
        }
        
        int row = 1;
        for (const auto& subscription : subscriptions) {
            // ID типа абонемента
            auto typeCell = subscriptionsTable_->elementAt(row, 0);
            typeCell->addNew<Wt::WText>(subscription.subscriptionTypeId.toString());
            typeCell->setStyleClass("cell-type-id");
            
            // Период действия
            auto periodCell = subscriptionsTable_->elementAt(row, 1);
            std::string periodStr = formatDate(subscription.startDate) + " - " + formatDate(subscription.endDate);
            periodCell->addNew<Wt::WText>(periodStr);
            periodCell->setStyleClass("cell-period");
            
            // Осталось посещений
            std::string visitsStr = subscription.remainingVisits == -1 ? 
                "Безлимит" : std::to_string(subscription.remainingVisits);
            subscriptionsTable_->elementAt(row, 2)->addNew<Wt::WText>(visitsStr);
            
            // Статус с цветовой индикацией
            auto statusCell = subscriptionsTable_->elementAt(row, 3);
            auto statusText = statusCell->addNew<Wt::WText>(getStatusDisplayName(subscription.status));
            statusCell->setStyleClass(getStatusStyleClass(subscription.status));
            
            // Действия
            auto actionsCell = subscriptionsTable_->elementAt(row, 4);
            actionsCell->setStyleClass("cell-actions");
            
            // Кнопка отмены показывается только для активных абонементов
            if (subscription.status == "ACTIVE") {
                auto cancelBtn = actionsCell->addNew<Wt::WPushButton>("❌ Отменить");
                cancelBtn->setStyleClass("btn-cancel-small");
                cancelBtn->clicked().connect([this, subscription]() {
                    handleCancelSubscription(subscription.subscriptionId);
                });
            } else {
                actionsCell->addNew<Wt::WText>("—");
            }
            
            row++;
        }
        
        updateStatus("✅ Загружено абонементов: " + std::to_string(subscriptions.size()), false);
        
    } catch (const std::exception& e) {
        updateStatus("❌ Ошибка загрузки абонементов: " + std::string(e.what()), true);
    }
}

void SubscriptionListWidget::handleCancelSubscription(const UUID& subscriptionId) {
    try {
        // Создаем кастомный диалог подтверждения
        auto dialog = addNew<Wt::WDialog>("Подтверждение отмены");
        dialog->setModal(true);
        dialog->setStyleClass("cancel-dialog");
        
        // Содержимое диалога
        auto content = dialog->contents()->addNew<Wt::WContainerWidget>();
        content->setStyleClass("dialog-content");
        
        // Иконка и текст
        auto icon = content->addNew<Wt::WText>("<div style='text-align: center; font-size: 3rem; margin-bottom: 1rem;'>⚠️</div>");
        icon->setTextFormat(Wt::TextFormat::UnsafeXHTML);
        
        auto message = content->addNew<Wt::WText>(
            "<h3 style='text-align: center; margin-bottom: 1rem;'>Отмена абонемента</h3>"
            "<p style='text-align: center; color: #666;'>Вы уверены, что хотите отменить этот абонемент?</p>"
        );
        message->setTextFormat(Wt::TextFormat::UnsafeXHTML);
        
        // Кнопки
        auto buttonContainer = dialog->footer()->addNew<Wt::WContainerWidget>();
        buttonContainer->setStyleClass("dialog-buttons");
        
        auto cancelBtn = buttonContainer->addNew<Wt::WPushButton>("Нет, оставить");
        cancelBtn->setStyleClass("btn-dialog-secondary");
        cancelBtn->clicked().connect([dialog]() {
            dialog->reject();
        });
        
        auto confirmBtn = buttonContainer->addNew<Wt::WPushButton>("Да, отменить");
        confirmBtn->setStyleClass("btn-dialog-primary");
        confirmBtn->clicked().connect([this, dialog, subscriptionId]() {
            dialog->accept();
            // ИСПРАВЛЕНИЕ: вызываем cancelSubscription напрямую вместо performCancelSubscription
            cancelSubscription(subscriptionId);
        });
        
        // Обработка закрытия диалога
        dialog->finished().connect([this, dialog](Wt::DialogCode code) {
            removeChild(dialog);
        });
        
        dialog->show();
        
    } catch (const std::exception& e) {
        updateStatus("❌ Ошибка при отмене абонемента: " + std::string(e.what()), true);
    }
}

// ИСПРАВЛЕНИЕ: переименовано из performCancelSubscription в cancelSubscription
void SubscriptionListWidget::cancelSubscription(const UUID& subscriptionId) {
    try {
        // Вызываем сервис для отмены абонемента
        bool success = cancelSubscriptionThroughService(subscriptionId);
        
        if (success) {
            updateStatus("✅ Абонемент успешно отменен!", false);
            
            // Перезагружаем список через 1 секунду
            auto timer = std::make_shared<Wt::WTimer>();
            timer->setSingleShot(true);
            timer->setInterval(std::chrono::seconds(1));
            timer->timeout().connect([this, timer]() {
                loadSubscriptions();
            });
            timer->start();
        } else {
            updateStatus("❌ Не удалось отменить абонемент", true);
        }
        
    } catch (const std::exception& e) {
        updateStatus("❌ Ошибка при отмене абонемента: " + std::string(e.what()), true);
    }
}

void SubscriptionListWidget::updateStatus(const std::string& message, bool isError) {
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
std::vector<SubscriptionResponseDTO> SubscriptionListWidget::getClientSubscriptionsFromService() {
    try {
        return app_->getSubscriptionController()->getClientSubscriptions(getCurrentClientId());
    } catch (const std::exception& e) {
        std::cerr << "Ошибка получения абонементов: " << e.what() << std::endl;
        return {};
    }
}

bool SubscriptionListWidget::cancelSubscriptionThroughService(const UUID& subscriptionId) {
    try {
        auto response = app_->getSubscriptionController()->cancelSubscription(subscriptionId, getCurrentClientId());
        return response.status == "CANCELLED";
    } catch (const std::exception& e) {
        std::cerr << "Ошибка отмены абонемента: " << e.what() << std::endl;
        return false;
    }
}

UUID SubscriptionListWidget::getCurrentClientId() {
    return app_->getCurrentClientId();
}

std::string SubscriptionListWidget::formatDate(const std::string& dateStr) {
    // Упрощенное форматирование даты
    if (dateStr.length() >= 10) {
        return dateStr.substr(0, 10); // Берем только дату YYYY-MM-DD
    }
    return dateStr;
}

std::string SubscriptionListWidget::getStatusDisplayName(const std::string& status) {
    static std::map<std::string, std::string> statusMap = {
        {"ACTIVE", "Активен"},
        {"SUSPENDED", "Приостановлен"},
        {"EXPIRED", "Истек"},
        {"CANCELLED", "Отменен"}
    };
    
    auto it = statusMap.find(status);
    return it != statusMap.end() ? it->second : status;
}

std::string SubscriptionListWidget::getStatusStyleClass(const std::string& status) {
    static std::map<std::string, std::string> styleMap = {
        {"ACTIVE", "status-active"},
        {"SUSPENDED", "status-suspended"},
        {"EXPIRED", "status-expired"},
        {"CANCELLED", "status-cancelled"}
    };
    
    auto it = styleMap.find(status);
    return it != styleMap.end() ? it->second : "";
}